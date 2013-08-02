/**\file sendctrl_thread.c
 * \brief Definição de métodos para gerenciamento de mensagens  de  controle  do
 * UdpTester (módulo para enviar mensagens).
 *  
 * \author Alberto Carvalho
 * \date July 05, 2013.
 */

#include <defines.h>
#include <tools/debug.h>
#include <threads/sendctrl_thread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <asm/sockios.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <time.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 
#include <signal.h>
#include <pthread.h>


void *sendctrl_thread (void *param) {
	settings *conf_settings = (settings *)param;
	ctrl_queue *send_queue = &(conf_settings->send_queue);
	ctrl_queue *recv_queue = &(conf_settings->recv_queue);
	ctrl_pkt **pkt_queue = send_queue->queue;
	ctrl_pkt *pkt;
	ctrlpacket *data;
    struct sockaddr_in server_info;
    int socket_fd;

	DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Starting send ctrl thread...\n");

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0))== -1) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Could create socket\n");
		exit (1);
	}

	memset (&server_info, 0, sizeof(server_info));
	server_info.sin_family = AF_INET;
	if (conf_settings->mode == RECEIVER)
		server_info.sin_port = htons(SEND_PORT);
	else
		server_info.sin_port = htons(RECV_PORT);
	server_info.sin_addr = conf_settings->address.sin_addr;
	if (connect (socket_fd, (struct sockaddr *)&server_info, sizeof(struct sockaddr)) < 0) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Connection Failure\n");
		exit (1);
	}
	else {
		/* sinalizar na fila de mensagens recebidas que a conexão foi estabelecida */
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Connection Established\n");
		pkt_queue = recv_queue->queue;

		if ((pkt_queue[recv_queue->start] != NULL))
			recv_queue->start = (recv_queue->start + 1) % MAX_CTRL_QUEUE;

		if (pkt_queue[recv_queue->start] == NULL) {
			pkt_queue[recv_queue->start] = (ctrl_pkt *)malloc(sizeof(ctrl_pkt));
			pkt_queue[recv_queue->start]->data = (ctrlpacket *)malloc(sizeof(ctrlpacket));
		}
		pkt_queue[recv_queue->start]->valid = 1;
		data = pkt_queue[recv_queue->start]->data;
		data->cp_type = SENDER_CONNECT;
		data->packet_size = sizeof(ctrlpacket);
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "ADD MSG SENDER_CONNECT to RECV MSG STACK %d\n", recv_queue->start);
	}

	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Starting send ctrl loop...\n");
	while (1) {

		pkt_queue = send_queue->queue;
		if (pkt_queue[send_queue->end] != NULL) {
			
			pkt = pkt_queue[send_queue->end];
			if ((pkt->valid) && (pkt->data != NULL)) {
				data = pkt->data;
				if ((send (socket_fd, data, data->packet_size,0)) == -1) {
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Failure Sending Message\n");
					continue;
				}

				pkt->valid = 0;
				send_queue->end = (send_queue->end + 1) % MAX_CTRL_QUEUE;
			}
		}
		usleep(10000);
	} 

	close(socket_fd);
	return NULL;
}



