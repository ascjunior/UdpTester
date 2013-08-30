/**\file ctrl.c
 * \brief Definição de métodos de controle do UdpTester.
 *  
 * \author Alberto Carvalho
 * \date June 26, 2013
 */

#include <defines.h>
#include <ctrl/ctrl.h>
#include <ctrl/recvctrl.h>
#include <ctrl/sendctrl.h>
#include <tools/debug.h>
#include <threads/recvctrl_thread.h>
#include <threads/sendctrl_thread.h>

#include <sys/time.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 
#include <pthread.h>

ctrlpacket *getnext_ctrlpacket (ctrl_queue *queue) {
	ctrl_pkt *pkt = NULL;
	ctrlpacket *data = NULL;
	ctrl_pkt **pkt_queue = queue->queue;

	pthread_mutex_lock (&(queue->mutex));
	if (pkt_queue[queue->end]->valid) {
		pkt = pkt_queue[queue->end];
		data = &(pkt->data);
		pkt->valid = 0;

		/* mensagens para depuração. */
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "MSG TYPE %s\n",
					(data->cp_type == RECEIVER_CONNECT) ? "RECEIVER_CONNECT" :
					(data->cp_type == SENDER_CONNECT) ? "SENDER_CONNECT" :
					(data->cp_type == START_TEST_DOWN) ? "START_TEST_DOWN" :
					(data->cp_type == FEEDBACK_START_TEST_DOWN) ? "FEEDBACK_START_TEST_DOWN" :
					(data->cp_type == FEEDBACK_TEST_DOWN) ? "FEEDBACK_TEST_DOWN" :
					(data->cp_type == START_TEST_UP) ? "START_TEST_UP" :
					(data->cp_type == FEEDBACK_START_TEST_UP) ? "FEEDBACK_START_TEST_UP" :
					(data->cp_type == FEEDBACK_TEST_UP) ? "FEEDBACK_TEST_UP" :
					(data->cp_type == LOOP_TEST_INTERVAL) ? "LOOP_TEST_INTERVAL" : "UNKNOW");

		pkt->valid = 0;
		queue->end = (queue->end + 1) % MAX_CTRL_QUEUE;
	}
	pthread_mutex_unlock (&(queue->mutex));

	return data;
}

int get_next_test (settings *conf_settings) {
	int ret = 0, count = MAX_CONFIG_TEST, index = 0;
	list_test *ag_test = &(conf_settings->ag_test);
	config_test *cfg_test = ag_test->cfg_test;

	if (ag_test->size > 0) {
		index = ag_test->next;

		while ((cfg_test[index].t_type == UDP_INVALID) && (count > 0)) {
			index = (index + 1) % MAX_CONFIG_TEST;
			count--;
		}
		if (count < 0) {
			ret = -1;
		}
		else {
			ag_test->next = index;
			conf_settings->t_type = cfg_test[index].t_type;
			conf_settings->udp_rate = cfg_test[index].udp_rate;
			switch (cfg_test[index].t_type) {
				case UDP_CONTINUO:
					conf_settings->test.cont = cfg_test[index].test.cont;
					ag_test->next++;
					break;
				case UDP_PACKET_TRAIN:
					conf_settings->test.train = cfg_test[index].test.train;
					ag_test->next++;
					break;
				default:
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Invalid test type %d\n", cfg_test[index].t_type);
			}
		}
	}

	return ret;
}

int save_test_report (settings *conf_settings, ctrlpacket *data) {
	int ret = 0;
	report *pkt_report = (report *)data->buffer;
	resume *result = &(pkt_report->result);
	
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "SAVE REPORT TEST...\n");
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "MODE: %s\n",
		(conf_settings->mode == SENDER) ? "SENDER" : (conf_settings->mode == RECEIVER) ? "RECEIVER" : "UNKNOW");
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "TEST TYPE             : %s\n",
		(pkt_report->t_type == UDP_CONTINUO) ? "CONTINUO" : (pkt_report->t_type == UDP_PACKET_TRAIN) ? "TRAIN" : "UNKNOW");
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "PACKET SIZE           : %d\n", result->packet_size);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "PACKET NUM            : %d\n", result->packet_num);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "TOTAL BYTES           : %d\n", result->bytes);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "UDP PORT              : %d\n", pkt_report->udp_port);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "BW MED                : %d.%06d\n",
						result->bw_med.tv_sec, result->bw_med.tv_usec);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "JITTER (med, min, max): %dus\t%dus\t%dus\n",
						result->jitter_med, result->jitter_min, result->jitter_max);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "LOSS                  : %d\n", result->loss_med);
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "TIME                  : %d.%06d\n",
						result->time_med.tv_sec, result->time_med.tv_usec);
						
	return ret;
}

int start_sendctrl_connection (settings *conf_settings) {
	int ret = 0;
	
	/* TODO: Verificar local melhor para id de threads, com configurações??? */
	pthread_t sendctrl_thread_id;
	pthread_attr_t attr;

	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

	/* inicializa thread para enviar mensagens de controle */
	if (pthread_create (&sendctrl_thread_id, &attr, sendctrl_thread, (void *)conf_settings)) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not sendctrl thread\n");
		ret = -1;
	}

	return ret;
}

