/**\file recvctrl.c
 * \brief Definição de métodos para controle da recepção de testes.
 *  
 * \author Alberto Carvalho
 * \date August 28, 2013
 */
 
#include <defines.h>
#include <ctrl/recvctrl.h>
#include <modules/continuo_burst.h>
#include <tools/debug.h>
#include <tools/tmath.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 
#include <pthread.h>

/**\fn void print_result (received_probe *probe, int ended)
 * \brief Imprime o resultado atual do teste.
 * 
 * \param probe Estrutura de dados com as informações do teste.
 * \param ended Flag que sinaliza fim de teste.
 */
void print_result (received_probe *probe, resume *result);

int recvctrl (settings *conf_settings) {
	int ret = 0, timeout = 0;
	data_test data;
	ctrl_queue *send_queue = &(conf_settings->send_queue);
	ctrl_queue *recv_queue = &(conf_settings->recv_queue);
	ctrl_pkt **pkt_queue = send_queue->queue;
	ctrlpacket *ctrlpkt;
	report pkt_report;
	received_probe probe;
	pthread_t recvtest_thread_id;
	pthread_t timeout_thread_id;

	memset (&pkt_report, 0, sizeof (report));
	memset (&probe, 0, sizeof (received_probe));

	data.conf_settings = conf_settings;
	data.result = &(pkt_report.result);
	data.probe = &probe;
	data.handle = NULL;

	/* Determina tipo de teste */
	switch (conf_settings->t_type) {
		case UDP_CONTINUO:
				if ((ret = pthread_create (&recvtest_thread_id, NULL, recv_continuo_burst, (void *)&data))) {
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not create recvtest thread: %s\n",
					(ret == EAGAIN) ? "EAGAIN" : (ret == EINVAL) ? "EINVAL" : (ret == EPERM) ? "EPERM" : "UNKNOW");
				}
			break;
		default:
			DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Invalid test type\n");
	}
	
	if (ret)
		return ret;

	/* pacote de feedback */
	pthread_mutex_lock (&(send_queue->mutex));

	send_queue->start = (send_queue->start + 1) % MAX_CTRL_QUEUE;
	pkt_queue[send_queue->start]->valid = 1;
	ctrlpkt = &(pkt_queue[send_queue->start]->data);
	if (conf_settings->mode == RECEIVER) {
		ctrlpkt->cp_type = FEEDBACK_START_TEST_DOWN;
	}
	else {
		ctrlpkt->cp_type = FEEDBACK_START_TEST_UP;
	}
	ctrlpkt->packet_size = sizeof(ctrlpacket);

	pthread_mutex_unlock (&(send_queue->mutex));

	/* thread de timeout */
	if ((timeout = pthread_create (&timeout_thread_id, NULL, timeout_thread, (void *)&data))) {
		DEBUG_MSG(DEBUG_LEVEL_LOW, "Could not create timeout thread\n");
	}

	/* join de espera */
	pthread_join (recvtest_thread_id, NULL);

	if (!timeout)
		pthread_join (timeout_thread_id, NULL);

	/* configurações do teste */
	pkt_report.t_type = conf_settings->t_type;
	pkt_report.packet_size = conf_settings->packet_size;
	pkt_report.udp_rate = conf_settings->udp_rate;
	pkt_report.udp_port = conf_settings->udp_port;
	pkt_report.test.cont.size = conf_settings->test.cont.size;
	pkt_report.test.cont.pkt_num = (conf_settings->test.cont.size * 1000000) / conf_settings->packet_size;
	if (((conf_settings->test.cont.size * 1000000) % conf_settings->packet_size))
		pkt_report.test.cont.pkt_num++;
	pkt_report.test.cont.interval = conf_settings->test.cont.interval;
	pkt_report.test.cont.report_interval = conf_settings->test.cont.report_interval;

	/* resultados */
	pkt_report.result.loss_med = pkt_report.test.cont.pkt_num - probe.received_packets;
	
	/* log dos resultados */
	print_result (data.probe, data.result);

/*	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "SEND REPORT TEST...\n");
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "MODE: %s\n",
		(conf_settings->mode == SENDER) ? "SENDER" : (conf_settings->mode == RECEIVER) ? "RECEIVER" : "UNKNOW");
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "TEST TYPE             : %s\n",
		(conf_settings->t_type == UDP_CONTINUO) ? "CONTINUO" : (conf_settings->t_type == UDP_PACKET_TRAIN) ? "TRAIN" : "UNKNOW");
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "PACKET SIZE           : %d\n", data.result->packet_size);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "PACKET NUM            : %d\n", data.result->packet_num);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "TOTAL BYTES           : %d\n", data.result->bytes);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "UDP PORT              : %d\n", pkt_report.udp_port);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "BW MED                : %d.%06d\n",
						data.result->bw_med.tv_sec, data.result->bw_med.tv_usec);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "JITTER (med, min, max): %dus\t%dus\t%dus\n",
						data.result->jitter_med, data.result->jitter_min, data.result->jitter_max);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "LOSS                  : %d\n", data.result->loss_med);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "TIME                  : %d.%06d\n",
						data.result->time_med.tv_sec, data.result->time_med.tv_usec);
*/
	pthread_mutex_lock (&(send_queue->mutex));
	
	send_queue->start = (send_queue->start + 1) % MAX_CTRL_QUEUE;
	pkt_queue[send_queue->start]->valid = 1;
	ctrlpkt = &(pkt_queue[send_queue->start]->data);
	if (conf_settings->mode == RECEIVER) {
		ctrlpkt->cp_type = FEEDBACK_TEST_DOWN;
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "ADD MSG FEEDBACK_TEST_DOWN to SEND MSG STACK %d (%lu)\n", send_queue->start, sizeof(report));
	}
	else {
		ctrlpkt->cp_type = FEEDBACK_TEST_UP;
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "ADD MSG FEEDBACK_TEST_UP to SEND MSG STACK %d\n", send_queue->start);
	}
	ctrlpkt->packet_size = sizeof(ctrlpacket);
	memcpy (ctrlpkt->buffer, &pkt_report, sizeof(report));

	pthread_mutex_unlock (&(send_queue->mutex));

	if (conf_settings->mode == RECEIVER) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "conf_settings->mode == RECEIVER, SET MSG START_TEST_UP\n");
		pthread_mutex_lock (&(recv_queue->mutex));

		send_queue->start = (send_queue->start + 1) % MAX_CTRL_QUEUE;
		pkt_queue[send_queue->start]->valid = 1;
		ctrlpkt = &(pkt_queue[send_queue->start]->data);
		ctrlpkt->cp_type = START_TEST_UP;
		ctrlpkt->packet_size = sizeof (report);

		pthread_mutex_unlock (&(recv_queue->mutex));
	}
	else {
		pthread_mutex_lock (&(recv_queue->mutex));

		pkt_queue = recv_queue->queue;
		recv_queue->start = (recv_queue->start + 1) % MAX_CTRL_QUEUE;
		pkt_queue[recv_queue->start]->valid = 1;
		ctrlpkt = &(pkt_queue[recv_queue->start]->data);
		ctrlpkt->cp_type = LOOP_TEST_INTERVAL;
		ctrlpkt->packet_size = sizeof(report);

		pthread_mutex_unlock (&(recv_queue->mutex));
	}

	return ret;
}

void print_result (received_probe *probe , resume *result) {
	double bandwidth = 0.0, diff = 0.0;
	char buffer[256];
	char *log_file = probe->log_file;

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

		memset (buffer, 0, 256);
		if (!get_currentDateTime (buffer, 256)) {
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
