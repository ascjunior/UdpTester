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


void *recvtest_thread (void *param) {
	settings *conf_settings = (settings *)param;
	ctrl_queue *send_queue = &(conf_settings->send_queue);
	ctrl_queue *recv_queue = &(conf_settings->recv_queue);
	ctrl_pkt **pkt_queue = send_queue->queue;
	ctrlpacket *data;
	report pkt_report;

	/* teste fake (por enquanto)...
	 *   -- sleep de teste...
	 *     -- criar relatório fake
	 *     -- colocar relatório fake em pkt e add a stack de msg de controle para envio
	 *     -- cria msg de inicio de teste de up e add a stack de msg de controle
	 */

	/* sleep para simular teste */
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "RECV TEST THREAD FAKE... SLEEP by 5s\n");
	//usleep (5000000);

	if (conf_settings->t_type == UDP_CONTINUO) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "CALL recv_continuo_burst\n");
		recv_continuo_burst (conf_settings->udp_port,
				conf_settings->recvsock_buffer,
				conf_settings->test.cont.report_interval, NULL, NULL);
	}
	else
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Could'nt start send test: test type (%d) != continuo (%d)\n",
							conf_settings->t_type, UDP_CONTINUO);
	/* criar relatório fake */
	memset (&pkt_report, 0, sizeof(report));
	pkt_report.t_type = UDP_CONTINUO;
	pkt_report.packet_size = 1280;
	pkt_report.packet_num = 1504;
	pkt_report.bytes = 1925120;
	pkt_report.udp_port = 33333;
	pkt_report.bw_med.tv_sec = 25;
	pkt_report.bw_med.tv_usec = 424242;
	pkt_report.jitter_med = 1250;
	pkt_report.time_med.tv_sec = 0;
	pkt_report.time_med.tv_usec = 504242;

	if ((pkt_queue[send_queue->start] != NULL))
		send_queue->start = (send_queue->start + 1) % MAX_CTRL_QUEUE;

	if (pkt_queue[send_queue->start] == NULL) {
		pkt_queue[send_queue->start] = (ctrl_pkt *)malloc(sizeof(ctrl_pkt));
		pkt_queue[send_queue->start]->data = (ctrlpacket *)malloc(sizeof(ctrlpacket));
	}
	pkt_queue[send_queue->start]->valid = 1;
	data = pkt_queue[send_queue->start]->data;
	if (conf_settings->mode == RECEIVER) {
		data->cp_type = FEEDBACK_TEST_DOWN;
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "ADD MSG FEEDBACK_TEST_DOWN to SEND MSG STACK %d\n", send_queue->start);
	}
	else {
		data->cp_type = FEEDBACK_TEST_UP;
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "ADD MSG FEEDBACK_TEST_UP to SEND MSG STACK %d\n", send_queue->start);
	}
	data->packet_size = sizeof(report);
	memcpy (data->buffer, &pkt_report, sizeof(report));

	if (conf_settings->mode == RECEIVER) {
		send_queue->start = (send_queue->start + 1) % MAX_CTRL_QUEUE;
		if (pkt_queue[send_queue->start] == NULL) {
			pkt_queue[send_queue->start] = (ctrl_pkt *)malloc(sizeof(ctrl_pkt));
			pkt_queue[send_queue->start]->data = (ctrlpacket *)malloc(sizeof(ctrlpacket));
		}
		pkt_queue[send_queue->start]->valid = 1;
		data = pkt_queue[send_queue->start]->data;
		data->cp_type = START_TEST_UP;
		data->packet_size = sizeof(report);
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "ADD MSG START_TEST_UP to SEND MSG STACK %d\n", send_queue->start);
	}
	else {
		pkt_queue = recv_queue->queue;
		if ((pkt_queue[recv_queue->start] != NULL))
			recv_queue->start = (recv_queue->start + 1) % MAX_CTRL_QUEUE;
		if (pkt_queue[recv_queue->start] == NULL) {
			pkt_queue[recv_queue->start] = (ctrl_pkt *)malloc(sizeof(ctrl_pkt));
			pkt_queue[recv_queue->start]->data = (ctrlpacket *)malloc(sizeof(ctrlpacket));
		}
		pkt_queue[recv_queue->start]->valid = 1;
		data = pkt_queue[recv_queue->start]->data;
		data->cp_type = LOOP_TEST_INTERVAL;
		data->packet_size = sizeof(report);
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "ADD MSG LOOP_TEST_INTERVAL to RECV MSG STACK %d\n", recv_queue->start);
	}

	return NULL;
}
