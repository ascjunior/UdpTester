/**\file recvctrl.h
 * \brief Declaração de métodos, tipos e constantes para controle da recepção de
 * testes.
 *  
 * \author Alberto Carvalho
 * \date August 28, 2013
 */

#ifndef __RECVCTRL_H__
#define __RECVCTRL_H__

/**\fn int recvctrl (settings *conf_settings)
 * \brief Realiza o controle da recepção de testes.
 * 
 *   Cria thread para recepção de teste, cria mensagem de controle  para  inicio
 * de teste,  gera  os  resultados, logs e mensagens de feedback do teste.
 * 
 * \param conf_settings Configurações de teste.
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int recvctrl (settings *conf_settings);

#endif /* __RECVCTRL_H__ */
