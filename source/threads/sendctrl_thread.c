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
    int sockfd;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0))== -1) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Could create socket\n");
		exit (1);
	}

	memset (&server_info, 0, sizeof(server_info));
	server_info.sin_family = AF_INET;
	if (conf_settings->mode == RECEIVER) {
		server_info.sin_port = htons(SEND_PORT);
	}
	else {
		server_info.sin_port = htons(RECV_PORT);
	}
	server_info.sin_addr = conf_settings->address.sin_addr;

	if (connect (sockfd, (struct sockaddr *)&server_info, sizeof(struct sockaddr)) < 0) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Connection Failure\n");
		exit (1);
	}
	else {
		/* sinalizar na fila de mensagens recebidas que a conexão foi estabelecida */
		pthread_mutex_lock (&(recv_queue->mutex));

		pkt_queue = recv_queue->queue;
		recv_queue->start = (recv_queue->start + 1) % MAX_CTRL_QUEUE;
		pkt_queue[recv_queue->start]->valid = 1;
		data = &(pkt_queue[recv_queue->start]->data);
		data->cp_type = SENDER_CONNECT;
		data->packet_size = sizeof(ctrlpacket);

		pthread_mutex_unlock (&(recv_queue->mutex));
	}

	while (1) {

		pthread_mutex_lock (&(send_queue->mutex));

		pkt_queue = send_queue->queue;
		if (pkt_queue[send_queue->end]->valid) {
			
			pkt = pkt_queue[send_queue->end];
			data = &(pkt->data);
			if ((send (sockfd, data, data->packet_size,0)) == -1) {
				DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Failure Sending Message\n");
				continue;
			}

			pkt->valid = 0;
			send_queue->end = (send_queue->end + 1) % MAX_CTRL_QUEUE;
		}

		pthread_mutex_unlock (&(send_queue->mutex));
		usleep(10000);
	}

	close(sockfd);
	return NULL;
}


void *sendctrlUDP_thread (void *param) {
	settings *conf_settings = (settings *)param;
	ctrl_queue *send_queue = &(conf_settings->send_queue);
	ctrl_queue *recv_queue = &(conf_settings->recv_queue);
	ctrl_pkt **pkt_queue = send_queue->queue;
	ctrl_pkt *pkt;
	ctrlpacket *data;
    struct sockaddr_in server_info;
	struct sockaddr *sock_server_info = (struct sockaddr *)&server_info;
    int sockfd, resize_buffer = DEFAULT_SOC_BUFFER, slen = sizeof (server_info);

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))== -1) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could create socket\n");
		exit (1);
	}

	memset (&server_info, 0, sizeof(server_info));
	server_info.sin_family = AF_INET;
	server_info.sin_addr = conf_settings->address.sin_addr;
	if (conf_settings->mode == RECEIVER)
		server_info.sin_port = htons(SEND_PORT);
	else
		server_info.sin_port = htons(RECV_PORT);

	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const void *)&resize_buffer, sizeof(resize_buffer)) != 0) {
		DEBUG_MSG (DEBUG_LEVEL_LOW, "Could not resize send socket buffers!!\n");
		exit(1);
	}

	if ((bind (sockfd, (const struct sockaddr *)&server_info, sizeof(server_info))) == -1) {
		DEBUG_MSG (DEBUG_LEVEL_LOW, "Could not bind!!\n");
		exit(1);
	}
	else {
		/* sinalizar na fila de mensagens recebidas que a conexão foi estabelecida */
		pkt_queue = recv_queue->queue;

		pthread_mutex_lock (&(recv_queue->mutex));

		recv_queue->start = (recv_queue->start + 1) % MAX_CTRL_QUEUE;
		pkt_queue[recv_queue->start]->valid = 1;
		data = &(pkt_queue[recv_queue->start]->data);
		data->cp_type = SENDER_UDPCONNECT;
		data->packet_size = sizeof(ctrlpacket);

		pthread_mutex_unlock (&(recv_queue->mutex));
	}

	while (1) {
		pthread_mutex_lock (&(send_queue->mutex));

		pkt_queue = send_queue->queue;
		if (pkt_queue[send_queue->end]->valid) {
			
			pkt = pkt_queue[send_queue->end];
			data = &(pkt->data);
			if ((sendto (sockfd, data, data->packet_size, 0, sock_server_info, slen)) == -1) {
				DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Failure Sending Message\n");
				continue;
			}

			pkt->valid = 0;
			send_queue->end = (send_queue->end + 1) % MAX_CTRL_QUEUE;
		}

		pthread_mutex_unlock (&(send_queue->mutex));
		usleep(10000);
	} 

	close(sockfd);
	return NULL;
}


