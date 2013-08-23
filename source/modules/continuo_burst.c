/**\file continuo_burst.c
 * \brief Definição dos métodos para teste udp com tráfego contínuo limitado  em
 * bytes.
 *  
 * \author Alberto Carvalho
 * \date August 14, 2013
 */

#include <modules/continuo_burst.h>
#include <tools/debug.h>
#include <tools/tmath.h>

#include <pcap.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
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
	short packet_id;				/**< Número identificador do pacote. >*/
	short packet_total;				/**< Total de pacotes transmitidos. >*/
	short burst_size;				/**< Total de bytes transmitidos. >*/
	short send_rate;				/**< Taxa de transmissão. >*/
	short packet_size;				/**< Tamanho do pacotes. >*/
} packet_probe;

/**\struct received_probe
 * \brief Estrutura de dados armazenar dados do teste em andamento.
 */
typedef struct {
	int received_total;					/**< Total de bytes recebidos. >*/
	int received_packets;				/**< Total de pacotes recebidos. >*/
	timeval32 start;					/**< Instante de tempo do primeiro pacote recebido. >*/
	timeval32 end;						/**< Instante de tempo do último pacote recebido. >*/
	timeval32 report_time;				/**< Instante de tempo da próxima amostragem. >*/
	int report_interval;				/**< Intervalo entre amostras, em ms. >*/
	int resize_buffer;					/**< Tamanho do buffer de recepção. >*/
	int stop_recv;						/**< Flag para sinalização de fim de teste. >*/
#ifdef MBANDWIDTH
	int mreceived_total;				/**< Total de bytes recebidos na parte final do teste (1/2, 2/3...). >*/
	int mreceived_packets;				/**< Total de pacotes recebidos na parte final do teste (1/2, 2/3...).. >*/
	timeval32 mstart;					/**< Instante de tempo do primeiro pacote recebido (parte final do teste). >*/
#endif
#ifdef CALC_JITTER
	timeval32 tlast;					/**< Instante de tempo de chegada do pacote anterior. >*/
	double min_jitter;					/**< Jitter mínimo. >*/
	double max_jitter;					/**< Jitter máximo. >*/
	double med_jitter;					/**< Jitter médio. >*/
#endif
	void *handle;						/**< Ponteiro para handle do filtro (libpcap). >*/
	char *log_file;						/**< Arquivo para log de execução do teste. >*/
	resume *result;						/**< Resumo de resultados do teste. >*/
} received_probe;

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

/**\fn void print_result (received_probe *probe, int ended)
 * \brief Imprime o resultado atual do teste.
 * 
 * \param probe Estrutura de dados com as informações do teste.
 * \param ended Flag que sinaliza fim de teste.
 */
void print_result (received_probe *probe, int ended);

int send_continuo_burst (int port, int soc_buffer_size, int send_rate, int burst_size, int packet_size, u_long addr, char *log_file) {

	struct sockaddr sender_addr;
	struct sockaddr_in *sender_addr_in = (struct sockaddr_in *)&sender_addr;
	int sockfd = -1, i = 0, slen = sizeof(sender_addr), count = 0, p_size = 0, loop_count = 0, yes = 1;
	int resize_buffer = DEFAULT_SOC_BUFFER, interval_packet = 0, packet_num = 0;
	packet_probe *packet;
	struct timeval time_next;
	char buffer[BUFFER_SIZE];

	/* teste! espera 1s antes de começar o envio do teste! 
	 * TODO: TORNAR ESSE SLEEP PARAMETRIZÁVEL */
	usleep (1000000);

	resize_buffer = soc_buffer_size * 1024;
	if (resize_socket_buffer(resize_buffer)) {
		DEBUG_MSG(DEBUG_LEVEL_LOW, "Could not resize socket buffers!!\n");
		exit(1);
	}

	if (send_rate > 0)
		interval_packet = (packet_size * 8)/(send_rate);
	p_size = packet_size - OVERHEAD_SIZE;

	if (burst_size > 0) {
		loop_count = packet_num = (burst_size * 1000000)/packet_size;
		if (((burst_size * 1000000) % packet_size) > 0) {
			packet_num++;
			loop_count++;
		}
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sender_addr_in->sin_port = htons(port);
	sender_addr_in->sin_family = AF_INET;
	sender_addr_in->sin_addr.s_addr = addr;

	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const void *)&resize_buffer, sizeof(resize_buffer)) != 0) {
		DEBUG_MSG (DEBUG_LEVEL_HIGH, "Could not resize send socket buffers!!\n");
		exit(1);
	}

	if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not set socket\n");
		exit(1);
	}

	memset(buffer, 'B', BUFFER_SIZE);
	gettimeofday(&time_next, NULL);

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

	close (sockfd);

	return 0;
	
}

