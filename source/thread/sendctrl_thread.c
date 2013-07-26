/**\file sendctrl_thread.c
 * \brief Definição de métodos para gerenciamento de mensagens  de  controle  do
 * UdpTester (módulo para enviar mensagens).
 *  
 * \author Alberto Carvalho
 * \date July 05, 2013
 */

#include <defines.h>
#include <thread/sendctrl_thread.h>


start_ctrl_connection(settings *conf_settings, sockets *sock) {

	inicializa dados de socket
	cria thread de controle -- enviar/receber dados
}

void *sendctrl_thread (void *param) {
	settings *conf_settings = (settings *)param;
	ctrlpacket **queue = conf_settings->recv_queue;
	ctrlpacket *pkt;
    struct sockaddr_in server_info;
    struct hostent *he;
    int socket_fd,num;
    char buffer[MAXBUFFER_SIZE];

	if (argc != 2) {
		fprintf(stderr, "Usage: client hostname\n");
		exit(1);
	}

	if ((he = gethostbyname(conf_settings->address))==NULL) {
		//LOG(LOG_ERROR, "Cannot get host name\n");
		exit(1);
	}

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0))== -1) {
		//LOG(LOG_ERROR, "Socket Failure!!\n");
		exit (1);
	}

	memset (&server_info, 0, sizeof(server_info));
	server_info.sin_family = AF_INET;
	server_info.sin_port = htons(PORT);
	server_info.sin_addr = *((struct in_addr *)he->h_addr);
	if (connect (socket_fd, (struct sockaddr *)&server_info, sizeof(struct sockaddr)) < 0) {
		//LOG(LOG_ERROR, "Connection Failure\n");
		exit (1);
	}
	else {
		//sinalizar na fila de mensagens recebidas que a conexão foi estabelecida
		//LOG(LOG_INFO, "Connection Established\n");
		conf_settings->send_queue_id = (conf_settings->send_queue_id + 1) % MAX_CTRL_QUEUE;
		if (queue[conf_settings->send_queue_id] == NULL) {
			queue[conf_settings->send_queue_id] = (ctrl_pkt *)malloc(sizeof(ctrl_pkt));
			queue[conf_settings->send_queue_id]->pkt = (ctrlpacket *)malloc(sizeof(ctrlpacket));
		}
		queue[conf_settings->send_queue_id]->valid = 1;
		pkt = queue[conf_settings->send_queue_id]->pkt;
		pkt->cp_type =SENDER_CONNECT;
		pkt->packet_size = sizeof(ctrlpacket);
		pkt->connected_address = dest.sin_addr;
	}

	while (1) {

		if ((queue[conf_settings->send_queue_id] != NULL) &&
			(queue[conf_settings->send_queue_id]->valid == 1)) {

			pkt = queue[conf_settings->send_queue_id]->pkt;
			if ((send (socket_fd, pkt, pkt->packet_size,0)) == -1) {
				//LOG(LOG_ERROR, "Failure Sending Message\n");
				continue;
			}

			queue[conf_settings->send_queue_id]->valid = 0;
			conf_settings->send_queue_id--;
			if (conf_settings->send_queue_id < 0)
				conf_settings->send_queue_id = MAX_CTRL_QUEUE;
		}

	} 

	close(socket_fd);
	return NULL;
}



