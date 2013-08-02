/**\file defines.h
 * \brief Definição de tipos e constantes para o UdpTester.
 *  
 * \author Alberto Carvalho
 * \date June 25, 2013
 */

#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* constantes/ranges para parâmetros de entrada */

/**\def DEFAULT_DATA_PORT
 * \brief Porta default para tráfego de dados.
 */
#define	DEFAULT_DATA_PORT		33333

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

/**\def MAX_PAYLOAD
 * \brief Tamanho máximo para payload dos pacotes de controle
 */
#define MAX_PAYLOAD				972

/**\def MAX_CTRL_QUEUE
 * \brief Tamanho máximo para fila de pacotes de controle
 */
#define MAX_CTRL_QUEUE			1024

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

/**\def BACKLOG
 * \brief Número máximo de conexões de controle em espera.
 */
#define BACKLOG 10

/* provisório */
#define SEND_PORT	11999
#define RECV_PORT	22999
 
/**\def OVERHEAD_SIZE
 * \brief Overhead UDP/TCP.
 */
#define	OVERHEAD_SIZE			42

#ifndef VERSION_CODE_MAJOR
#define VERSION_CODE_MAJOR		0
#endif
#ifndef VERSION_CODE_MINOR
#define VERSION_CODE_MINOR		1
#endif

/**\def STR_VERSION
 * \brief String de versão.
 */
#define STR_VERSION				"UdpTester Version "


/**\enum mode_type
 * \brief Enumeração para os modos de operação do UdpTester
 */
typedef enum {
	RECEIVER = 0,			/**< Modo Receiver. >*/
	SENDER,					/**< Modo Sender. >*/
} mode_type;

/**\enum arg_type
 * \brief Enumeração para os tipos de parâmetros de entrada.
 */
typedef enum {
	NO_ARGUMENT=0,		/**< Argumento não requerido. >*/
	REQ_ARGUMENT,		/**< Argumento requerido. >*/
	OPT_ARGUMENT		/**< Argumento opcional. >*/
} arg_type;

/**\struct option
 * \brief Estrutura de dados para parâmetros de entrada
 */
typedef struct {
    const char *name;		/**< Parâmetro por extenso. >*/
    int has_arg;			/**< Informação de argumento. >*/
    int *flag;				/**< Flag de status. >:*/
    int val;				/**< Parâmetro reduzido???. >*/
} option;

/**\enum test_type
 * \brief Enumeração para os tipos de teste.
 */
typedef enum {
	UDP_CONTINUO = 0,			/**< Teste UDP com tráfego contínuo. >*/
	UDP_PACKET_TRAIN,			/**< Teste UDP com trem de pacotes. >*/
} test_type;

/**\enum ctrlpacket_type
 * \brief Enumeração para os tipos de pacotes de controle.
 */
typedef enum {
	RECEIVER_CONNECT = 0,		/**< Sinalização para conexão recebida. >*/
	SENDER_CONNECT,				/**< Sinalização para conexão estabelecida com host remoto. >*/
	START_TEST_DOWN,			/**< Requisição para teste de download. >*/
	FEEDBACK_START_TEST_DOWN,	/**< Confirmação para início de teste de download. >*/
	FEEDBACK_TEST_DOWN,			/**< Relatório de teste de download. >*/
	START_TEST_UP,				/**< Requisição para teste de upload. >*/
	FEEDBACK_START_TEST_UP,		/**< Confirmação para início de teste de upload. >*/
	FEEDBACK_TEST_UP,			/**< Relatório de teste de upload. >*/
	LOOP_TEST_INTERVAL,			/**< Mensagem para inicio de intervalo entre testes. >*/
} ctrlpacket_type;

/**\struct report
 * \brief Relatório de teste udp.
 */
