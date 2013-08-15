/**\file continuo_burst.c
 * \brief Definição dos métodos para teste udp com tráfego contínuo limitado  em
 * bytes.
 *  
 * \author Alberto Carvalho
 * \date August 14, 2013
 */

#include <modules/continuo_burst.h>
#include <tools/debug.h>
#include <defines.h>

#include <pcap.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/**\struct packet_probe
 * \brief Estrutura de dados para o pacote de teste.
 */
typedef struct {
	short packet_id;
	short packet_total;
	short burst_size;
	short send_rate;
	short packet_size;
} packet_probe;

/**\struct received_probe
 * \brief Estrutura de dados armazenar dados do teste em andamento.
 */
typedef struct {
	int received_total;
	int received_packets;
	struct timeval start;
	struct timeval end;
	struct timeval report_time;
	int report_interval;
	int resize_buffer;
#ifdef MBANDWIDTH
	int mreceived_total;
	int mreceived_packets;
	struct timeval mstart;
#endif
#ifdef CALC_JITTER
	struct timeval tlast;
	double min_jitter;
	double max_jitter;
	double med_jitter;
#endif
	char *log_file;
} received_probe;

/**\struct recv_ctrl
 * \brief Estrutura de dados para controle de timeout na recepção do teste.
 */
typedef struct {
	received_probe *recv_probe;
	pcap_t *handle;
} recv_ctrl;

/**\def WHILE_DELAY_LOOP(t, i)
 * \brief Macro para intervalo entre pacotes.
 * 
 *   Executa um pseudo sleep para ocupar o intervalo entre envio de pacotes,  no
 * final do intervalo calcula o instante de tempo para o próximo envio.
 * 
 * \param t instante de tempo de envio do próximo pacote.
 * \param i intervalo entre o envio de pacotes consecutivos.
 */
#define WHILE_DELAY_LOOP(t, i)	{	struct timeval t_now; \
									int j; \
									gettimeofday(&t_now, NULL); \
									while((t.tv_sec > t_now.tv_sec) || \
										  ((t.tv_sec == t_now.tv_sec) && (t.tv_usec > t_now.tv_usec))) { \
										for(j = 0; j < 1000; j++); \
										gettimeofday(&t_now, NULL); \
									} \
									t.tv_sec = t_now.tv_sec; \
									t.tv_usec = t_now.tv_usec + i; \
								}

/**\fn int get_currentDateTime(char *buffer, int size)
 * \brief Get para data/hora no formato YYYY-MM-DD.HH:MM:SS.
 * 
 * \param buffer Buffer que receberá a string de data/hora.
 * \param size Tamanho do buffer.
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int get_currentDateTime(char *buffer, int size);

/**\fn int resize_socket_buffer(int size)
 * \brief Altera configuração de tamanho do buffer dos sockets no sistema. 
 * 
 * \param size Novo tamanho do buffer de sockets.
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int resize_socket_buffer(int size);

/**\fn int compare_time(struct timeval *t1, struct timeval *t2)
 * \brief Verifica se já transcorreu o tempo para report do teste. Compara  duas
 * struct timeval.
 * 
 * \param t1 Primeiro valor.
 * \param t2 Segundo valor.
 * \return 1 caso t2>t1, 0 caso contrário.
 */
int compare_time(struct timeval *t1, struct timeval *t2);

/**\fn void process_packet (u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char* packet)
 * \brief Callback chamada pela libpcap para processar pacotes recebidos.
 * 
 * \param arg Argumento passado pelo usuário.
 * \param pkthdr Header criado pela libcap com informações adicionais do  pacote
 * (tamanho, timestamp...).
 * \param packet Ponteiro para o pacote recebido.
 */
void process_packet (u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char* packet);

/**\fn void *timeout_thread (void *param)
 * \brief Thread para verificação de timeout na recepção de teste.
 * 
 * \param param informação do teste sendo recebido.
 * \return NULL.
 */
void *timeout_thread (void *param);

/**\fn void print_result(received_probe *probe, int ended)
 * \brief Imprime o resultado atual do teste.
 * 
 * \param probe Estrutura de dados com as informações do teste.
 * \param ended Flag que sinaliza fim de teste.
 */
void print_result(received_probe *probe, int ended);

/**\fn double difftimeval(struct timeval *t1, struct timeval *t2)
 * \brief Calcula a diferença entre instantes de tempo e retorna o resultado.
 * 
 * \param t1 Primeiro instante de tempo.
 * \param t2 Segundo instante de tempo.
 * \return A diferença t2-t1 no formato double.
 */
double difftimeval(struct timeval *t1, struct timeval *t2);


