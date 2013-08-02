/**\file ctrl.h
 * \brief Declaração de constantes, tipos e métodos de controle do UdpTester.
 *  
 * \author Alberto Carvalho
 * \date June 26, 2013
 */
 
#ifndef __CTRL_H__
#define __CTRL_H__

/**\fn int start_ctrl_connection (settings *conf_settings)
 * \brief Inicializa conexões de controle.
 * 
 * \param conf_settings Estrutura com as configurações de operação.
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int start_ctrl_connection (settings *conf_settings);

/**\fn ctrlpacket *getnext_ctrlpacket (ctrl_queue *queue)
 * \brief Get para o próximo pacote na fila queue.
 * 
 *   Retorna o ponteiro para o pacote e decrementa a fila.
 *
 * \param queue Fila para busca.
 * \return Ponteiro para o elemento em caso de sucesso, NULL caso contrário.
 */
ctrlpacket *getnext_ctrlpacket (ctrl_queue *queue);

/**\fn int start_sendctrl_connection (settings *conf_settings)
 * \brief Inicializa conexão para envio de mensagens de controle (exclusivo).
 * 
 * \param conf_settings Estrutura com as configurações de operação.
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int start_sendctrl_connection (settings *conf_settings);

/**\fn int start_sendtest_thread (settings *conf_settings)
 * \brief Inicializa thread para envio de teste udp.
 * 
 * \param conf_settings Estrutura com as configurações de operação.
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int start_sendtest_thread (settings *conf_settings);

/**\fn int start_recvtest_thread (settings *conf_settings)
 * \brief Inicializa thread para receber teste udp.
 * 
 * \param conf_settings Estrutura com as configurações de operação.
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int start_recvtest_thread (settings *conf_settings);

/**\fn void loop_control (settings *conf_settings)
 * \brief Loop de controle.
 * 
 *   Método para  ser  executado  consecutivamente  em  loop.  Verifica  a  fila
 * de mensagens de controle (mensagens recebidas) e executa as  ações  definidas
 * como resposta.
 * 
 * \param conf_settings Estrutura com as configurações de operação.
 */
void loop_control (settings *conf_settings);

#endif /* __CTRL_H__ */
