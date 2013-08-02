/**\file recvctrl_thread.c
 * \brief Definição de métodos para gerenciamento de mensagens  de  controle  do
 * UdpTester (módulo para receber mensagens).
 *  
 * \author Alberto Carvalho
 * \date July 05, 2013.
 */

#include <defines.h>
#include <tools/debug.h>
#include <threads/recvctrl_thread.h>

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


void *recvctrl_thread (void *param) {
	settings *conf_settings = (settings *)param;
	ctrl_queue *recv_queue = &(conf_settings->recv_queue);
	ctrl_pkt **pkt_queue = recv_queue->queue;
	ctrlpacket *data;
	struct sockaddr_in server;
	struct sockaddr_in dest;
	int socket_fd, client_fd,num;
	socklen_t size;
	char buffer[BUFFER_SIZE+1];
	int yes = 1;

	DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Starting recv ctrl thread...\n");
    if ((socket_fd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Could not create socket\n");
        exit(1);
    }

	if (setsockopt (socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Could not set socket\n");
		exit(1);
	}

	memset(&server, 0, sizeof(server));
	memset(&dest,0, sizeof(dest));
	server.sin_family = AF_INET;
	if (conf_settings->mode == RECEIVER)
		server.sin_port = htons(RECV_PORT);
	else
		server.sin_port = htons(SEND_PORT);
	server.sin_addr.s_addr = INADDR_ANY;
	
	if ((bind (socket_fd, (struct sockaddr *)&server, sizeof(struct sockaddr ))) == -1) { 
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Could not bind\n");
		exit(1);
	}

	if ((listen (socket_fd, BACKLOG)) == -1) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Could not listen\n");
		exit(1);
	}

	while (1) {
		size = sizeof(struct sockaddr_in);  

		if ((client_fd = accept(socket_fd, (struct sockaddr *)&dest, &size))==-1) {
			DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Could not accept\n");
			exit(1);
		}

		//adicionar a lista de mensagens recebidas a nova conexão: receiver_connect
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Connection accepted (%d)\n", recv_queue->start);

		if ((pkt_queue[recv_queue->start] != NULL))
			recv_queue->start = (recv_queue->start + 1) % MAX_CTRL_QUEUE;

		if (pkt_queue[recv_queue->start] == NULL) {
			pkt_queue[recv_queue->start] = (ctrl_pkt *)malloc(sizeof(ctrl_pkt));
			pkt_queue[recv_queue->start]->data = (ctrlpacket *)malloc(sizeof(ctrlpacket));
		}
		pkt_queue[recv_queue->start]->valid = 1;
		data = pkt_queue[recv_queue->start]->data;
		data->cp_type = RECEIVER_CONNECT;
		data->packet_size = sizeof(ctrlpacket);
		data->connected_address = dest.sin_addr.s_addr;
		
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Start recv ctrl loop thread (%d)\n", recv_queue->start);
		while (1) {
			if ((num = recv (client_fd, buffer, BUFFER_SIZE,0)) == -1) {
				DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Error in receiving message\n");
				exit(1);
			}   
			else if (num == 0) {
				DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Connection closed\n");
				break;
			}

			buffer[num] = '\0';
			DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Message received: %s\n", buffer);

			/* adcionar msg de log a fila de mensagens recebidas */
			if ((pkt_queue[recv_queue->start] != NULL))
				recv_queue->start = (recv_queue->start + 1) % MAX_CTRL_QUEUE;

			if (pkt_queue[recv_queue->start] == NULL) {
				pkt_queue[recv_queue->start] = (ctrl_pkt *)malloc(sizeof(ctrl_pkt));
				pkt_queue[recv_queue->start]->data = (ctrlpacket *)malloc(sizeof(ctrlpacket));
			}
			pkt_queue[recv_queue->start]->valid = 1;
			data = pkt_queue[recv_queue->start]->data;
			memcpy(data, buffer, num);
			data->packet_size = num;
		}
		close(client_fd);   
    }
    close(socket_fd);

    return NULL;
}
