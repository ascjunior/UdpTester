/**\file main.c
 * \brief Inicialização do UdpTester.
 * 
 * \author Alberto Carvalho
 * \date June 25, 2013
 */
 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <defines.h>
#include <tools/parser.h>

/**\fn int  main( int argc, char **argv )
 * \brief Inicialização da Buscard.
 * 
 * \param argc Número de argumentos da linha de comando.
 * \param argv Lista de argumentos da linha de comando.
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int main (int argc, char **argv) {

	int ret = 0;
	settings conf_settings;
	ctrl_pkt recv_queue[MAX_CTRL_QUEUE];
	ctrl_pkt send_queue[MAX_CTRL_QUEUE];
	
	ret = parse_command_line (argc, argv, &conf_settings);
	if (ret < 0) {
		usage ();
	}
	else if (ret > 0)
		exit (0);


	/* inicializa fila de pacotes de controle */
	pktctrl_queue_init (&send_queue);
	pktctrl_queue_init (&recv_queue);
	conf_settings->send_queue = &send_queue;
	conf_settings->recv_queue = &recv_queue;

	start_ctrl_connection (&conf_settings);

	while (1) {
		loop_control (&conf_settings);
		usleep(1000);
	}
	
	return 0;
}
