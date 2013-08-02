/**\file sendctrl_thread.h
 * \brief Declaração de constantes, tipos e métodos para troca  de  mensagens  de
 * controle do UdpTester.
 *  
 * \author Alberto Carvalho
 * \date July 05, 2013.
 */

#ifndef __SENDCTRL_THREAD_H__
#define __SENDCTRL_THREAD_H__

/**\fn void *sendctrl_thread (void *param)
 * \brief Thread para envio de mensagens de controle
 * 
 * \param param Ponteiro para estrutura de configuração.
 * \return NULL.
 */
void *sendctrl_thread (void *param);

#endif /* __SENDCTRL_THREAD_H__ */
