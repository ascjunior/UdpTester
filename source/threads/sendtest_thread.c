/**\file sendtest_thread.c
 * \brief Definição de métodos para controle do envio de testes udp.
 *  
 * \author Alberto Carvalho
 * \date August 02, 2013.
 */


#include <defines.h>
#include <tools/debug.h>
#include <threads/sendtest_thread.h>
#include <modules/continuo_burst.h>


#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


void *sendtest_thread (void *param) {
	settings *conf_settings = (settings *)param;

	if (conf_settings->t_type == UDP_CONTINUO) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "CALL send_continuo_burst to %s\n", inet_ntoa(conf_settings->address.sin_addr));
		send_continuo_burst (conf_settings->udp_port,
			conf_settings->sendsock_buffer, conf_settings->udp_rate,
			conf_settings->test.cont.size, conf_settings->packet_size,
			conf_settings->address.sin_addr.s_addr, NULL);
		//usleep (5000000);
	}
	else
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Could'nt start send test: test type (%d) != continuo (%d)\n",
							conf_settings->t_type, UDP_CONTINUO);
	return NULL;
}
