/**\file continuo_burst.h
 * \brief Declaração de constantes, tipos e métodos para teste udp  com  tráfego
 * contínuo limitado em bytes.
 *  
 * \author Alberto Carvalho
 * \date August 14, 2013
 */


#ifndef __CONTINUO_BURST__
#define __CONTINUO_BURST__

/**\fn int send_continuo_burst ()
 * \brief Envia teste udp de tráfego contínuo limitado em bytes.
 * 
 * \param port Porta UDP para teste.
 * \param soc_buffer_size Tamanho do buffer de transmissão.
 * \param send_rate Taxa de transmissão do teste.
 * \param burst_size Tamanho em MB dos dados.
 * \param packet_size Tamanho do pacote para transmissão.
 * \param log_file Arquivo para log de execução.
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int send_continuo_burst (int port, int soc_buffer_size, int send_rate, int burst_size, int packet_size, char *log_file);

/**\fn int recv_continuo_burst (int port, int soc_buffer_size, int report_interval, char *device)
 * \brief Recebe teste udp de tráfego contínuo limitado em bytes.
 * 
 * \param port Porta UDP para teste.
 * \param soc_buffer_size Tamanho do buffer de transmissão.
 * \param report_interval Intervalo de tempo para report de teste.
 * \param device Device que será observado no teste.
 * \param log_file Arquivo para log de execução.
 * \return
 */
int recv_continuo_burst (int port, int soc_buffer_size, int report_interval, char *device, char *log_file);

#endif /* __CONTINUO_BURST__ */
