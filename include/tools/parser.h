/**\file parser.h
 * \brief Declaração de métodos para parser de linha de comando no UdpTester.
 *  
 * \author Alberto Carvalho
 * \date June 26, 2013.
 */

#ifndef __PARSER_H__
#define __PARSER_H__

/**\fn void usage(void) 
 * \brief Help básico para uso do UdpTester.
 */
void usage(void);

/**\fn void help(void) 
 * \brief Help completo para uso do UdpTester.
 */
void help(void);

/**\fn void init_settings (settings *conf_settings)
 * \brief Inicializa estrutura de dados com parâmetros de configuração.
 * 
 * \param conf_settings estrutura de dados a ser incializada.
 */
void init_settings (settings *conf_settings);

/**\fn int parse_command_line (int argc, char **argv, settings *conf_settings)
 * \brief Realiza o parser da linha de comando.
 * 
 * \param argc Número de argumentos.
 * \param argv Lista de argumentos
 * \param conf_settings estrutura de dados que receberá os parâmetros.
 * \return 0 em caso de sucesso, -1 em caso de falha e 1 em caso de mensagens de
 * help ou version.
 */
int parse_command_line (int argc, char **argv, settings *conf_settings);

#endif /* __PARSER_H__ */
