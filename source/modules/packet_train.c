/**\file packet_train.h
 * \brief Definição de métodos para teste udp com tráfego de trens de pacotes.
 *  
 * \author Alberto Carvalho
 * \date August 29, 2013
 */

#include <modules/packet_train.h>
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

/**\struct probe_info
 * \brief
 */
typedef struct {
	packet_probe pkt;						/**< .>*/
	int size;									/**< .>*/
	timeval32 timestamp;						/**< .>*/
} probe_info;

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
									while (t.tv_usec >= 1000000) { \
										t.tv_usec -= 1000000; \
										t.tv_sec += 1; \
									} \
								}

/**\fn int resize_socket_buffer(int size)
 * \brief Altera configuração de tamanho do buffer dos sockets no sistema. 
 * 
 * \param size Novo tamanho do buffer de sockets.
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int resize_socket_buffer(int size);

/**\fn void process_train_packet (u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char* packet)
 * \brief Callback chamada pela libpcap para processar pacotes recebidos.
 * 
 * \param arg Argumento passado pelo usuário.
 * \param pkthdr Header criado pela libcap com informações adicionais do  pacote
 * (tamanho, timestamp...).
 * \param packet Ponteiro para o pacote recebido.
 */
void process_train_packet (u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char* packet);

/**\fn process_probe (recv_probe, result)
 * \brief Processa o conjunto de pacotes recebidos durante o teste.
 * 
 * \param recv_probe Informações de execução do teste.
 * \param result Resultado calculado do teste.
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int process_probe (received_probe *recv_probe, resume *result);

/**\var probe_info info[MAX_PACKET_PROBES]
 * \brief Buffer com pacotes recebidos do teste.
 * 
 *   Armazena o pacote recebido e seu timestamp, para processamento posterior.
 */
probe_info info[MAX_PACKET_PROBES];

/**\var int received_packets
 * \brief Contador de pacotes recebidos. Utilizado para verificar timeout.
 */
int received_packets = -1;

void *send_packet_train (void *param) {
	settings *conf_settings = (settings *)param;
	struct sockaddr sender_addr;
	struct sockaddr_in *sender_addr_in = (struct sockaddr_in *)&sender_addr;
	int sockfd = -1, i = 0, j = 0, slen = sizeof(sender_addr);
	packet_probe *packet;
	int interval_packet = 0, resize_buffer = 0, yes = 1, p_size = 0;
	struct timeval time_next;
	char buffer[BUFFER_SIZE];

	/* teste! espera 1s antes de começar o envio do teste! 
	 * TODO: TORNAR ESSE SLEEP PARAMETRIZÁVEL */
	usleep (1000000);

	resize_buffer = conf_settings->sendsock_buffer * 1024;
	if (resize_socket_buffer(resize_buffer)) {
		DEBUG_MSG(DEBUG_LEVEL_LOW, "Could not resize socket buffers!!\n");
		return NULL;
	}

	if (conf_settings->udp_rate > 0)
		interval_packet = (conf_settings->packet_size * 8)/(conf_settings->udp_rate);
	p_size = conf_settings->packet_size - OVERHEAD_SIZE;

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sender_addr_in->sin_port = htons(conf_settings->udp_port);
	sender_addr_in->sin_family = AF_INET;
	sender_addr_in->sin_addr.s_addr = conf_settings->address.sin_addr.s_addr;

	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const void *)&resize_buffer, sizeof(resize_buffer)) != 0) {
		fprintf(stderr, "Could not resize send socket buffers!!\n");
		exit(1);
	}

	if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not set socket\n");
		return NULL;
	}

	memset (buffer, 'B', BUFFER_SIZE);
	gettimeofday (&time_next, NULL);

	packet = (packet_probe *)buffer;
	packet->packet_id = 0;
	packet->packet_size = conf_settings->packet_size;
	packet->packet_total = conf_settings->test.train.num * conf_settings->test.train.size;
	packet->burst_size = conf_settings->test.train.num * conf_settings->test.train.size * conf_settings->packet_size;
	packet->send_rate = conf_settings->udp_rate;

	for (j = 0; j < conf_settings->test.train.num; j++) {
		packet->train_id = j + 1;
		packet->train_num = conf_settings->test.train.num;
		for (i = 0; i < conf_settings->test.train.size; i++) {
			packet->packet_id = i + 1;
			//LOG_FILE (DEFAULT_SENDCONT_BURST_LOGFILE, "SENDING PKT %d of TRAIN %d\n", packet->packet_id, packet->train_id);
			if (interval_packet > 0)
				WHILE_DELAY_LOOP(time_next, interval_packet)
			if (sendto(sockfd, buffer, p_size, 0, &sender_addr, slen) == -1) {
				DEBUG_MSG (DEBUG_LEVEL_HIGH, "Could not send packet number %d, train %d\n", i+1, j+1);
				continue;
			}
	   }
	   usleep (conf_settings->test.train.interval * 1000);
	}

	/* sending end test */
	memset (buffer, 'B', BUFFER_SIZE);
	packet = (packet_probe *)buffer;
	packet->train_id = PACKET_END_TEST;
	packet->packet_id = PACKET_END_TEST;
	packet->packet_total = NPACKET_END_TEST;

	for (i = 0; i < NPACKET_END_TEST; i++) {
		if (sendto (sockfd, buffer, p_size, 0, &sender_addr, slen) == -1) {
			DEBUG_MSG (DEBUG_LEVEL_HIGH, "Could not send packet number %d", i+1);
			continue;
		}
		usleep (100);
	}

	close (sockfd);
	
	return NULL;
}

