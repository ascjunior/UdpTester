/**\file recvctrl.h
 * \brief Declaração de métodos, tipos e constantes para controle  do  envio  de
 * testes.
 *  
 * \author Alberto Carvalho
 * \date August 28, 2013
 */

#ifndef __SENDCTRL_H__
#define __SENDCTRL_H__

/**\fn int sendctrl (settings *conf_settings)
 * \brief Realiza o controle do envio de testes.
 * 
 *   Cria thread para envio de teste e gera os logs de teste.
 * 
 * \param conf_settings Configurações de teste.
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int sendctrl (settings *conf_settings);

#endif /* __SENDCTRL_H__ */