int recv_continuo_burst (int port, int soc_buffer_size, int report_interval, char *device, char *log_file, resume *result) {
	int sockfd = -1, resize_buffer = DEFAULT_SOC_BUFFER, yes = 1;
	struct sockaddr receiver_addr;
	struct sockaddr_in *receiver_addr_in = (struct sockaddr_in *)&receiver_addr;
	pthread_t timeout_thread_id;
	received_probe recv_probe;

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
		DEBUG_MSG (DEBUG_LEVEL_LOW, "Could not resize socket buffers!!\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bzero(&receiver_addr, sizeof(receiver_addr));
	receiver_addr_in->sin_family = AF_INET;
	receiver_addr_in->sin_addr.s_addr = 0;
	receiver_addr_in->sin_port = htons(port);

	if (setsockopt (sockfd, SOL_SOCKET, SO_RCVBUF, (const void *)&resize_buffer, sizeof(resize_buffer)) != 0) {
		DEBUG_MSG (DEBUG_LEVEL_LOW, "Could not resize receive socket buffers!!\n");
		exit(1);
	}

	if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not set socket\n");
		exit(1);
	}

	if (bind(sockfd, (const struct sockaddr *)&receiver_addr, sizeof(receiver_addr))==-1) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not bind!!\n");
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
	recv_probe.handle = (void *)descr;
	recv_probe.result = result;

	if (pthread_create (&timeout_thread_id, NULL, timeout_thread, (void *)&recv_probe)) {
		DEBUG_MSG(DEBUG_LEVEL_LOW, "Could not create timeout thread\n");
		exit (1);
	}

	/* loop for callback function */
	pcap_loop (descr, -1, process_packet, (u_char *)&recv_probe);

	close(sockfd);

	return 0;
}

int recv2_continuo_burst (int port, int soc_buffer_size, int report_interval, char *device, char *log_file, resume *result) {
	int sockfd = -1, resize_buffer = DEFAULT_SOC_BUFFER, yes = 1, received = 0;
	struct sockaddr receiver_addr;
	struct sockaddr_in *receiver_addr_in = (struct sockaddr_in *)&receiver_addr;
	pthread_t timeout_thread_id;
	received_probe recv_probe;
	char buffer[BUFFER_SIZE];
	packet_probe *pkt = (packet_probe *)buffer;
	struct timeval tm_now;
	timeval32 tm32_now;
	int jitter = 0;

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

	if (setsockopt (sockfd, SOL_SOCKET, SO_RCVBUF, (const void *)&resize_buffer, sizeof(resize_buffer)) != 0) {
		DEBUG_MSG (DEBUG_LEVEL_LOW, "Could not resize receive socket buffers!!\n");
		exit(1);
	}

	if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not set socket\n");
		exit(1);
	}

	if (bind(sockfd, (const struct sockaddr *)&receiver_addr, sizeof(receiver_addr))==-1) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not bind!!\n");
		exit(1);
	}

	memset (&recv_probe, 0, sizeof(received_probe));
	memset (buffer, 0, BUFFER_SIZE);

	/* timeout thread */
	recv_probe.log_file = log_file;
	recv_probe.report_interval = report_interval;
	recv_probe.resize_buffer = soc_buffer_size;
	//recv_probe.handle = (void *)descr;
	recv_probe.result = result;

	if (pthread_create (&timeout_thread_id, NULL, timeout_thread, (void *)&recv_probe)) {
		DEBUG_MSG(DEBUG_LEVEL_LOW, "Could not create timeout thread\n");
		exit (1);
	}

	while (1) {
		received = recvfrom(sockfd, pkt, BUFFER_SIZE, 0, NULL, NULL);
		ioctl(sockfd, SIOCGSTAMP, &tm_now);
		tm32_now.tv_sec = tm_now.tv_sec;
		tm32_now.tv_usec = tm_now.tv_usec;
		if ((received > sizeof(packet_probe)) &&
			(pkt->packet_id != PACKET_END_TEST)) {
			if (recv_probe.received_total == 0) {
				recv_probe.received_total = received + OVERHEAD_SIZE;
				recv_probe.received_packets = 1;
				recv_probe.start = tm32_now;
				recv_probe.end = recv_probe.report_time = tm32_now;
				recv_probe.report_time.tv_usec += ((recv_probe.report_interval) % 1000) * 1000;
				recv_probe.report_time.tv_sec += (recv_probe.report_interval) / 1000;
				if (recv_probe.report_time.tv_usec >= 1000000) {
					recv_probe.report_time.tv_usec -= 1000000;
					recv_probe.report_time.tv_sec += 1;
				}
			}
			else {
				/* calcular jitter medio, min e max */
				jitter = difftimeval2us (&(recv_probe.end), &tm32_now);
				if (jitter > result->jitter_max)
					result->jitter_max = jitter;
				if (jitter < result->jitter_min)
					result->jitter_min = jitter;
				result->jitter_med += jitter;
				recv_probe.received_total += received + OVERHEAD_SIZE;
				recv_probe.received_packets++;
				recv_probe.end = tm32_now;
			}
		}

		if ((compare_time(&(recv_probe.report_time), &(tm32_now))) || 
			(pkt->packet_id == PACKET_END_TEST)) {
			print_result (&recv_probe, (pkt->packet_id == PACKET_END_TEST));
			recv_probe.stop_recv = 1;
			break;
		}
	}

	close (sockfd);

	return 0;
}

