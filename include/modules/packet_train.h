/**\file packet_train.h
 * \brief Declaração de constantes, tipos e métodos para teste udp  com  tráfego
 * de trens de pacotes.
 *  
 * \author Alberto Carvalho
 * \date August 29, 2013
 */


#ifndef __PACKET_TRAIN__
#define __PACKET_TRAIN__

#include <defines.h>

/**\fn void *send_packet_train (void *param)
 * \brief Thread para envio de teste udp de tráfego de trem de pacotes.
 * 
 * \param param Estrutura de dados com configurações do teste.
 * \return NULL.
 */
void *send_packet_train (void *param);

/**\fn void *recv_packet_train (void *param)
 * \brief Thread para recepção de teste udp de tráfego de trem de pacotes.
 * 
 *   Análise do teste é realizada através da libpcap.
 * 
 * \param param Estrutura de dados com configurações e resultados do teste.
 * \return NULL.
 */
void *recv_packet_train (void *param);

/**\fn void *recv2_packet_train (void *param)
 * \brief Thread para recepção de teste udp de tráfego de trem de pacotes.
 * 
 * 
 * \param param Estrutura de dados com configurações e resultados do teste.
 * \return NULL.
 */
void *recv2_packet_train (void *param);

/**\fn void *timeout_train_thread (void *param)
 * \brief Thread para verificação de timeout na recepção de teste.
 * 
 * \param param Configurações, resultados e andamento do teste.
 * \return NULL.
 */
void *timeout_train_thread (void *param);

/**\fn void print_train_result (settings *conf_settings, received_probe *probe, int ended)
 * \brief Imprime o resultado atual do teste.
 * 
 * \param conf_settings Configurações de teste.
 * \param probe Estrutura de dados com as informações do teste.
 * \param ended Flag que sinaliza fim de teste.
 */
void print_train_result (settings *conf_settings, received_probe *probe, resume *result);

#endif /* __PACKET_TRAIN__ */
