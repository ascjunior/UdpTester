/**\file recvtest_thread.c
 * \brief Definição de métodos para controle de testes udp.
 *  
 * \author Alberto Carvalho
 * \date August 02, 2013.
 */


#include <defines.h>
#include <tools/debug.h>
#include <threads/recvtest_thread.h>

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
	ctrl_queue *recv_queue = &(conf_settings->recv_queue);
	ctrl_pkt **pkt_queue = recv_queue->queue;
	ctrlpacket *data;
	report pkt_report;

	/* teste fake (por enquanto)...
	 *   -- sleep de teste...
	 *     -- criar relatório fake
	 *     -- colocar relatório fake em pkt e add a stack de msg de controle para envio
	 *     -- cria msg de inicio de teste de up e add a stack de msg de controle
	 */

	/* sleep para simular teste */
	usleep (5000000);

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
	pkt_report.bw_med.tv_sec = 0;
	pkt_report.bw_med.tv_usec = 504242;

	if ((pkt_queue[recv_queue->start] != NULL))
		recv_queue->start = (recv_queue->start + 1) % MAX_CTRL_QUEUE;

	if (pkt_queue[recv_queue->start] == NULL) {
		pkt_queue[recv_queue->start] = (ctrl_pkt *)malloc(sizeof(ctrl_pkt));
		pkt_queue[recv_queue->start]->data = (ctrlpacket *)malloc(sizeof(ctrlpacket));
	}
	pkt_queue[recv_queue->start]->valid = 1;
	data = pkt_queue[recv_queue->start]->data;
	data->cp_type = FEEDBACK_TEST_DOWN;
	data->packet_size = sizeof(report);
	memcpy (data->buffer, &pkt_report, sizeof(report));
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "ADD MSG FEEDBACK_TEST_DOWN to RECV MSG STACK %d\n", recv_queue->start);

	return NULL;
}