void process_packet (u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
	packet_probe *pkt;
	received_probe *recv_probe = (received_probe *)arg;
	resume *result = recv_probe->result;
	timeval32 tm32_now;
	int jitter = 0;
	pcap_t* handle = (pcap_t*)recv_probe->handle;

	if (pkthdr->len >= (sizeof(packet_probe) + OVERHEAD_SIZE)) {
#ifdef CONECT_4G
		pkt = (packet_probe *)&packet[OVERHEAD_SIZE+2];
#else
		pkt = (packet_probe *)&packet[OVERHEAD_SIZE];
#endif
		tm32_now.tv_sec = pkthdr->ts.tv_sec;
		tm32_now.tv_usec = pkthdr->ts.tv_usec;
		if (pkt->packet_id != PACKET_END_TEST) {
			if (recv_probe->received_total == 0) {
				recv_probe->received_total = pkthdr->len;
				recv_probe->received_packets = 1;
				recv_probe->start = tm32_now;
				recv_probe->end = recv_probe->report_time = tm32_now;
				recv_probe->report_time.tv_usec += ((recv_probe->report_interval) % 1000) * 1000;
				recv_probe->report_time.tv_sec += (recv_probe->report_interval) / 1000;
				if (recv_probe->report_time.tv_usec >= 1000000) {
					recv_probe->report_time.tv_usec -= 1000000;
					recv_probe->report_time.tv_sec += 1;
				}
			}
			else {
				/* calcular jitter medio, min e max */
				jitter = difftimeval2us (&(recv_probe->end), &tm32_now);
				if (jitter > result->jitter_max)
					result->jitter_max = jitter;
				if (jitter < result->jitter_min)
					result->jitter_min = jitter;
				result->jitter_med += jitter;
				recv_probe->received_total += pkthdr->len;
				recv_probe->received_packets++;
				recv_probe->end = tm32_now;
			}
		}
		if ((pkt->packet_id == PACKET_END_TEST) ||
			(compare_time (&(recv_probe->report_time), &(tm32_now)))) {
			print_result (recv_probe, (pkt->packet_id == PACKET_END_TEST));
			recv_probe->stop_recv = 1;
			pcap_breakloop (handle);
		}
	}
}

void *timeout_thread (void *param) {
	int recv_packets = -1;
	struct timeval tm_now;
	timeval32 tm32_now, tm_timeout;
	received_probe *recv_probe = (received_probe *)param;
	pcap_t* handle = (pcap_t*)recv_probe->handle;

	while (!(recv_probe->stop_recv)) {
		if (recv_probe->end.tv_sec != 0) {
			gettimeofday(&tm_now, NULL);
			tm32_now.tv_sec = tm_now.tv_sec;
			tm32_now.tv_usec = tm_now.tv_usec;
			if (recv_probe->received_packets == recv_packets) {
				tm_timeout = recv_probe->end;
				tm_timeout.tv_usec += (RECVCAPTURE_TIMEOUT * 1000);
				while (tm_timeout.tv_usec >= 1000000) {
					tm_timeout.tv_usec -= 1000000;
					tm_timeout.tv_sec += 1;
				}
				if ((compare_time (&tm_timeout, &tm32_now))) {
					print_result (recv_probe, 1);
					pcap_breakloop (handle);
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
	resume *result = probe->result;

	if (!probe)
		return;

	diff = difftimeval2db(&(probe->start), &(probe->end));
	if ((probe->received_total > 0) &&
		(probe->received_packets > 0) && (diff > 0.0)) {
		
		bandwidth = (double)(probe->received_total*8)/diff;

		result->packet_size = probe->received_total/probe->received_packets;
		result->packet_num = probe->received_packets;
		result->bytes = probe->received_total;
		/* double to timeval....*/
		double2timeval(bandwidth, &(result->bw_med));
		/* timeval - timeval */
		difftimeval ( &(probe->start), &(probe->end), &(result->time_med));
		result->jitter_med = result->jitter_med/(probe->received_packets - 1);

		memset(buffer, 0, 256);
		if (!get_currentDateTime(buffer, 256)) {
			LOG_FILE (((log_file != NULL) ? log_file : DEFAULT_RECVCONT_BURST_LOGFILE), "[%s]\t%4d\t%4d\t%5d\t%10d\t%12.4f\t%09.6f\n",
					buffer, probe->resize_buffer, probe->received_total/probe->received_packets, probe->received_packets, probe->received_total, bandwidth/1000000, diff);
		}
		else {
			LOG_FILE (((log_file != NULL) ? log_file : DEFAULT_RECVCONT_BURST_LOGFILE), "%4d\t%4d\t%5d\t%10d\t%12.4f\t%09.6f\n",
					probe->resize_buffer, probe->received_total/probe->received_packets, probe->received_packets, probe->received_total, bandwidth/1000000, diff);
		}
	}
	return;
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