int send_continuo_burst (int port, int soc_buffer_size, int send_rate, int burst_size, int packet_size, char *log_file) {

	struct sockaddr sender_addr;
	struct sockaddr_in *sender_addr_in = (struct sockaddr_in *)&sender_addr;
	int sockfd = -1, i = 0, slen = sizeof(sender_addr), count = 0, p_size = 0, loop_count = 0;
	int resize_buffer = DEFAULT_SOC_BUFFER, interval_packet = 0, packet_num = 0;
	packet_probe *packet;
	struct timeval time_next, time_end;
	char buffer[BUFFER_SIZE];
	DEBUG_MSG(DEBUG_LEVEL_LOW, "Starting send_continuo_burst......................!!\n");
	resize_buffer = soc_buffer_size * 1024;
	if (resize_socket_buffer(resize_buffer)) {
		DEBUG_MSG(DEBUG_LEVEL_LOW, "Could not resize socket buffers!!\n");
		exit(1);
	}

	if (send_rate > 0)
		interval_packet = (packet_size * 8)/(send_rate);
	p_size = packet_size - OVERHEAD_SIZE;

	if (burst_size > 0) {
		packet_num = (burst_size * 1000000)/packet_size;
		if (((burst_size * 1000000)%packet_size) > 0)
			packet_num++;
	}

	DEBUG_MSG(DEBUG_LEVEL_HIGH, "SEND PACKET of %d bytes at %dMbps (packet interval of %dus)\n",
			packet_size, send_rate, interval_packet);

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sender_addr_in->sin_port = htons(port);

	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const void *)&resize_buffer, sizeof(resize_buffer)) != 0) {
		DEBUG_MSG (DEBUG_LEVEL_HIGH, "Could not resize send socket buffers!!\n");
		exit(1);
	}

	memset(buffer, 'B', BUFFER_SIZE);
	gettimeofday(&time_next, NULL);
	time_next = time_end;

	packet = (packet_probe *)buffer;
	packet->packet_id = 0;
	packet->packet_size = packet_size;
	packet->packet_total = packet_num;
	packet->burst_size = burst_size;
	packet->send_rate = send_rate;

	while (loop_count) {
		packet->packet_id++;
		count++;
		loop_count--;
		DEBUG_MSG (DEBUG_LEVEL_HIGH, "SENDING PKT %03d (%d bytes)...\n", i, packet_size);
		if (interval_packet > 0)
			WHILE_DELAY_LOOP(time_next, interval_packet);
		if (sendto(sockfd, buffer, p_size, 0, &sender_addr, slen) == -1) {
			DEBUG_MSG (DEBUG_LEVEL_HIGH, "Could not send packet number %d", i+1);
			exit(1);
		}
	}

	/* sending end test */
	memset(buffer, 'B', BUFFER_SIZE);
	packet = (packet_probe *)buffer;
	packet->packet_id = PACKET_END_TEST;
	packet->packet_total = NPACKET_END_TEST;

	for (i = 0; i < NPACKET_END_TEST; i++) {
		if (sendto (sockfd, buffer, p_size, 0, &sender_addr, slen) == -1) {
			DEBUG_MSG (DEBUG_LEVEL_HIGH, "Could not send packet number %d", i+1);
			exit(1);
		}
		usleep(100);
	}

	/* log expresso! */
	char log_buffer[256];
	memset (log_buffer, 0, 256);
	if (!get_currentDateTime(log_buffer, 256)) {
		LOG_FILE (((log_file != NULL) ? log_file : DEFAULT_SENDCONT_BURST_LOGFILE), "[%s]\t%4d\t%4d\t%5d\t%10d\t%4d\t%4d\t%4d\n",
				log_buffer, resize_buffer/1024, packet_size, count, count*packet_size, send_rate, interval_packet, burst_size);
	}
	else {
		LOG_FILE (((log_file != NULL) ? log_file : DEFAULT_SENDCONT_BURST_LOGFILE), "%4d\t%4d\t%5d\t%10d\t%4d\t%4d\t%4d\n",
				resize_buffer/1024, packet_size, count, count*packet_size, send_rate, interval_packet, burst_size);
	}
	return 0;
	
}

