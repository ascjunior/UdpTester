/**\file ctrl.c
 * \brief Definição de métodos de controle do UdpTester.
 *  
 * \author Alberto Carvalho
 * \date June 26, 2013
 */

#include <defines.h>
#include <ctrl/ctrl.h>
#include <tools/debug.h>
#include <threads/recvctrl_thread.h>
#include <threads/sendctrl_thread.h>
#include <threads/recvtest_thread.h>
#include <threads/sendtest_thread.h>

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

int save_test_report (settings *conf_settings, ctrlpacket *data) {
	int ret = 0;
	report *pkt_report = (report *)data->buffer;
	resume *result = &(pkt_report->result);
	
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "SAVE REPORT TEST...\n");
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "MODE: %s\n",
		(conf_settings->mode == SENDER) ? "SENDER" : (conf_settings->mode == RECEIVER) ? "RECEIVER" : "UNKNOW");
	DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "TEST TYPE             : %s\n",
		(data->t_type == UDP_CONTINUO) ? "CONTINUO" : (data->t_type == UDP_PACKET_TRAIN) ? "TRAIN" : "UNKNOW");
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

	/* inicializa thread para enviar mensagens de controle */
	if (pthread_create (&sendctrl_thread_id, NULL, sendctrl_thread, (void *)conf_settings)) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not sendctrl thread\n");
		ret = -1;
	}

	return ret;
}


int start_sendtest_thread (settings *conf_settings) {
	int ret = 0;
	/* TODO: Verificar local melhor para id de threads, com configurações??? */
	pthread_t sendtest_thread_id;

	/* inicializa thread para enviar teste udp */
	if ((ret = pthread_create (&sendtest_thread_id, NULL, sendtest_thread, (void *)conf_settings))) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not sendtest thread\n");
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not create sendtest thread: %s\n",
						(ret == EAGAIN) ? "EAGAIN" : (ret == EINVAL) ? "EINVAL" : (ret == EPERM) ? "EPERM" : "UNKNOW");
	}

	return ret;
}

int start_recvtest_thread (settings *conf_settings) {
	int ret = 0;
	/* TODO: Verificar local melhor para id de threads, com configurações??? */
	pthread_t recvtest_thread_id;

	/* inicializa thread para receber teste udp */
	if ((ret = pthread_create (&recvtest_thread_id, NULL, recvtest_thread, (void *)conf_settings))) {
		DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Could not create recvtest thread: %s\n",
						(ret == EAGAIN) ? "EAGAIN" : (ret == EINVAL) ? "EINVAL" : (ret == EPERM) ? "EPERM" : "UNKNOW");
	}

	return ret;
}

int start_ctrl_connection (settings *conf_settings) {
	int ret = 0;
	/* TODO: Verificar local melhor para id de threads, com configurações??? */
	pthread_t recvctrl_thread_id;

	/* inicializa thread para receber mensagens de controle */
	if (pthread_create (&recvctrl_thread_id, NULL, recvctrl_thread, (void *)conf_settings)) {
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
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Get receiver connection, call start_sendctrl_connection %s\n", inet_ntoa(data->connected_address.sin_addr));
					conf_settings->address.sin_addr.s_addr = data->connected_address.sin_addr.s_addr;
					start_sendctrl_connection (conf_settings);
				}
				break;
			case SENDER_CONNECT:
				if (conf_settings->mode == SENDER) {

					pthread_mutex_lock (&(sendctrl_queue->mutex));

					ctrl_pkt **pkt_queue = sendctrl_queue->queue;
					sendctrl_queue->start = (sendctrl_queue->start + 1) % MAX_CTRL_QUEUE;
					pkt_queue[sendctrl_queue->start]->valid = 1;
					data = &(pkt_queue[sendctrl_queue->start]->data);
					data->cp_type = START_TEST_DOWN;
					data->packet_size = conf_settings->packet_size;
					data->udp_rate = conf_settings->udp_rate;
					data->udp_port = conf_settings->udp_port;
					if (conf_settings->t_type == UDP_CONTINUO) { /* TODO: Outros testes... */
						data->test.cont = conf_settings->test.cont;
					}

					pthread_mutex_unlock (&(sendctrl_queue->mutex));
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "ADD MSG START_TEST_DOWN to SEND STACK!!\n");
				}
				break;
			case START_TEST_DOWN:
				if (conf_settings->mode == RECEIVER) {
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "RECV TEST DOWN THREAD CREATED!!\n");
					if (!(conf_settings->udp_rate))
						conf_settings->udp_rate = data->udp_rate;
					conf_settings->packet_size = data->packet_size;
					conf_settings->test.cont.size = data->test.cont.size;
					conf_settings->test.cont.pkt_num = data->test.cont.pkt_num;
					conf_settings->test.cont.interval = data->test.cont.interval;
					conf_settings->test.cont.report_interval = data->test.cont.report_interval;
					start_recvtest_thread (conf_settings);

