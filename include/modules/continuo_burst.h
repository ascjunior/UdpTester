/**\file continuo_burst.h
 * \brief Declaração de constantes, tipos e métodos para teste udp  com  tráfego
 * contínuo limitado em bytes.
 *  
 * \author Alberto Carvalho
 * \date August 14, 2013
 */


#ifndef __CONTINUO_BURST__
#define __CONTINUO_BURST__

#include <defines.h>

/**\fn void *send_continuo_burst (void *param)
 * \brief Thread para envio de teste udp de tráfego contínuo limitado em bytes.
 * 
 * \param param Estrutura de dados com configurações do teste.
 * \return NULL.
 */
void *send_continuo_burst (void *param);

/**\fn void *recv_continuo_burst (void *param)
 * \brief Thread para recepção de teste udp de tráfego contínuo limitado em bytes.
 * 
 *   Análise do teste é realizada através da libpcap.
 * 
 * \param param Estrutura de dados com configurações e resultados do teste.
 * \return NULL.
 */
void *recv_continuo_burst (void *param);

/**\fn void *recv2_continuo_burst (void *param)
 * \brief Thread para recepção de teste udp de tráfego contínuo limitado em bytes.
 * 
 * 
 * \param param Estrutura de dados com configurações e resultados do teste.
 * \return NULL.
 */
void *recv2_continuo_burst (void *param);

/**\fn void *timeout_thread (void *param)
 * \brief Thread para verificação de timeout na recepção de teste.
 * 
 * \param param Configurações, resultados e andamento do teste.
 * \return NULL.
 */
void *timeout_thread (void *param);

/**\fn void print_result (settings *conf_settings, received_probe *probe, int ended)
 * \brief Imprime o resultado atual do teste.
 * 
 * \param conf_settings Configurações de teste.
 * \param probe Estrutura de dados com as informações do teste.
 * \param ended Flag que sinaliza fim de teste.
 */
void print_result (settings *conf_settings, received_probe *probe, resume *result);

#endif /* __CONTINUO_BURST__ */