int recv_continuo_burst (int port, int soc_buffer_size, int report_interval, char *device, char *log_file) {
	int sockfd = -1, resize_buffer = DEFAULT_SOC_BUFFER;
	struct sockaddr receiver_addr;
	struct sockaddr_in *receiver_addr_in = (struct sockaddr_in *)&receiver_addr;
	pthread_t timeout_thread_id;
	received_probe recv_probe;
	recv_ctrl timeout_ctrl;
	DEBUG_MSG(DEBUG_LEVEL_LOW, "Starting recv_continuo_burst......................!!\n");
	/* pcap */
	char errbuf[PCAP_ERRBUF_SIZE];
	char expr[BUFFER_SIZE];
	char dev[BUFFER_SIZE];
	pcap_t* descr;
	struct bpf_program fp;        /* hold compiled program */
    bpf_u_int32 maskp;            /* subnet mask */
    bpf_u_int32 netp;             /* ip */

	memset (expr, 0, BUFFER_SIZE);
	memset (dev, 0, BUFFER_SIZE);
	snprintf (dev, BUFFER_SIZE, "%s", DEFAULT_INTERFACE_NAME);

	if (device != NULL) {
		if (pcap_lookupnet(device, &netp, &maskp, errbuf)) {
			DEBUG_MSG (DEBUG_LEVEL_LOW, "Get Device %s error: %s\n", device, errbuf);
			exit(1);					
		}
		else {
			memset (dev, 0, BUFFER_SIZE);
			snprintf (dev, BUFFER_SIZE, "%s", device);
		}
	}

	resize_buffer = soc_buffer_size * 1024;
	if (resize_socket_buffer (resize_buffer)) {
		DEBUG_MSG(DEBUG_LEVEL_LOW, "Could not resize socket buffers!!\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bzero(&receiver_addr, sizeof(receiver_addr));
	receiver_addr_in->sin_family = AF_INET;
	receiver_addr_in->sin_addr.s_addr = 0;
	receiver_addr_in->sin_port = htons(port);

	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const void *)&resize_buffer, sizeof(resize_buffer)) != 0) {
		DEBUG_MSG(DEBUG_LEVEL_LOW, "Could not resize receive socket buffers!!\n");
		exit(1);
	}

	if (bind(sockfd, (const struct sockaddr *)&receiver_addr, sizeof(receiver_addr))==-1) {
		DEBUG_MSG(DEBUG_LEVEL_LOW, "Could not bind!!\n");
		exit(1);
	}

	memset (&recv_probe, 0, sizeof(received_probe));

	/* libpcap filter! */
	snprintf (expr, BUFFER_SIZE, "udp port %d", port);

	/* open device for reading in promiscuous mode */
	descr = pcap_open_live (dev, BUFSIZ, 1, 0, errbuf); 
	if(descr == NULL) {
		DEBUG_MSG(DEBUG_LEVEL_LOW, "pcap_open_live(): %s\n", errbuf);
		exit (1);
	} 
 
	/* Now we'll compile the filter expression*/
	if (pcap_compile (descr, &fp, expr, 0, netp) == -1) {
		DEBUG_MSG(DEBUG_LEVEL_LOW, "Error calling pcap_compile\n");
		exit (1);
	} 

	/* set the filter */
	if (pcap_setfilter (descr, &fp) == -1) {
		DEBUG_MSG(DEBUG_LEVEL_LOW, "Error setting filter\n");
		exit(1);
	}
	
	/* timeout thread */
	recv_probe.log_file = log_file;
	recv_probe.report_interval = report_interval;
	recv_probe.resize_buffer = soc_buffer_size;
	timeout_ctrl.recv_probe = &recv_probe;
	timeout_ctrl.handle = descr;
	if (pthread_create (&timeout_thread_id, NULL, timeout_thread, (void *)&timeout_ctrl)) {
		DEBUG_MSG(DEBUG_LEVEL_LOW, "Could not create timeout thread\n");
		exit (1);
	}

    /* loop for callback function */
    pcap_loop (descr, -1, process_packet, (u_char *)&recv_probe);
	
   return 0;
}

void process_packet (u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
	packet_probe *pkt;
	received_probe *recv_probe = (received_probe *)arg;
	struct timeval tm_now;

	if (pkthdr->len >= (sizeof(packet_probe) + OVERHEAD_SIZE)) {
#ifdef CONECT_4G
		pkt = (packet_probe *)&packet[OVERHEAD_SIZE+2];
#else
		pkt = (packet_probe *)&packet[OVERHEAD_SIZE];
#endif
		tm_now = pkthdr->ts;
		if (pkt->packet_id != PACKET_END_TEST) {
			if (recv_probe->received_total == 0) {
				recv_probe->received_total = pkthdr->len;
				recv_probe->received_packets = 1;
				recv_probe->start = tm_now;
				recv_probe->end = recv_probe->report_time = tm_now;
				recv_probe->report_time.tv_usec += ((recv_probe->report_interval) % 1000) * 1000;
				recv_probe->report_time.tv_sec += (recv_probe->report_interval) / 1000;
				if (recv_probe->report_time.tv_usec >= 1000000) {
					recv_probe->report_time.tv_usec -= 1000000;
					recv_probe->report_time.tv_sec += 1;
				}
			}
			else {
				recv_probe->received_total += pkthdr->len;
				recv_probe->received_packets++;
				recv_probe->end = tm_now;
			}
		}

		if ((compare_time(&(recv_probe->report_time), &(tm_now))) || 
			(pkt->packet_id == PACKET_END_TEST)) {
			print_result (recv_probe, (pkt->packet_id == PACKET_END_TEST));
			exit (0);
		}
		DEBUG_MSG (DEBUG_LEVEL_HIGH, "RECEIVED PKT: %6d BYTES RECEIVED...\n", received + OVERHEAD_SIZE);
	}
}

