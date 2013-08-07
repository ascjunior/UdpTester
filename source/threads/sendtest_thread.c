/**\file sendtest_thread.c
 * \brief Definição de métodos para controle do envio de testes udp.
 *  
 * \author Alberto Carvalho
 * \date August 02, 2013.
 */


#include <defines.h>
#include <tools/debug.h>
#include <threads/sendtest_thread.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


void *sendtest_thread (void *param) {
	//settings *conf_settings = (settings *)param;

	/* teste fake (por enquanto)... */
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "SEND TEST THREAD FAKE... SLEEP by 5s\n");
	usleep (5000000);

	return NULL;
}