typedef struct {
	test_type t_type;					/**< Tipo de teste realizado. */
	int packet_size;					/**< Tamanho médio do pacote recebido. >*/
	int packet_num;						/**< Número de pacotes recebidos. >*/
	int bytes;							/**< Número de byres recebidos. >*/
	int train_size;						/**< Tamanho médio dos trens de pacotes (número de pacotes). >*/
	int train_num;						/**< Número de trens de pacotes. >*/
	int train_interval;					/**< Intervalo entre trens de pacotes. >*/
	int udp_port;						/**< Porta para teste UDP. >*/
	struct timeval bw_med;				/**< Banda média calculada (em Mbps). >*/
	struct timeval bw_max;				/**< Banda máxima calculada (em Mbps). >*/
	struct timeval bw_min;				/**< Banda mínima calculada (em Mbps). >*/
	int jitter_med;						/**< Jitter médio observado (em us). >*/
	int jitter_max;						/**< Jitter máximo calculado (em us). >*/
	int jitter_min;						/**< Jitter mínimo calculado (em us). >*/
	struct timeval time_med;			/**< Tempo médio de teste (em s). >*/
	struct timeval time_max;			/**< Tempo máximo de teste (em s). >*/
	struct timeval time_min;			/**< Tempo mínimo de teste (em s). >*/
	int lost_med;						/**< Número médio de pacotes perdidos. >*/
	int lost_max;						/**< Número máximo de pacotes perdidos. >*/
	int lost_min;						/**< Número mínimo de pacotes perdidos. >*/
} report;

/**\struct ctrlpacket
 * \brief Estrutura para os pacotes de controle.
 */
typedef struct {
	ctrlpacket_type cp_type;			/**< Tipo de pacote de controle. >*/
	int packet_size;					/**< Tamanho do pacote (header+payload). >*/
	uint connected_address;				/**< Endereço do host remoto conectado ao server. >*/
	test_type t_type;					/**< Tipo de teste (contínuo ou trem de pacotes). >*/
	int test_packet_size;				/**< Tamanho do pacote para teste. >*/
	int test_train_size;				/**< Tamanho do trem de pacotes (número de pacotes). >*/
	int test_train_num;					/**< Número de trens de pacotes (intervalo de teste, caso teste contínuo). >*/
	int test_train_interval;			/**< Intervalo entre trens de pacotes (ou intervalo para amostragem em caso de teste contínuo). >*/
	int test_udp_port;					/**< Porta para teste UDP. >*/
	int test_udp_rate;					/**< Taxa de transmissão do teste UDP (em Mbps). >*/
	int test_sendsock_buffer;			/**< Tamanho do buffer do socket para envio do teste. >*/
	int test_recvsock_buffer;			/**< Tamanho do buffer do socket para receber o teste. >*/
	int test_loop_interval;				/**< Intervalo entre testes. >*/
	char buffer[MAX_PAYLOAD];			/**< Payload para relatório do teste. >*/
} ctrlpacket;

/**\struct ctrl_pkt
 * \brief Estrutura para lista de pacotes de controle.
 */
typedef struct {
	char valid;							/**< Flag de validade da informação no pacote. >*/
	ctrlpacket *data;					/**< Ponteiro para pacote de controle. >*/
} ctrl_pkt;

/**\struct ctrl_queue
 * \brief Estrutura para fila de pacotes de controle.
 */
typedef struct {
	short start;							/**< Indíce para o início da fila (inserção). >*/
	short end;								/**< Indíce para o fim da fila (remoção). >*/
	short size;								/**< Número de elementos válidos na fila. >*/
	ctrl_pkt **queue;						/**< Ponteiro para pacote de controle. >*/
} ctrl_queue;

/**\struct settings
 * \brief Lista de configurações do UdpTester.
 */
typedef struct {
	mode_type mode;					/**< Modo de operação do UdpTester. >*/
	int bandwidth;					/**< Bandwidth para trasmissão em Mbps. >*/
	int packet_size;				/**< Tamanho do pacotes em bytes. >*/
	int train_num;					/**< Número de trens de pacotes por teste.> */
	int train_size;					/**< Número de pacotes por trem. >*/
	int train_interval;				/**< Intervalo entre trens de pacotes de um mesmo teste. >*/
	int loop_test_interval;			/**< Intervalo entre testes consecutivos. >*/
	int report_interval;			/**< Intervalo de report para testes de tráfego contínuos. >*/
	int test_interval;				/**< Intervalo de teste para teste contínuo. >*/
	int soc_buffer;					/**< Tamanho para o buffer do socket em KB. > */
	int port;						/**< Porta para tráfego de dados. >*/
	struct sockaddr_in address;		/**< Endereço de destino da transmissão. >*/
	ctrl_queue recv_queue;			/**< Fila de pacotes de controle recebidos. >*/
	ctrl_queue send_queue;			/**< Fila de pacotes de controle para envio. >*/
} settings;

#endif /* __DEFINES_H__ */
