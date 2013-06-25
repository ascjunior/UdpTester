/**\file defines.h
 * \brief Definição de tipos e constantes para o UdpTester.
 *  
 * \author Alberto Carvalho
 * \date June 25, 2013
 */

#ifndef __DEFINES_HH__
#define __DEFINES_HH__


/* constantes/ranges para parâmetros de entrada */

/**\def DEFAULT_CONTROL_PORT
 * \brief Porta default para tráfego de controle.
 */
#define	DEFAULT_CONTROL_PORT		33333

/**\def MIN_PACKET_SIZE
 * \brief Tamanho mínimo para pacote de teste.
 */
#define	MIN_PACKET_SIZE			64

/**\def MAX_PACKET_SIZE
 * \brief Tamanho máximo para pacote de teste.
 */
#define	MAX_PACKET_SIZE			1512

/**\def DEFAULT_PACKET_SIZE
 * \brief Tamanho default para pacote de teste.
 */
#define	DEFAULT_PACKET_SIZE		1280

/**\def MIN_TRAIN_SIZE
 * \brief Número mínimo de pacotes por trem.
 */
#define	MIN_TRAIN_SIZE			60

/**\def MAX_TRAIN_SIZE
 * \brief Número mínimo de pacotes por trem.
 */
#define	MAX_TRAIN_SIZE			300

/**\def DEFAULT_TRAIN_SIZE
 * \brief Número default de pacotes por trem.
 */
#define	DEFAULT_TRAIN_SIZE		180

/**\def MIN_TRAIN_NUM
 * \brief Número mínimo de trens por probe.
 */
#define	MIN_TRAIN_NUM			1

/**\def MAX_TRAIN_NUM
 * \brief Número máximo de trens por probe.
 */
#define	MAX_TRAIN_NUM			10

/**\def DEFAULT_TRAIN_NUM
 * \brief Número default de trens por probe.
 */
#define	DEFAULT_TRAIN_NUM		MAX_TRAIN_NUM

/**\def MIN_TRAIN_INTERVAL
 * \brief Intervalo mínimo entre trens de um mesmo probe (em ms).
 */
#define	MIN_TRAIN_INTERVAL		100

/**\def MIN_TRAIN_INTERVAL
 * \brief Intervalo máximo entre trens de um mesmo probe (em ms).
 */
#define	MAX_TRAIN_INTERVAL		2000

/**\def DEFAULT_TRAIN_INTERVAL
 * \brief Intervalo default entre trens de um mesmo probe (em ms).
 */
#define	DEFAULT_TRAIN_INTERVAL	500

/**\def MIN_TEST_INTERVAL
 * \brief Intervalo mínimo de teste (em s).
 */
#define	MIN_TEST_INTERVAL		5	

/**\def MAXTEST_INTERVAL
 * \brief Intervalo máximo de teste (em s).
 */
#define	MAX_TEST_INTERVAL		30

/**\def DEFAULT_TEST_INTERVAL
 * \brief Intervalo default de teste (em s).
 */
#define	DEFAULT_TEST_INTERVAL	10

/**\def MIN_LOOP_TEST_INTERVAL
 * \brief Intervalo mínimo entre testes (em m).
 */
#define	MIN_LOOP_TEST_INTERVAL		10	

/**\def MAX_LOOP_TEST_INTERVAL
 * \brief Intervalo máximo entre testes (em m).
 */
#define	MAX_LOOP_TEST_INTERVAL		120

/**\def DEFAULT_LOOP_TEST_INTERVAL
 * \brief Intervalo default entre teste (em m).
 */
#define	DEFAULT_LOOP_TEST_INTERVAL	15

/**\def MIN_SOC_BUFFER
 * \brief Tamanho mínimo para buffer do socket de testes (em KB).
 */
#define MIN_SOC_BUFFER			256

/**\def MAX_SOC_BUFFER
 * \brief Tamanho máximo para buffer do socket de testes (em KB).
 */
#define MAX_SOC_BUFFER			4096

/**\def DEFAULT_SOC_BUFFER
 * \brief Tamanho default para buffer do socket de testes (em KB).
 */
#define DEFAULT_SOC_BUFFER		1024

/**\def MIN_SEND_RATE
 * \brief  Taxa mínima de transmissão (em Mbps).
 */
#define MIN_SEND_RATE			0

/**\def MAX_SEND_RATE
 * \brief  Taxa máxima de transmissão (em Mbps).
 */
#define MAX_SEND_RATE			1000

/**\def DEFAULT_SEND_RATE
 * \brief Taxa default de transmissão (em Mbps).
 */
#define DEFAULT_SEND_RATE		MIN_SEND_RATE

/**\def MIN_REPORT_INTERVAL
 * \brief Intervalo mínimo entre reports de resultado (em s).
 */
#define	MIN_REPORT_INTERVAL		5	

/**\def MIN_REPORT_INTERVAL
 * \brief Intervalo máximo entre reports de resultado (em s).
 */
#define	MAX_REPORT_INTERVAL		30

/**\def DEFAULT_REPORT_INTERVAL
 * \brief Intervalo default entre reports de resultado (em s).
 */
#define	DEFAULT_REPORT_INTERVAL	10


/* constantes/ranges para variáveis internas */

/**\def BUFFER_SIZE
 * \brief Buffer máximo de transmissão/recepção de dados.
 */
#define	BUFFER_SIZE				2048

/**\def MAX_PACKET_PROBES
 * \brief Número máximo de pacotes em todas as probes.
 */
#define MAX_PACKET_PROBES		(MAX_TRAIN_NUM * MAX_TRAIN_SIZE)+11 	/* #trens x #pacotes + 10 pacotes de fim de probe */

/**\def WMEM_SOCKET_FILE
 * \brief Path para arquivo de configuração do buffer de transmissão do socket.
 */
#define WMEM_SBUFFER_FILE		"/proc/sys/net/core/wmem_max"

/**\def RMEM_SOCKET_FILE
 * \brief Path para arquivo de configuração do buffer de recepção do socket.
 */
#define RMEM_SBUFFER_FILE		"/proc/sys/net/core/rmem_max"

/**\def OVERHEAD_SIZE
 * \brief Overhead UDP/TCP.
 */
#define	OVERHEAD_SIZE			42

/**\enum packet_type
 * \brief Enumeração para os tipos de pacotes
 */
typedef enum {
	PACKET_CTRL = 0,		/**< Pacote de controle. */
	PACKET_TRAIN,			/**< Pacote de teste com trem de pacotes. */
	PACKET_CONT				/**< Pacote de teste com tráfego contínuo. */
} packet_type;


#endif /* __DEFINES_HH__ */
