/**\file sendctrl.c
 * \brief Definição de métodos para controle da envio de testes.
 *  
 * \author Alberto Carvalho
 * \date August 28, 2013
 */

#include <defines.h>
#include <ctrl/sendctrl.h>
#include <modules/continuo_burst.h>
#include <modules/packet_train.h>
#include <tools/debug.h>
#include <tools/tmath.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 
#include <pthread.h>

int sendctrl (settings *conf_settings) {
	int ret = 0;
	pthread_t sendtest_thread_id;

	/* cria thread de envio do probe */
	switch (conf_settings->t_type) {
		case UDP_CONTINUO:
				if ((ret = pthread_create (&sendtest_thread_id, NULL, send_continuo_burst, (void *)conf_settings))) {
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not create recvtest thread: %s\n",
					(ret == EAGAIN) ? "EAGAIN" : (ret == EINVAL) ? "EINVAL" : (ret == EPERM) ? "EPERM" : "UNKNOW");
				}
			break;
		case UDP_PACKET_TRAIN:
				if ((ret = pthread_create (&sendtest_thread_id, NULL, send_packet_train, (void *)conf_settings))) {
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not create recvtest thread: %s\n",
					(ret == EAGAIN) ? "EAGAIN" : (ret == EINVAL) ? "EINVAL" : (ret == EPERM) ? "EPERM" : "UNKNOW");
				}
			break;
		default:
			DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Invalid test type %d\n", conf_settings->t_type);
	}

	if (ret)
		return ret;

	/* join thread */
	pthread_join (sendtest_thread_id, NULL);

	/* log de resultados */
	char log_buffer[256];
	memset (log_buffer, 0, 256);
	switch (conf_settings->t_type) {
		case UDP_CONTINUO:
			if (!get_currentDateTime(log_buffer, 256)) {
				LOG_FILE (DEFAULT_SENDCONT_BURST_LOGFILE, "[%s]\t%4d\t%4d\t%5d\t%10d\t%4d\t%4d\n",
						log_buffer, conf_settings->sendsock_buffer, conf_settings->packet_size, conf_settings->test.cont.pkt_num,
						conf_settings->test.cont.pkt_num*conf_settings->packet_size, conf_settings->udp_rate,
						conf_settings->test.cont.size);
			}
			else {
				LOG_FILE (DEFAULT_SENDCONT_BURST_LOGFILE, "%4d\t%4d\t%5d\t%10d\t%4d\t%4d\n",
						conf_settings->sendsock_buffer, conf_settings->packet_size, conf_settings->test.cont.pkt_num,
						conf_settings->test.cont.pkt_num*conf_settings->packet_size, conf_settings->udp_rate,
						conf_settings->test.cont.size);
			}
			break;
		case UDP_PACKET_TRAIN:
			if (!get_currentDateTime(log_buffer, 256)) {
				LOG_FILE (DEFAULT_SENDTRAIN_BURST_LOGFILE, "[%s]\t%4d\t%4d\t%5d\t%10d\t%4d\t%4d\t%4d\t%4d\n",
						log_buffer, conf_settings->sendsock_buffer, conf_settings->packet_size,
						(conf_settings->test.train.num * conf_settings->test.train.size),
						(conf_settings->test.train.num * conf_settings->test.train.size * conf_settings->packet_size),
						conf_settings->udp_rate, conf_settings->test.train.num,
						conf_settings->test.train.size, conf_settings->test.train.interval);
			}
			else {
				LOG_FILE (DEFAULT_SENDTRAIN_BURST_LOGFILE, "%4d\t%4d\t%5d\t%10d\t%4d\t%4d\t%4d\t%4d\n",
						conf_settings->sendsock_buffer, conf_settings->packet_size,
						(conf_settings->test.train.num * conf_settings->test.train.size),
						(conf_settings->test.train.num * conf_settings->test.train.size * conf_settings->packet_size),
						conf_settings->udp_rate, conf_settings->test.train.num,
						conf_settings->test.train.size, conf_settings->test.train.interval);
			}
			break;
		default:
			DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Invalid test type %d\n", conf_settings->t_type);
	}

	return ret;
}