void *timeout_thread (void *param) {
	int recv_packets = -1;
	struct timeval tm_now, tm_timeout;
	recv_ctrl *timeout_ctrl = (recv_ctrl *)param;
	pcap_t *handle = timeout_ctrl->handle;
	received_probe *recv_probe = timeout_ctrl->recv_probe;

	while (1) {
		if (recv_probe->end.tv_sec != 0) {
			gettimeofday(&tm_now, NULL);
			if (recv_probe->received_packets == recv_packets) {
				tm_timeout = recv_probe->end;
				tm_timeout.tv_usec += (RECVCAPTURE_TIMEOUT * 1000);
				while (tm_timeout.tv_usec >= 1000000) {
					tm_timeout.tv_usec -= 1000000;
					tm_timeout.tv_sec += 1;
				}
				if ((compare_time (&(recv_probe->report_time), &tm_now))
					|| (compare_time (&tm_timeout, &tm_now))) {
					print_result(recv_probe, 1);
					pcap_breakloop(handle);
					exit (0);
					break;
				}
			}
			else {
				recv_packets = recv_probe->received_packets;
			}
		}
		usleep(100000);
	}
	
	return NULL;
}


void print_result(received_probe *probe, int ended) {
	double bandwidth = 0.0, diff = 0.0;
	char buffer[256];
	char *log_file = probe->log_file;

	if (!probe)
		return;

	diff = difftimeval(&(probe->start), &(probe->end));
	if ((probe->received_total > 0) &&
		(probe->received_packets > 0) && (diff > 0.0)) {
		
		bandwidth = (double)(probe->received_total*8)/diff;
		memset(buffer, 0, 256);
		if (!get_currentDateTime(buffer, 256)) {
			LOG_FILE (((log_file != NULL) ? log_file : DEFAULT_RECVCONT_BURST_LOGFILE), "[%s]\t%4d\t%4d\t%5d\t%10d\t%12.4f\t(%09.6f)\n",
					buffer, probe->resize_buffer, probe->received_total/probe->received_packets, probe->received_packets, probe->received_total, bandwidth/1000000, diff);
		}
		else {
			LOG_FILE (((log_file != NULL) ? log_file : DEFAULT_RECVCONT_BURST_LOGFILE), "%4d\t%4d\t%5d\t%10d\t%12.4f\t(%09.6f)\n",
					probe->resize_buffer, probe->received_total/probe->received_packets, probe->received_packets, probe->received_total, bandwidth/1000000, diff);		
		}
	}
}

int get_currentDateTime (char *buffer, int size) {
	time_t now = time (0);
	struct tm  tstruct;

	tstruct = *localtime (&now);
	strftime (buffer, size, "%Y-%m-%d.%X", &tstruct);

	return 0;
}

int resize_socket_buffer (int size) {
	int ret = 0, handle = -1, len = 0;
	char buffer[32];

	memset(buffer, 0, 32);
	len = snprintf (buffer, 32, "%d", size);

	if ((handle = open (WMEM_SBUFFER_FILE, O_WRONLY,  S_IWRITE)) < 0)
		return -1;
	else {
		if (write (handle, buffer, len) < 0)
			ret = -1;
		close(handle);
		if (!ret) {
			if ((handle = open (RMEM_SBUFFER_FILE, O_WRONLY,  S_IWRITE)) < 0)
				return -2;
			else {
				if (write (handle, buffer, len) < 0)
					ret = -2;
				close (handle);
			}
		}
	}
	return ret;
}

int compare_time(struct timeval *t1, struct timeval *t2) {
	if ((t2->tv_sec > t1->tv_sec) ||
		((t2->tv_sec == t1->tv_sec) && (t2->tv_usec >= t1->tv_usec)))
		return 1;
	else
		return 0;
}

double difftimeval(struct timeval *t1, struct timeval *t2) {
	struct timeval time_diff;

	memset(&time_diff, 0, sizeof(struct timeval));
	time_diff.tv_sec = t2->tv_sec - t1->tv_sec;
	time_diff.tv_usec = t2->tv_usec - t1->tv_usec;

	if (time_diff.tv_usec < 0) {
		time_diff.tv_usec += 1000000;
		time_diff.tv_sec -= 1;
		if (time_diff.tv_sec < 0) {	/* ??? -- BSD/OS does this */
			time_diff.tv_sec = 0;
			time_diff.tv_usec = 0;
		}
    }
    return (double)(time_diff.tv_sec)+((double)(time_diff.tv_usec)/1000000.0); 
}
