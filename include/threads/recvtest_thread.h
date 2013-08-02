/**\file recvtest_thread.h
 * \brief Declaração de constantes, tipos e métodos para receber testes udp.
 *  
 * \author Alberto Carvalho
 * \date August 02, 2013.
 */

#ifndef __RECVTEST_THREAD_H__
#define __RECVTEST_THREAD_H__


/**\fn void *recvtest_thread (void *param)
 * \brief Thread de controle para envio de testes udp.
 * 
 * \param param Estrutura de dados com configurações para teste.
 * \return NULL.
 */
void *recvtest_thread (void *param);


#endif /* __RECVTEST_THREAD_H__ */
