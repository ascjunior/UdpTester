/**\file ctrl.c
 * \brief Definição de métodos de controle do UdpTester.
 *  
 * \author Alberto Carvalho
 * \date June 26, 2013
 */
 
#include <ctrl/ctrl.h>

 
void loop_control (settings *conf_settings) {
	fila_send_msg
		-- fila de mensagens de controle para send, verificada pela thread de controle que envia msg
	fila_rcv_msg
		-- fila populada pela thread de receive para mensagens de controle

	struct_sockets???
	socket_init()
		-- estrutura com dados dos sockets de send/rcv para mensagens de controle (deve ser tcp!!) e inicialização


	inicia_conexão_de_controle(conf_settings, struct_sockets)--send e receive???, uma ou duas threads de controle???
			2 THREADs de CTRL:
				- modo server: espera conexão de cliente
					- recebe sinal de inicio de teste
					- recebe log de teste
				- modo cliente: busca conexão com servidor
					- envia sinal de incio de teste
					- envia log de teste
			CALLBACKS para lista de msg??? -- NÃO: loop de controle
					
					Verifica lista de msg para envio
			THREAD para RCV de controle!, mensagens recebidas adicionadas a uma fila de controle, thread independente (e as msgs duplicadas?)
	if (conf_settings->mode == SENDER) {
		while (1) {
			loop_control_sender (conf_settings, )
			----entra no modo receiver????
			loop_control_receiver (conf_settings)
			sleep(conf_settings->loop_test_interval)
		}
	}
	else {
		while (1) {
			loop_control_receiver (conf_settings)
			sleep(30s)
			loop_control_sender (conf_settings)
			sleep(30s)
		}
	}
}

int start_ctrl_connection (settings *conf_settings) {
	int ret = 0;

	if (conf_settings->mode == RECEIVER) {
		inicializa thread para enviar mensagens de controle -- destino conhecido o server!
	}

	inicializa thread para receber mensagens de controle

	return ret;
}

//simplificado: executa teste continuo depois teste com trem de pacotes sempre
void loop_control_sender (settings *conf_settings) {
	send_inicio_teste () --através da conexão de controle envia inicio de teste e tipo de teste, adiona msg a lista de envio
		espera_resposta --através da lista de mensagens recebidas
	inicia_teste() --abre conexão, envia probes e retorna
	recv_report() --através da conexão de controle recebe report do teste, alterar probe???
	--envia mensagem de confirmação
}

void loop_control_receiver (settings *conf_settings) {
	
	while(modo=RECEIVER)
		sleep...
	{CALLBACK????
		espera_msg_controle --através da conexão de controle espera mensagem de inicio de teste de up --lista de mensagens recebidas
			mensagem de inicio de teste?
			send_ok
			inicia_recv_teste() --inicia recepção de teste, tipo, parametros...
			cria_relatorio() --cria relatorio do teste e imprime (log, console...)
			envia_relatorio_teste() --através da conexão de controle --lista de mensagens de envio
				--espera mensagem de confirmação
	}
	inicia_recv_teste() --inicia recepção de teste simplificado
	cria_relatorio() --cria relatorio do teste e imprime (log, console...)
	envia_relatorio_teste() --através da conexão de controle
}



--CONTROLE DENTRO DAS THREADS DE SEND/RCV DE MENSAGENS DE CONTROLE

server
	cria socket para listen
		recebe conexão
		espera mensagens do cliente
			pedido de inicio de teste down (parametrizado???)
				envia confirmação
				cria callback para envio de teste
			relatório de testes down
				envia confirmação
				callback para revisão dos parâmetros de testes??? --tabela com dados de testes para cada cliente(IP???)
			pedido de inicio de teste up (parametrizado???)
				envia confirmação
				cria callback para receber teste
			pedido de relatório de teste up
				envia relatório de teste up


cliente
	cria socket para connect ao server
		estabelece conexão
		envia pedido de inicio de teste down(parametros do teste???)
			recebe confirmação
			cria thread para receber testes
		envia relatório de testes
			recebe confirmação
		envia pedido de inicio de teste up
			recebe confirmação
			cria thread para enviar teste
		pedido de relatório de teste de up
			recebe relatório


