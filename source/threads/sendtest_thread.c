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


void *sendtest_thread (void *param) {
	settings *conf_settings = (settings *)param;

	/* teste fake (por enquanto)... */
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "SEND TEST THREAD FAKE... SLEEP by 5s\n");
	//usleep (5000000);
	if (conf_settings->t_type == UDP_CONTINUO) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "CALL send_continuo_burst\n");
		send_continuo_burst (conf_settings->udp_port,
			conf_settings->sendsock_buffer,
			conf_settings->udp_rate, conf_settings->test.cont.size, conf_settings->packet_size, NULL);
	}
	else
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Could'nt start send test: test type (%d) != continuo (%d)\n",
							conf_settings->t_type, UDP_CONTINUO);

	return NULL;
}
