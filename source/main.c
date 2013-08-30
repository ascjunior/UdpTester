/**\file main.c
 * \brief Inicialização do UdpTester.
 * 
 * \author Alberto Carvalho
 * \date June 25, 2013
 */
 
#include <defines.h>
#include <tools/parser.h>
#include <tools/debug.h>
#include <ctrl/ctrl.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <sys/time.h>
#include <arpa/inet.h>
#include <pthread.h>

/**\fn int  main( int argc, char **argv )
 * \brief Inicialização da Buscard.
 * 
 * \param argc Número de argumentos da linha de comando.
 * \param argv Lista de argumentos da linha de comando.
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int main (int argc, char **argv) {

	int ret = 0, i = 0;
	settings conf_settings;
	ctrl_pkt *recv_queue[MAX_CTRL_QUEUE];
	ctrl_pkt *send_queue[MAX_CTRL_QUEUE];
	
	for (i= 0; i < MAX_CTRL_QUEUE; i++) {
		recv_queue[i] = (ctrl_pkt *)malloc(sizeof(ctrl_pkt));
		recv_queue[i]->valid = 0;
		send_queue[i] = (ctrl_pkt *)malloc(sizeof(ctrl_pkt));
		send_queue[i]->valid = 0;
	}

	ret = parse_command_line (argc, argv, &conf_settings);
	if (ret) {
		usage ();
		exit (0);
	}


	/* inicializa fila de pacotes de controle */
	/*
	pktctrl_queue_init (&send_queue);
	pktctrl_queue_init (&recv_queue);
	*/
	conf_settings.send_queue.queue = send_queue;
	conf_settings.send_queue.size = 0;
	conf_settings.send_queue.start = -1;
	conf_settings.send_queue.end = 0;
	pthread_mutex_init (&(conf_settings.send_queue.mutex), NULL);
	conf_settings.recv_queue.queue = recv_queue;
	conf_settings.recv_queue.size = 0;
	conf_settings.recv_queue.start = -1;
	conf_settings.recv_queue.end = 0;
	pthread_mutex_init (&(conf_settings.recv_queue.mutex), NULL);
	
/*
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "settings:\n");
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "settings->mode: %s\n",
			(conf_settings.mode == SENDER) ? "SENDER" : (conf_settings.mode == RECEIVER) ? "RECEIVER" : "UNKNOW");
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "settings->bandwidth: %dMbps\n", conf_settings.udp_rate);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "settings->packet_size: %dbytes\n", conf_settings.packet_size);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "settings_>interval: %ds\n", conf_settings.loop_interval);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "settings->address: %s\n", inet_ntoa(conf_settings.address.sin_addr));
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "CALL start_ctrl_connection \n");
*/

	start_ctrl_connection (&conf_settings);

	while (1) {
		loop_control (&conf_settings);
		usleep(10000);
	}
	
	return 0;
}
