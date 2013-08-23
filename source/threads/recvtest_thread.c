/**\file recvtest_thread.c
 * \brief Definição de métodos para controle de testes udp.
 *  
 * \author Alberto Carvalho
 * \date August 02, 2013.
 */


#include <defines.h>
#include <tools/debug.h>
#include <threads/recvtest_thread.h>
#include <modules/continuo_burst.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


void *recvtest_thread (void *param) {
	settings *conf_settings = (settings *)param;
	ctrl_queue *send_queue = &(conf_settings->send_queue);
	ctrl_queue *recv_queue = &(conf_settings->recv_queue);
	ctrl_pkt **pkt_queue = send_queue->queue;
	ctrlpacket *data;
	report pkt_report;
	resume *result = &(pkt_report.result);

	memset (&pkt_report, 0, sizeof(report));
	result->jitter_min = 10000000;
	result->bw_min.tv_sec = 1000000000;

	if (conf_settings->t_type == UDP_CONTINUO) {
		recv2_continuo_burst (conf_settings->udp_port,
				conf_settings->recvsock_buffer,
				conf_settings->test.cont.report_interval, conf_settings->iface, NULL, result);
		//usleep (6000000);
	}
	else
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Could'nt start send test: test type (%d) != continuo (%d)\n",
							conf_settings->t_type, UDP_CONTINUO);

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
	pkt_report.test.cont.report_interval	= conf_settings->test.cont.report_interval;

	/* resultados */
	result->loss_med = pkt_report.test.cont.pkt_num - result->packet_num;

	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "SEND REPORT TEST...\n");
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "MODE: %s\n",
		(conf_settings->mode == SENDER) ? "SENDER" : (conf_settings->mode == RECEIVER) ? "RECEIVER" : "UNKNOW");
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "TEST TYPE             : %s\n",
		(conf_settings->t_type == UDP_CONTINUO) ? "CONTINUO" : (conf_settings->t_type == UDP_PACKET_TRAIN) ? "TRAIN" : "UNKNOW");
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "PACKET SIZE           : %d\n", result->packet_size);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "PACKET NUM            : %d\n", result->packet_num);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "TOTAL BYTES           : %d\n", result->bytes);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "UDP PORT              : %d\n", pkt_report.udp_port);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "BW MED                : %d.%06d\n",
						result->bw_med.tv_sec, result->bw_med.tv_usec);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "JITTER (med, min, max): %dus\t%dus\t%dus\n",
						result->jitter_med, result->jitter_min, result->jitter_max);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "LOSS                  : %d\n", result->loss_med);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "TIME                  : %d.%06d\n",
						result->time_med.tv_sec, result->time_med.tv_usec);

	pthread_mutex_lock (&(send_queue->mutex));
	
	send_queue->start = (send_queue->start + 1) % MAX_CTRL_QUEUE;
	pkt_queue[send_queue->start]->valid = 1;
	data = &(pkt_queue[send_queue->start]->data);
	if (conf_settings->mode == RECEIVER) {
		data->cp_type = FEEDBACK_TEST_DOWN;
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "ADD MSG FEEDBACK_TEST_DOWN to SEND MSG STACK %d (%lu)\n", send_queue->start, sizeof(report));
	}
	else {
		data->cp_type = FEEDBACK_TEST_UP;
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "ADD MSG FEEDBACK_TEST_UP to SEND MSG STACK %d\n", send_queue->start);
	}
	data->packet_size = sizeof(ctrlpacket);
	memcpy (data->buffer, &pkt_report, sizeof(report));

	pthread_mutex_unlock (&(send_queue->mutex));

	if (conf_settings->mode == RECEIVER) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "conf_settings->mode == RECEIVER, SET MSG START_TEST_UP\n");
		pthread_mutex_lock (&(recv_queue->mutex));

		pkt_queue = recv_queue->queue;
		recv_queue->start = (recv_queue->start + 1) % MAX_CTRL_QUEUE;
		pkt_queue[recv_queue->start]->valid = 1;
		data = &(pkt_queue[recv_queue->start]->data);
		data->cp_type = START_TEST_UP;
		data->packet_size = sizeof (report);

		pthread_mutex_unlock (&(recv_queue->mutex));
	}
	else {
		pthread_mutex_lock (&(recv_queue->mutex));

		pkt_queue = recv_queue->queue;
		recv_queue->start = (recv_queue->start + 1) % MAX_CTRL_QUEUE;
		pkt_queue[recv_queue->start]->valid = 1;
		data = &(pkt_queue[recv_queue->start]->data);
		data->cp_type = LOOP_TEST_INTERVAL;
		data->packet_size = sizeof(report);

		pthread_mutex_unlock (&(recv_queue->mutex));
	}

	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "return NULL...\n");
	return NULL;
}