void *recv_packet_train (void *param) {
	data_test *data = (data_test *)param;
	received_probe *recv_probe = &(data->probe[0]);
	settings *conf_settings = data->conf_settings;
	resume *result = data->result;
	int sockfd = -1, resize_buffer = DEFAULT_SOC_BUFFER, yes = 1;
	struct sockaddr receiver_addr;
	struct sockaddr_in *receiver_addr_in = (struct sockaddr_in *)&receiver_addr;
	received_packets = 0;

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

	if (strlen(conf_settings->iface)) {
		if (pcap_lookupnet(conf_settings->iface, &netp, &maskp, errbuf)) {
			DEBUG_MSG (DEBUG_LEVEL_LOW, "Get Device %s error: %s\n", conf_settings->iface, errbuf);
			exit(1);					
		}
		else {
			memset (dev, 0, BUFFER_SIZE);
			snprintf (dev, BUFFER_SIZE, "%s", conf_settings->iface);
		}
	}

	resize_buffer = conf_settings->recvsock_buffer * 1024;
	if (resize_socket_buffer (resize_buffer)) {
		DEBUG_MSG (DEBUG_LEVEL_LOW, "Could not resize socket buffers!!\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bzero(&receiver_addr, sizeof(receiver_addr));
	receiver_addr_in->sin_family = AF_INET;
	receiver_addr_in->sin_addr.s_addr = 0;
	receiver_addr_in->sin_port = htons(conf_settings->udp_port);

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

	/* libpcap filter! */
	snprintf (expr, BUFFER_SIZE, "udp port %d", conf_settings->udp_port);

	/* open device for reading in promiscuous mode */
	descr = pcap_open_live (dev, BUFSIZ, 1, 0, errbuf); 
	if(descr == NULL) {
		DEBUG_MSG(DEBUG_LEVEL_LOW, "pcap_open_live(): %s\n", errbuf);
		exit (1);
	}
	data->handle = (void *)descr;
 
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
	
	/* loop for callback function */
	pcap_loop (descr, -1, process_train_packet, (u_char *)data);

	close(sockfd);
	pcap_close (descr);

	process_probe (recv_probe, result);

	return NULL;
}

void *recv2_packet_train (void *param) {
	data_test *data = (data_test *)param;
	received_probe *recv_probe = &(data->probe[0]);
	settings *conf_settings = data->conf_settings;
	resume *result = data->result;
	int sockfd = -1, resize_buffer = DEFAULT_SOC_BUFFER, yes = 1, received = 0;
	struct sockaddr receiver_addr;
	struct sockaddr_in *receiver_addr_in = (struct sockaddr_in *)&receiver_addr;
	char buffer[BUFFER_SIZE];
	packet_probe *pkt = (packet_probe *)buffer;
	struct timeval tm_now;

	memset (info, 0, (sizeof (probe_info) * MAX_PACKET_PROBES));

	resize_buffer = conf_settings->recvsock_buffer * 1024;
	if (resize_socket_buffer (resize_buffer)) {
		DEBUG_MSG(DEBUG_LEVEL_LOW, "Could not resize socket buffers!!\n");
		return NULL;
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bzero(&receiver_addr, sizeof(receiver_addr));
	receiver_addr_in->sin_family = AF_INET;
	receiver_addr_in->sin_addr.s_addr = 0;
	receiver_addr_in->sin_port = htons(conf_settings->udp_port);

	if (setsockopt (sockfd, SOL_SOCKET, SO_RCVBUF, (const void *)&resize_buffer, sizeof(resize_buffer)) != 0) {
		DEBUG_MSG (DEBUG_LEVEL_LOW, "Could not resize receive socket buffers!!\n");
		return NULL;
	}

	if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not set socket\n");
		exit(1);
	}

	if (bind(sockfd, (const struct sockaddr *)&receiver_addr, sizeof(receiver_addr))==-1) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not bind!!\n");
		return NULL;
	}

	memset (recv_probe, 0, sizeof(received_probe));

	received_packets = 0;
	while (!recv_probe->stop_recv) {
		memset (buffer, 0, BUFFER_SIZE);
		received = recvfrom(sockfd, pkt, BUFFER_SIZE, 0, NULL, NULL);
		ioctl(sockfd, SIOCGSTAMP, &tm_now);
		if ((received >= sizeof(packet_probe)) && (received_packets < MAX_PACKET_PROBES)) {
			if (pkt->train_id != PACKET_END_TEST) {
				memcpy(&(info[received_packets].pkt), pkt, sizeof(packet_probe));
				info[received_packets].timestamp.tv_sec = tm_now.tv_sec;
				info[received_packets].timestamp.tv_usec = tm_now.tv_usec;
				info[received_packets].size = received + OVERHEAD_SIZE;
				received_packets++;
			}
			else {
				recv_probe->stop_recv = 1;
				break;
			}
		}
	}

	close (sockfd);

	process_probe (recv_probe, result);
	
	return NULL;
}

void process_train_packet (u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char* packet) {
	packet_probe *pkt;
	data_test *data = (data_test *)arg;
	received_probe *recv_probe = &(data->probe[0]);
	pcap_t* handle = (pcap_t*)data->handle;

	if (pkthdr->len >= (sizeof(packet_probe) + OVERHEAD_SIZE) &&
		(received_packets < MAX_PACKET_PROBES)) {
#ifdef CONECT_4G
		pkt = (packet_probe *)&packet[OVERHEAD_SIZE+2];
#else
		pkt = (packet_probe *)&packet[OVERHEAD_SIZE];
#endif
		if (pkt->train_id != PACKET_END_TEST) {
			memcpy(&(info[received_packets].pkt), pkt, sizeof(packet_probe));
			info[received_packets].timestamp.tv_sec =  pkthdr->ts.tv_sec;
			info[received_packets].timestamp.tv_usec = pkthdr->ts.tv_usec;
			info[received_packets].size = pkthdr->len;
			received_packets++;
		}
		else {
			recv_probe->stop_recv = 1;
			if (handle != NULL) {
				pcap_breakloop (handle);
			}
		}
	}

	return;
}

void *timeout_train_thread (void *param) {
	data_test *data = (data_test *)param;
	received_probe *recv_probe = &(data->probe[0]);
	int recv_packets = -1;
	struct timeval tm_now;
	timeval32 tm32_now, tm_timeout;
	pcap_t* handle = (pcap_t*)data->handle;

	while (!(recv_probe->stop_recv)) {
		if (received_packets > 0) {
			gettimeofday(&tm_now, NULL);
			tm32_now.tv_sec = tm_now.tv_sec;
			tm32_now.tv_usec = tm_now.tv_usec;
			if (received_packets == recv_packets) {
				tm_timeout = info[received_packets - 1].timestamp;
				tm_timeout.tv_usec += (RECVCAPTURE_TIMEOUT * 2000);
				while (tm_timeout.tv_usec >= 1000000) {
					tm_timeout.tv_usec -= 1000000;
					tm_timeout.tv_sec += 1;
				}
				if ((compare_time (&tm_timeout, &tm32_now))) {
					recv_probe->stop_recv = 1;
					if (handle != NULL) {
						pcap_breakloop (handle);
					}
					break;
				}
			}
			else {
				recv_packets = received_packets;
			}
		}
		usleep(100000);
	}
	
	return NULL;
}

int process_probe (received_probe *recv_probe, resume *result) {
	int ret = 0, i = 0, train_id = 0, jitter = 0;
	timeval32 tm32_now;
#ifdef MBANDWIDTH
	int train_size = 0;
#endif
	if (received_packets <= 0)
		return -1;
	for (i = 0; i < received_packets; i++) {
		if (info[i].size <= 0)
			continue;
		train_id = info[i].pkt.train_id;
		if ((train_id != PACKET_END_TEST) &&
			(train_id >= MIN_TRAIN_NUM) &&
			(train_id <= MAX_TRAIN_NUM)) {
			if (recv_probe[train_id - 1].received_packets > 0) {
				recv_probe[train_id - 1].received_packets++;
				recv_probe[train_id - 1].received_total += info[i].size;
				recv_probe[train_id - 1].end = info[i].timestamp;
#ifdef MBANDWIDTH
				train_size = info[i].pkt.packet_total / info[i].pkt.train_num;
				if (recv_probe[train_id - 1].received_packets == ((train_size / MBANDWIDTH_FACTOR) + 1)) {
					recv_probe[train_id - 1].mreceived_packets = 1;
					recv_probe[train_id - 1].mreceived_total = info[i].size;
					recv_probe[train_id - 1].mstart = info[i].timestamp;
				}
				if (recv_probe[train_id - 1].received_packets > ((train_size / MBANDWIDTH_FACTOR) + 1)) {
					recv_probe[train_id - 1].mreceived_packets++;
					recv_probe[train_id - 1].mreceived_total += info[i].size;
				}
#endif
				/* calcular jitter medio, min e max */
				jitter = difftimeval2us (&tm32_now, &(info[i].timestamp));
				if (jitter > result->jitter_max)
					result->jitter_max = jitter;
				if (jitter < result->jitter_min)
					result->jitter_min = jitter;
				result->jitter_med += jitter;
				tm32_now = info[i].timestamp;
			}
			else {
				recv_probe[train_id - 1].received_packets = 1;
				recv_probe[train_id - 1].received_total = info[i].size;
				recv_probe[train_id - 1].start = info[i].timestamp;
				tm32_now = info[i].timestamp;
			}
		}
	}

	return ret;
}

void print_train_result (settings *conf_settings, received_probe *recv_probe , resume *result) {
	double bw = 0.0, bw_min = 1000000000.0, bw_max = 0.0, bw_med = 0.0;
	double diff = 0.0, diff_min = 1000000.0, diff_max = 0.0, diff_med = 0.0;
	char buffer[256];
	char *log_file = recv_probe->log_file;
	int i = 0, packet_size = 0, train_num = 0;
	received_probe *probe;

#ifdef MBANDWIDTH
	double mbw_min = 1000000000.0, mbw_max = 0.0, mbw_med = 0.0;
	double mdiff_min = 1000000.0, mdiff_max = 0.0, mdiff_med = 0.0;
	int mpacket_size = 0, mtrain_num = 0;
#endif
	if (!recv_probe)
		return;

	for (i = 0; i < MAX_TRAIN_NUM; i++ ) {
		probe = &(recv_probe[i]);

		if (!probe)
			continue;

		diff = difftimeval2db(&(probe->start), &(probe->end));
		if ((probe->received_total > 0) &&
			(probe->received_packets > 0) && (diff > 0.0)) {

			train_num++;

			diff_med += diff;
			if (diff < diff_min)
				diff_min = diff;
			if (diff > diff_max)
				diff_max = diff;

			bw = (double)(probe->received_total*8) / diff;
			bw_med += bw;
			if (bw < bw_min)
				bw_min = bw;
			if (bw > bw_max)
				bw_max = bw;

			packet_size += probe->received_total / probe->received_packets;
			result->packet_num += probe->received_packets;
			result->bytes += probe->received_total;
		}
#ifdef MBANDWIDTH
		diff = difftimeval2db(&(probe->mstart), &(probe->end));
		if ((probe->mreceived_total > 0) &&
			(probe->mreceived_packets > 0) && (diff > 0.0)) {
			train_num++;

			mdiff_med += diff;
			if (diff < mdiff_min)
				mdiff_min = diff;
			if (diff > mdiff_max)
				mdiff_max = diff;

			bw = (double)(probe->received_total*8) / diff;
			mbw_med += bw;
			if (bw < mbw_min)
				mbw_min = bw;
			if (bw > mbw_max)
				mbw_max = bw;
		}
#endif

	}

	/* time */
	diff_med = diff_med / train_num;
	double2timeval (diff_med, &(result->time_med));
	double2timeval (diff_min, &(result->time_min));
	double2timeval (diff_max, &(result->time_max));

	/* bandwidth */
	bw_med = bw_med / train_num;
	double2timeval (bw_med, &(result->bw_med));
	double2timeval (bw_min, &(result->bw_min));
	double2timeval (bw_max, &(result->bw_max));
	
	/* jitter */
	result->jitter_med = result->jitter_med / (result->packet_num - 1);

	/* packet_size */
	result->packet_size = packet_size / train_num;
	

	memset (buffer, 0, 256);
	if (!get_currentDateTime (buffer, 256)) {
		LOG_FILE (((log_file != NULL) ? log_file : DEFAULT_RECVTRAIN_BURST_LOGFILE), "[%s]\t%4d\t%4d\t%5d\t%10d\t%12.4f\t%12.4f\t%12.4f\t%09.6f\t%09.6f\t%09.6f\t%10d\t%10d\t%10d\n",
				buffer, conf_settings->recvsock_buffer, result->packet_size, result->packet_num,
				result->bytes, bw_min/1000000, bw_max/1000000, bw_med/1000000,
				diff_min, diff_max, diff_med, result->jitter_min, result->jitter_max, result->jitter_med);
	}
	else {
		LOG_FILE (((log_file != NULL) ? log_file : DEFAULT_RECVTRAIN_BURST_LOGFILE), "%4d\t%4d\t%5d\t%10d\t%12.4f\t%12.4f\t%12.4f\t%09.6f\t%09.6f\t%09.6f\t%10d\t%10d\t%10d\n",
				conf_settings->recvsock_buffer, result->packet_size, result->packet_num,
				result->bytes, bw_min/1000000, bw_max/1000000, bw_med/1000000,
				diff_min, diff_max, diff_med, result->jitter_min, result->jitter_max, result->jitter_med);
	}

	return;
}

