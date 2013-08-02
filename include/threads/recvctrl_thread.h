/**\file recvctrl_thread.h
 * \brief Declaração de constantes, tipos e métodos para troca  de  mensagens  de
 * controle do UdpTester.
 *  
 * \author Alberto Carvalho
 * \date July 05, 2013.
 */

#ifndef __RECVCTRL_THREAD_H__
#define __RECVCTRL_THREAD_H__

/**\fn void *recvctrl_thread (void *param)
 * \brief Thread para recepção de mensagens de controle
 * 
 * \param param Ponteiro para estrutura de configuração.
 * \return NULL.
 */
void *recvctrl_thread (void *param);

#endif /* __RECVCTRL_THREAD_H__ */