int start_ctrl_connection (settings *conf_settings) {
	int ret = 0;
	pthread_t recvctrl_thread_id;
	pthread_attr_t attr;

	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

	/* inicializa thread para receber mensagens de controle */
	if (pthread_create (&recvctrl_thread_id, &attr, recvctrl_thread, (void *)conf_settings)) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Could not create recvctrl thread\n");
		ret = -1;
	}

	if (conf_settings->mode == RECEIVER) {
		/* inicializa thread para enviar mensagens de controle -- destino conhecido o server! */
		if ((ret = start_sendctrl_connection (conf_settings)))
			DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not create sendctrl thread\n");
	}

	return ret;
}

void loop_control (settings *conf_settings) {
	ctrlpacket *data;
	ctrl_queue *recvctrl_queue = &(conf_settings->recv_queue);
	ctrl_queue *sendctrl_queue = &(conf_settings->send_queue);

	data = getnext_ctrlpacket (recvctrl_queue);
	if (data != NULL) {
		switch (data->cp_type) {
			case RECEIVER_CONNECT:
				if (conf_settings->mode == SENDER) {
					/* send crtl msg connection */
					conf_settings->address.sin_addr.s_addr = data->connected_address.sin_addr.s_addr;
					start_sendctrl_connection (conf_settings);
				}
				break;
			case SENDER_CONNECT:
				if (conf_settings->mode == SENDER) {
					get_next_test (conf_settings);

					pthread_mutex_lock (&(sendctrl_queue->mutex));

					ctrl_pkt **pkt_queue = sendctrl_queue->queue;
					sendctrl_queue->start = (sendctrl_queue->start + 1) % MAX_CTRL_QUEUE;
					pkt_queue[sendctrl_queue->start]->valid = 1;
					data = &(pkt_queue[sendctrl_queue->start]->data);
					data->cp_type = START_TEST_DOWN;
					data->t_type = conf_settings->t_type;
					data->packet_size = conf_settings->packet_size;
					data->udp_rate = conf_settings->udp_rate;
					data->udp_port = conf_settings->udp_port;
					switch (conf_settings->t_type) {
						case UDP_CONTINUO:
							if (conf_settings->test.cont.size > 0) {
								conf_settings->test.cont.pkt_num = (conf_settings->test.cont.size * 1000000)/conf_settings->packet_size;
								if (((conf_settings->test.cont.size * 1000000) % conf_settings->packet_size) > 0) {
									conf_settings->test.cont.pkt_num++;
								}
							}
							data->test.cont = conf_settings->test.cont;
							break;
						case UDP_PACKET_TRAIN:
							data->test.train = conf_settings->test.train;
							break;
						default:
							DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Invalid test type %d\n", conf_settings->t_type);
					}

					pthread_mutex_unlock (&(sendctrl_queue->mutex));
				}
				break;
			case START_TEST_DOWN:
				if (conf_settings->mode == RECEIVER) {
					conf_settings->udp_rate = data->udp_rate;
					conf_settings->t_type = data->t_type;
					switch (data->t_type) {
						case UDP_CONTINUO:
							conf_settings->test.cont =  data->test.cont;
							break;
						case UDP_PACKET_TRAIN:
							conf_settings->test.train = data->test.train;
							break;
						default:
							DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Invalid test type %d\n", conf_settings->t_type);
					}
					recvctrl (conf_settings);

				}
				break;
			case FEEDBACK_START_TEST_DOWN:
				if (conf_settings->mode == SENDER) {
					sendctrl (conf_settings);
				}
				break;
			case FEEDBACK_TEST_DOWN:
				if (conf_settings->mode == SENDER) {
					save_test_report (conf_settings, data);
				}
				break;
			case START_TEST_UP:
				if (conf_settings->mode == SENDER) {
					recvctrl (conf_settings);
				}
				break;
			case FEEDBACK_START_TEST_UP:
				if (conf_settings->mode == RECEIVER) {
					sendctrl (conf_settings);
				}
				break;
			case FEEDBACK_TEST_UP:
				if (conf_settings->mode == RECEIVER) {
					save_test_report (conf_settings, data);
				}
				break;
			case LOOP_TEST_INTERVAL:
				if (conf_settings->mode == SENDER) {
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "Sleep ctrl by %dms\n", 5000);
					usleep (5000000);

					pthread_mutex_lock (&(recvctrl_queue->mutex));

					ctrl_pkt **pkt_queue = recvctrl_queue->queue;
					recvctrl_queue->start = (recvctrl_queue->start + 1) % MAX_CTRL_QUEUE;
					pkt_queue[recvctrl_queue->start]->valid = 1;
					data = &(pkt_queue[recvctrl_queue->start]->data);
					data->cp_type = SENDER_CONNECT;
					data->packet_size = sizeof(ctrlpacket);

					pthread_mutex_unlock (&(recvctrl_queue->mutex));
				}
				break;
			default:
				DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "WARNING: DEFAULT CASE ACTION!!!!\n");
				return;
		}
	}
}
