/**\file recvctrl_thread.c
 * \brief Definição de métodos para gerenciamento de mensagens  de  controle  do
 * UdpTester (módulo para receber mensagens).
 *  
 * \author Alberto Carvalho
 * \date July 05, 2013
 */



start_ctrl_connection(settings *conf_settings, sockets *sock) {

	inicializa dados de socket
	cria thread de controle -- enviar/receber dados
}

void *recvctrl_thread (void *param) {
	settings *conf_settings = (settings *)param;
	ctrlpacket **queue = conf_settings->recv_queue;
	ctrlpacket *pkt;
	struct sockaddr_in server;
	struct sockaddr_in dest;
	int socket_fd, client_fd,num;
	socklen_t size;
	char buffer[MAXBUFFER_SIZE+1];
	int yes = 1;

    if ((socket_fd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        //LOG(LOG_ERROR, "Socket failure!!\n");
        exit(1);
    }

	if (setsockopt (socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		//LOG(LOG_ERROR, "setsockopt fail\n");
		exit(1);
	}

	memset(&server, 0, sizeof(server));
	memset(&dest,0, sizeof(dest));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;
	
	if ((bind (socket_fd, (struct sockaddr *)&server, sizeof(struct sockaddr ))) == -1) { //sizeof(struct sockaddr) 
		//LOG(LOG_ERROR, "Binding Failure\n");
		exit(1);
	}

	if ((listen (socket_fd, BACKLOG)) == -1) {
		//LOG(LOG_ERROR, "Listening Failure\n");
		exit(1);
	}

	while (1) {
		size = sizeof(struct sockaddr_in);  

		if ((client_fd = accept(socket_fd, (struct sockaddr *)&dest, &size))==-1) {
			//LOG(LOG_ERROR,"Accept Failure\n");
			exit(1);
		}

		//adicionar a lista de mensagens recebidas a nova conexão: receiver_connect
		if ((queue[conf_settings->recv_queue_id] != NULL) &&
			(queue[conf_settings->recv_queue_id]->valid == 1))
			conf_settings->recv_queue_id = (conf_settings->recv_queue_id + 1) % MAX_CTRL_QUEUE;

		if (queue[conf_settings->recv_queue_id] == NULL) {
			queue[conf_settings->recv_queue_id] = (ctrl_pkt *)malloc(sizeof(ctrl_pkt));
			queue[conf_settings->recv_queue_id]->pkt = (ctrlpacket *)malloc(sizeof(ctrlpacket));
		}
		queue[conf_settings->recv_queue_id]->valid = 1;
		pkt = queue[conf_settings->recv_queue_id]->pkt;
		pkt->cp_type = RECEIVER_CONNECT;
		pkt->packet_size = sizeof(ctrlpacket);
		pkt->connected_address = dest.sin_addr;
		
		while (1) {
			if ((num = recv (client_fd, buffer, MAXBUFFER_SIZE,0)) == -1) {
				//LOG(LOG_ERROR, "Error in receiving message!!\n");
				exit(1);
			}   
			else if (num == 0) {
				//LOG(LOG_INFO, "Connection closed\n");
				break;
			}

			buffer[num] = '\0';
			//LOG(LOG_INFO, "Message received: %s(sendig feedback..)\n", buffer);
			//adcionar msg de log a fila de mensagens recebidas...
			conf_settings->recv_queue_id = (conf_settings->recv_queue_id + 1) % MAX_CTRL_QUEUE;
			if (queue[conf_settings->recv_queue_id] == NULL) {
				queue[conf_settings->recv_queue_id] = (ctrl_pkt *)malloc(sizeof(ctrl_pkt));
				queue[conf_settings->recv_queue_id]->pkt = (ctrlpacket *)malloc(sizeof(ctrlpacket));
			}
			queue[conf_settings->recv_queue_id]->valid = 1;
			pkt = queue[conf_settings->recv_queue_id]->pkt;
			memcpy(pkt, buffer, num);
			pkt->packet_size = num;
		}
		close(client_fd);   
    }
    close(socket_fd);

    return NULL;
}