/*
receiver:
	mensagem enviada pelo server sinalizando inicio de teste de down
		-- cria thread para receber teste (tipo de teste na mensagem???)
			-- fim de teste:
			-- cria relatório de teste
			-- envia relatório de teste com mensagem do tipo feedback_test_down
			-- envia mensagem de inicio de teste de up com tipo start_test_up
		-- envia mensagem de feedback_start_test_down
*/
					pthread_mutex_lock (&(sendctrl_queue->mutex));

					ctrl_pkt **pkt_queue = sendctrl_queue->queue;
					sendctrl_queue->start = (sendctrl_queue->start + 1) % MAX_CTRL_QUEUE;
					pkt_queue[sendctrl_queue->start]->valid = 1;
					data = &(pkt_queue[sendctrl_queue->start]->data);
					data->cp_type = FEEDBACK_START_TEST_DOWN;
					data->packet_size = sizeof(ctrlpacket);

					pthread_mutex_unlock (&(sendctrl_queue->mutex));
				}
				break;
			case FEEDBACK_START_TEST_DOWN:
				if (conf_settings->mode == SENDER) {
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "SEND TEST DOWN THREAD CREATED!!!\n");
					start_sendtest_thread (conf_settings);

/*
sender:
	cria thread para envio de teste de down
receiver:
	nada a fazer :(
*/
				}
				break;
			case FEEDBACK_TEST_DOWN:
				if (conf_settings->mode == SENDER) {
					printf ("SAVED REPORT TEST DOWN!!!\n");
					save_test_report (conf_settings, data);
/*
sender:
	armazena relatório de teste down (feedback do cliente, alterar probe em função dos resultados???)
receiver:
	nada a fazer :(
*/
				}
				break;
			case START_TEST_UP:
				if (conf_settings->mode == SENDER) {
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "RECV TEST UP THREAD CREATED!!\n");
					start_recvtest_thread (conf_settings);
/*
sender:
	cria thread para receber teste up (tipo de teste na mensagem???)
		--fim de teste:
		-- cria relatório de teste
		-- envia relatório de teste com mensagem do tipo feedback_test_up
		-- adiciona mensagem na lista de mensagem recebidas (workaround???) tipo loop_test_interval
	envia feedback para inicio de teste de up, tipo feedback_start_test_up
receiver:
	nada a fazer :(
*/
					pthread_mutex_lock (&(sendctrl_queue->mutex));

					ctrl_pkt **pkt_queue = sendctrl_queue->queue;
					sendctrl_queue->start = (sendctrl_queue->start + 1) % MAX_CTRL_QUEUE;
					pkt_queue[sendctrl_queue->start]->valid = 1;
					data = &(pkt_queue[sendctrl_queue->start]->data);
					data->cp_type = FEEDBACK_START_TEST_UP;
					data->packet_size = sizeof(ctrlpacket);

					pthread_mutex_unlock (&(sendctrl_queue->mutex));
				}
				else {
					pthread_mutex_lock (&(sendctrl_queue->mutex));

					ctrl_pkt **pkt_queue = sendctrl_queue->queue;
					sendctrl_queue->start = (sendctrl_queue->start + 1) % MAX_CTRL_QUEUE;
					pkt_queue[sendctrl_queue->start]->valid = 1;
					data = &(pkt_queue[sendctrl_queue->start]->data);
					data->cp_type = START_TEST_UP;
					data->packet_size = sizeof(ctrlpacket);

					pthread_mutex_unlock (&(sendctrl_queue->mutex));
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "SENDING TEST UP...!! -- statck %d\n", sendctrl_queue->start);
				}
				break;
			case FEEDBACK_START_TEST_UP:
				if (conf_settings->mode == RECEIVER) {
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "SEND TEST UP THREAD CREATED!!\n");
					start_sendtest_thread (conf_settings);
/*
sender:
	nada a fazer :(
receiver:
	cria thread para envio de teste de up
*/
				}
				break;
			case FEEDBACK_TEST_UP:
				if (conf_settings->mode == RECEIVER) {
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "SAVED REPORT TEST UP!!!\n");
					save_test_report (conf_settings, data);
/*
sender:
	nada a fazer :(
receiver:
	armazena relatório de teste up (feedback do server, alterar probe de teste???) 
		--fim do loop de teste:
			-- opção 1: não faz nada, espera loop no server para novo ciclo de teste down/up
			-- opção 2: finaliza threads de recv/send mensagens
			OBS: por enquanto... opção 1
*/
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
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_HIGH, "ADD MSG SENDER_CONNECT to RECV STACK!!\n");
/*
sender:
	loop(test_interval)
	envia mensagem de inicio de teste de down, tipo start_test_down (envio do tipo de teste, parametros...)
receiver:
	nada a fazer :(
*/
				}
				break;
			default:
				printf("WARNING: DEFAULT CASE ACTION!!!!\n");
				return;
		}
	}
}
