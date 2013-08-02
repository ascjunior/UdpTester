/**\file sendtest_thread.h
 * \brief Declaração de constantes, tipos e métodos para envio de testes udp.
 *  
 * \author Alberto Carvalho
 * \date August 02, 2013.
 */

#ifndef __SENDTEST_THREAD_H__
#define __SENDTEST_THREAD_H__


/**\fn void *sendtest_thread (void *param)
 * \brief Thread de controle para envio de testes udp.
 * 
 * \param param Estrutura de dados com configurações para teste.
 * \return NULL.
 */
void *sendtest_thread (void *param);


#endif /* __SENDTEST_THREAD_H__ */