------CONTROLE EXTERNOS AOS MÉTODOS DE SEND/RECEIVE (NECESSÁRIAS DUAS CONEXÕES)


SERVER_CTRL

	INIT
		recvctrl_thread: thread para receber msg
			recebe conexão	-- coloca mensagem tipo receiver_connect (com ip) na lista das mensagens recebidas
			espera mensagens de cliente	-- mensagens adicionadas na lista

	LOOP
		listen_msg_list()
		switch (msg_type)
			receiver_connect: --cliente conectado ao server, recebe msg
				cria thread para connect ao cliente (envio de msg) --conexão estabelecida?
																   --adiciona mensagem de sender_connect na lista das mensagens recebidas
			sender_connect: --server conectado ao cliente, envia msg
				envia mensagem de inicio de teste de down, tipo start_test_down (envio do tipo de teste??? parametros do teste???)
			feedback_start_test_down:
				cria thread para envio de teste de down
			feedback_test_down:
				armazena relatório de teste down (feedback do cliente)
			start_test_up:
				cria thread para receber teste up (tipo de teste na mensagem???)
					--fim de teste:
						-- cria relatório de teste
						-- envia relatório de teste com mensagem do tipo feedback_test_up
						-- adiciona mensagem na lista de mensagem recebidas (workaround!!!) tipo loop_test_interval
				envia feedback para inicio de teste de up, tipo feedback_start_test_up
			loop_test_interval:
				loop(test_interval)
				envia mensagem de inicio de teste de down, tipo start_test_down (envio do tipo de teste??? parametros do teste???)
				

CLIENT_CTRL

	INIT
		recvctrl_thread: thread para receber msg
			cria socket para listen
				recebe conexão	-- coloca mensagem de conexão estabelecida (receiver_connect) na lista das mensagens recebidas
				espera mensagens do server -- mensagens adicionadas na lista
		sendctrl_thread: thread para enviar msg
			cria socket para connect
				conexão estabelecida -- coloca mensagem de conexão estabelecida (sender_connect) na lista de mensagens recebidas

	LOOP

		listen_msg_list()
		switch (msg_type)
			sender_connect: --cliente conectado ao server, envia msg
				conexão estabelecida entre cliente e server -- nada a fazer :(
			receiver_connect: --server conectado ao cliente, recebe msg
				conexão estabelecida entre server e cliente -- nada a fazer :(
			start_test_down:	--mensagem enviada pelo server sinalizando inicio de teste de down
				cria thread para receber teste (tipo de teste na mensagem???)
					--fim de teste:
						-- cria relatório de teste
						-- envia relatório de teste com mensagem do tipo feedback_test_down
						-- envia mensagem de inicio de teste de up com tipo start_test_up
				envia mensagem de feedback_start_test_down
			feedback_start_test_up:
				cria thread para envio de teste de up
			feedback_test_up:
				armazena relatório de teste up (feedback do server) 
					--fim do loop de teste:
						-- opção 1: não faz nada, espera loop no server para novo ciclo de teste down/up
						-- opção 2: finaliza threads de recv/send mensagens
						OBS: por enquanto... opção 1
				


RESUMO:
	SERVER_CTRL											CLIENT_CTRL
		recvctrl_thread										recvctrl_thread
															sendctrl_thread (conhece o destino: o server)
		switch(lstmsg->top)									switch(lstmsg->top)
			receiver_connect:									sender_connect:
				sendctrl_thread										nop
			sender_connect:										receiver_connect:
				send start_test_down								nop
			feedback_start_test_down:							start_test_down:
				sendtest_thread										recvtest_thread
			feedback_test_down:											create_result
				save_result_down										send feedback_test_down
			start_test_up:												send start_test_up
				recvtest_thread										send feedback_start_test_down
					create_result								feedback_start_test_up:
					send feedback_test_up							sendtest_thread
					loop_test_interval							feedback_test_up:
				send feedback_start_test_up							save_result_up
			loop_test_interval:
				loop(test_interval)
				send start_test_down
			
