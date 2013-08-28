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
#define	DEFAULT_DATA_PORT					33333

/**\def MIN_PACKET_SIZE
 * \brief Tamanho mínimo para pacote de teste.
 */
#define	MIN_PACKET_SIZE						64

/**\def MAX_PACKET_SIZE
 * \brief Tamanho máximo para pacote de teste.
 */
#define	MAX_PACKET_SIZE						1512

/**\def DEFAULT_PACKET_SIZE
 * \brief Tamanho default para pacote de teste.
 */
#define	DEFAULT_PACKET_SIZE					1280

/**\def MIN_TRAIN_SIZE
 * \brief Número mínimo de pacotes por trem.
 */
#define	MIN_TRAIN_SIZE						60

/**\def MAX_TRAIN_SIZE
 * \brief Número mínimo de pacotes por trem.
 */
#define	MAX_TRAIN_SIZE						300

/**\def DEFAULT_TRAIN_SIZE
 * \brief Número default de pacotes por trem.
 */
#define	DEFAULT_TRAIN_SIZE					180

/**\def MIN_TRAIN_NUM
 * \brief Número mínimo de trens por probe.
 */
#define	MIN_TRAIN_NUM						1

/**\def MAX_TRAIN_NUM
 * \brief Número máximo de trens por probe.
 */
#define	MAX_TRAIN_NUM						10

/**\def DEFAULT_TRAIN_NUM
 * \brief Número default de trens por probe.
 */
#define	DEFAULT_TRAIN_NUM					MAX_TRAIN_NUM

/**\def MIN_TRAIN_INTERVAL
 * \brief Intervalo mínimo entre trens de um mesmo probe (em ms).
 */
#define	MIN_TRAIN_INTERVAL					100

/**\def MIN_TRAIN_INTERVAL
 * \brief Intervalo máximo entre trens de um mesmo probe (em ms).
 */
#define	MAX_TRAIN_INTERVAL					2000

/**\def DEFAULT_TRAIN_INTERVAL
 * \brief Intervalo default entre trens de um mesmo probe (em ms).
 */
#define	DEFAULT_TRAIN_INTERVAL				500

/**\def MIN_TEST_INTERVAL
 * \brief Intervalo mínimo de teste (em ms).
 */
#define	MIN_TEST_INTERVAL					100	

/**\def MAXTEST_INTERVAL
 * \brief Intervalo máximo de teste (em ms).
 */
#define	MAX_TEST_INTERVAL					10000

/**\def DEFAULT_TEST_INTERVAL
 * \brief Intervalo default de teste (em ms).
 */
#define	DEFAULT_TEST_INTERVAL				1000

/**\def MIN_LOOP_TEST_INTERVAL
 * \brief Intervalo mínimo entre testes (em s).
 */
#define	MIN_LOOP_TEST_INTERVAL				10	

/**\def MAX_LOOP_TEST_INTERVAL
 * \brief Intervalo máximo entre testes (em s).
 */
#define	MAX_LOOP_TEST_INTERVAL				120

/**\def DEFAULT_LOOP_TEST_INTERVAL
 * \brief Intervalo default entre teste (em s).
 */
#define	DEFAULT_LOOP_TEST_INTERVAL			15

/**\def MIN_SOC_BUFFER
 * \brief Tamanho mínimo para buffer do socket de testes (em KB).
 */
#define MIN_SOC_BUFFER						256

/**\def MAX_SOC_BUFFER
 * \brief Tamanho máximo para buffer do socket de testes (em KB).
 */
#define MAX_SOC_BUFFER						4096

/**\def DEFAULT_SOC_BUFFER
 * \brief Tamanho default para buffer do socket de testes (em KB).
 */
#define DEFAULT_SOC_BUFFER					1024

/**\def MIN_SEND_RATE
 * \brief  Taxa mínima de transmissão (em Mbps).
 */
#define MIN_SEND_RATE						0

/**\def MAX_SEND_RATE
 * \brief  Taxa máxima de transmissão (em Mbps).
 */
#define MAX_SEND_RATE						1000

/**\def DEFAULT_SEND_RATE
 * \brief Taxa default de transmissão (em Mbps).
 */
#define DEFAULT_SEND_RATE					MIN_SEND_RATE

/**\def MIN_REPORT_INTERVAL
 * \brief Intervalo mínimo entre reports de resultado (em ms).
 */
#define	MIN_REPORT_INTERVAL					100

/**\def MIN_REPORT_INTERVAL
 * \brief Intervalo máximo entre reports de resultado (em ms).
 */
#define	MAX_REPORT_INTERVAL					30000

/**\def DEFAULT_REPORT_INTERVAL
 * \brief Intervalo default entre reports de resultado (em ms).
 */
#define	DEFAULT_REPORT_INTERVAL				10000

/**\def MIN_BURST_SIZE
 * \brief Tamanho mínimo do burst de dados para teste (em MB).
 */
#define MIN_BURST_SIZE						2

/**\def MAX_BURST_SIZE
 * \brief Tamanho máximo do burst de dados para teste (em MB).
 */
#define MAX_BURST_SIZE						1000

/**\def DEFAULT_BURST_SIZE
 * \brief Tamanho default do burst de dados para teste (em MB).
 */
#define DEFAULT_BURST_SIZE					5


/* constantes/ranges para variáveis internas */

/**\def BUFFER_SIZE
 * \brief Buffer máximo de transmissão/recepção de dados.
 */
#define	BUFFER_SIZE							2048

/**\def STR_SIZE
 * \brief Buffer máximo para strings de nome (hostname, interface...)
 */
#define	STR_SIZE							256

/**\def MAX_PAYLOAD
 * \brief Tamanho máximo para payload dos pacotes de controle
 */
#define MAX_PAYLOAD							972

/**\def MAX_CTRL_QUEUE
 * \brief Tamanho máximo para fila de pacotes de controle
 */
#define MAX_CTRL_QUEUE						1024

/**\def MAX_PACKET_PROBES
 * \brief Número máximo de pacotes em todas as probes.
 */
#define MAX_PACKET_PROBES					(MAX_TRAIN_NUM * MAX_TRAIN_SIZE)+11 	/* #trens x #pacotes + 10 pacotes de fim de probe */

/**\def WMEM_SOCKET_FILE
 * \brief Path para arquivo de configuração do buffer de transmissão do socket.
 */
#define WMEM_SBUFFER_FILE					"/proc/sys/net/core/wmem_max"

/**\def RMEM_SOCKET_FILE
 * \brief Path para arquivo de configuração do buffer de recepção do socket.
 */
#define RMEM_SBUFFER_FILE					"/proc/sys/net/core/rmem_max"

/**\def BACKLOG
 * \brief Número máximo de conexões de controle em espera.
 */
#define BACKLOG 10

/**\def NPACKET_END_TEST
 * \brief Número de pacotes de fim de teste.
 */
#define NPACKET_END_TEST					100

/**\def PACKET_END_TEST
 * \brief Identificador de pacote de fim de teste.
 */
#define PACKET_END_TEST						-1

/* provisório */
#define SEND_PORT	11999
#define RECV_PORT	22999
 
/**\def OVERHEAD_SIZE
 * \brief Overhead UDP/TCP.
 */
#define	OVERHEAD_SIZE						42

#ifndef VERSION_CODE_MAJOR
#define VERSION_CODE_MAJOR					0
#endif
#ifndef VERSION_CODE_MINOR
#define VERSION_CODE_MINOR					1
#endif

/**\def STR_VERSION
 * \brief String de versão.
 */
#define STR_VERSION							"UdpTester Version "

/**\def DEFAULT_SENDCONT_BURST_LOGFILE
 * \brief Arquivo default para log da transmissão  de  teste  contínuo udp  com
 * limite em bytes.
 */
#define	DEFAULT_SENDCONT_BURST_LOGFILE		"UdpTester_SendUDPContBytes.txt"

/**\def DEFAULT_RECVCONT_BURST_LOGFILE
 * \brief Arquivo default para log da recepção de teste contínuo udp  com limite
 * em bytes.
 */
#define	DEFAULT_RECVCONT_BURST_LOGFILE		"UdpTester_RecvUDPContBytes.txt"

/**def DEFAULT_INTERFACE_NAME
 * \brief Interface default para observação em teste udp contínuo.
 */
#define DEFAULT_INTERFACE_NAME				"eth0"

/**\def RECVCAPTURE_TIMEOUT
 * \brief Timeout default para captura de teste udp contínuo (em ms).
 */
#define RECVCAPTURE_TIMEOUT					1000

/**\def u_long
 * \brief Abreviação para unsigned long.
 */
#ifndef u_long
typedef	unsigned long u_long;
#endif

/**\enum mode_type
 * \brief Enumeração para os modos de operação do UdpTester
 */
typedef enum {
	RECEIVER = 0,							/**< Modo Receiver. >*/
	SENDER,									/**< Modo Sender. >*/
} mode_type;

/**\enum arg_type
 * \brief Enumeração para os tipos de parâmetros de entrada.
 */
typedef enum {
	NO_ARGUMENT=0,							/**< Argumento não requerido. >*/
	REQ_ARGUMENT,							/**< Argumento requerido. >*/
	OPT_ARGUMENT							/**< Argumento opcional. >*/
} arg_type;

/**\struct option
 * \brief Estrutura de dados para parâmetros de entrada
 */
typedef struct {
    const char *name;						/**< Parâmetro por extenso. >*/
    int has_arg;							/**< Informação de argumento. >*/
    int *flag;								/**< Flag de status. >:*/
    int val;								/**< Parâmetro reduzido???. >*/
} option;

/**\enum test_type
 * \brief Enumeração para os tipos de teste.
 */
typedef enum {
	UDP_CONTINUO = 0,						/**< Teste UDP com tráfego contínuo. >*/
	UDP_PACKET_TRAIN,						/**< Teste UDP com trem de pacotes. >*/
} test_type;

/**\enum ctrlpacket_type
 * \brief Enumeração para os tipos de pacotes de controle.
 */
typedef enum {
	RECEIVER_CONNECT = 0,					/**< Sinalização para conexão recebida. >*/
	SENDER_CONNECT,							/**< Sinalização para conexão estabelecida com host remoto. >*/
	SENDER_UDPCONNECT,						/**< Sinalização enviado pelo lado transmissor da conexão de controle. >*/
	START_TEST_DOWN,						/**< Requisição para teste de download. >*/
	FEEDBACK_START_TEST_DOWN,				/**< Confirmação para início de teste de download. >*/
	FEEDBACK_TEST_DOWN,						/**< Relatório de teste de download. >*/
	START_TEST_UP,							/**< Requisição para teste de upload. >*/
	FEEDBACK_START_TEST_UP,					/**< Confirmação para início de teste de upload. >*/
	FEEDBACK_TEST_UP,						/**< Relatório de teste de upload. >*/
	LOOP_TEST_INTERVAL,						/**< Mensagem para inicio de intervalo entre testes. >*/
} ctrlpacket_type;

/**\struct timeval32
 * \brief Estrutura de dados para substituição da struct timeval.
 * 
 *   A struct timeval utiliza o tipo long int, que apresenta tamanhos diferentes
 * entre máquina 32 e 64bits, causando perda de informações durante a  troca  de
 * pacotes.
 */
typedef struct {
	int tv_sec;								/**< Porção do tempo em segundos. >*/
	int tv_usec;							/**< Porção do tempo em microssegundos. >*/
} timeval32;

/**\struct ttype_cont
 * \brief Estrutura de dados com informações específicas para testes com tráfego
 * contínuo.
 */
typedef struct {
	int size;								/**< Tamanho em MB do burst de dados transmitidos no teste (continuo). */
	int pkt_num;							/**< Número de pacotes. >*/
	int interval;							/**< Intervalo de teste. >*/
	int report_interval;					/**< Intervalo para amostragem. >*/
} ttype_cont;

/**\struct ttype_train
 * \brief Estrutura de dados com informações específicas para testes com tráfego
 * de trem de pacotes.
 */
typedef struct {
	int num;								/**< Número de trens de pacotes . >*/
	int size;								/**< Tamanho (em número de pacotes) dos trens de pacotes . >*/
	int interval;							/**< Intervalo entre trens de pacotes. >*/
} ttype_train;

/**\struct resume
 * \brief Relatório de teste udp (apenas resultados observados no receiver).
 */
typedef struct {
	int packet_size;						/**< Tamanho médio dos pacotes recebido. >*/
	int packet_num;							/**< Número de pacotes recebidos. >*/
	int bytes;								/**< Número de bytes recebidos. >*/
	int train_size;							/**< Tamanho médio dos trens de pacotes (número de pacotes). >*/
	int train_num;							/**< Número de trens de pacotes. >*/
	int train_interval;						/**< Intervalo entre trens de pacotes. >*/
	timeval32 bw_med;						/**< Banda média calculada (em Mbps). >*/
	timeval32 bw_max;						/**< Banda máxima calculada (em Mbps). >*/
	timeval32 bw_min;						/**< Banda mínima calculada (em Mbps). >*/
	int jitter_med;							/**< Jitter médio observado (em us). >*/
	int jitter_max;							/**< Jitter máximo calculado (em us). >*/
	int jitter_min;							/**< Jitter mínimo calculado (em us). >*/
	timeval32 time_med;						/**< Tempo médio de teste (em s). >*/
	timeval32 time_max;						/**< Tempo máximo de teste (em s). >*/
	timeval32 time_min;						/**< Tempo mínimo de teste (em s). >*/
	int loss_med;							/**< Número médio de pacotes perdidos. >*/
	int loss_max;							/**< Número máximo de pacotes perdidos. >*/
	int loss_min;							/**< Número mínimo de pacotes perdidos. >*/
} resume;

/**\struct report
 * \brief Relatório de teste udp.
 */
typedef struct {
	test_type t_type;						/**< Tipo de teste realizado. */
	resume result;							/**< Resumo de resultados do teste. >*/
	int udp_rate;							/**< Taxa de transmissão do teste (em Mpbs). >*/
	int udp_port;							/**< Porta para teste UDP. >*/
	int packet_size;						/**< Tamanho do pacote enviado no teste. >*/
	union {
		ttype_cont cont;					/**< Configurações específicas para teste com tráfego contínuo. >*/
		ttype_train train;					/**< Configurações específicas para teste com tráfego de trem de pacotes. >*/
	} test;
} report;

/**\struct ctrlpacket
 * \brief Estrutura para os pacotes de controle.
 */
typedef struct {
	ctrlpacket_type cp_type;				/**< Tipo de pacote de controle. >*/
	test_type t_type;						/**< Tipo de teste (contínuo ou trem de pacotes). >*/
	struct sockaddr_in connected_address;	/**< Endereço do host remoto conectado ao server. >*/
	int udp_port;							/**< Porta para teste UDP. >*/
	int udp_rate;							/**< Taxa de transmissão do teste UDP (em Mbps). >*/
	int packet_size;						/**< Tamanho do pacote para teste. >*/
	int loop_interval;						/**< Intervalo entre testes. >*/
	int sendsock_buffer;					/**< Tamanho do buffer do socket para envio do teste. >*/
	int recvsock_buffer;					/**< Tamanho do buffer do socket para receber o teste. >*/
	union {
		ttype_cont cont;					/**< Configurações específicas para teste com tráfego contínuo. >*/
		ttype_train train;					/**< Configurações específicas para teste com tráfego de trem de pacotes. >*/
	} test;
	char buffer[MAX_PAYLOAD];				/**< Payload para relatório do teste. >*/
} ctrlpacket;

/**\struct ctrl_pkt
 * \brief Estrutura para lista de pacotes de controle.
 */
typedef struct {
	char valid;								/**< Flag de validade da informação no pacote. >*/
	ctrlpacket data;						/**< Ponteiro para pacote de controle. >*/
} ctrl_pkt;

/**\struct ctrl_queue
 * \brief Estrutura para fila de pacotes de controle.
 */
typedef struct {
	short start;							/**< Indíce para o início da fila (inserção). >*/
	short end;								/**< Indíce para o fim da fila (remoção). >*/
	short size;								/**< Número de elementos válidos na fila. >*/
	pthread_mutex_t mutex;					/**< Semáforo de controle de acesso. */
	ctrl_pkt **queue;						/**< Ponteiro para pacote de controle. >*/
} ctrl_queue;

/**\struct settings
 * \brief Lista de configurações do UdpTester.
 */
typedef struct {
	mode_type mode;							/**< Modo de operação do UdpTester. >*/
	test_type t_type;						/**< Tipo de teste (contínuo ou trem de pacotes). >*/
	struct sockaddr_in address;				/**< Endereço de destino da transmissão. >*/
	int udp_port;							/**< Porta para tráfego de dados. >*/
	int udp_rate;							/**< Bandwidth para trasmissão em Mbps. >*/
	int packet_size;						/**< Tamanho do pacotes em bytes. >*/
	int loop_interval;						/**< Intervalo entre testes consecutivos. >*/
	int sendsock_buffer;					/**< Tamanho do buffer do socket para envio do teste. >*/
	int recvsock_buffer;					/**< Tamanho do buffer do socket para receber o teste. >*/
	char iface[STR_SIZE];					/**< Interface para captura da transmissão de dados. */
	union {
		ttype_cont cont;					/**< Configurações específicas para teste com tráfego contínuo. >*/
		ttype_train train;					/**< Configurações específicas para teste com tráfego de trem de pacotes. >*/
	} test;
	ctrl_queue recv_queue;					/**< Fila de pacotes de controle recebidos. >*/
	ctrl_queue send_queue;					/**< Fila de pacotes de controle para envio. >*/
} settings;

/**\struct packet_probe
 * \brief Estrutura de dados para o pacote de teste.
 */
typedef struct {
	short packet_id;						/**< Número identificador do pacote. >*/
	short packet_total;						/**< Total de pacotes transmitidos. >*/
	short burst_size;						/**< Total de bytes transmitidos. >*/
	short send_rate;						/**< Taxa de transmissão. >*/
	short packet_size;						/**< Tamanho do pacotes. >*/
} packet_probe;

/**\struct received_probe
 * \brief Estrutura de dados armazenar dados do teste em andamento.
 */
typedef struct {
	int received_total;						/**< Total de bytes recebidos. >*/
	int received_packets;					/**< Total de pacotes recebidos. >*/
	timeval32 start;						/**< Instante de tempo do primeiro pacote recebido. >*/
	timeval32 end;							/**< Instante de tempo do último pacote recebido. >*/
	timeval32 report_time;					/**< Instante de tempo da próxima amostragem. >*/
	int report_interval;					/**< Intervalo entre amostras, em ms. >*/
	int resize_buffer;						/**< Tamanho do buffer de recepção. >*/
	int stop_recv;							/**< Flag para sinalização de fim de teste. >*/
#ifdef MBANDWIDTH
	int mreceived_total;					/**< Total de bytes recebidos na parte final do teste (1/2, 2/3...). >*/
	int mreceived_packets;					/**< Total de pacotes recebidos na parte final do teste (1/2, 2/3...).. >*/
	timeval32 mstart;						/**< Instante de tempo do primeiro pacote recebido (parte final do teste). >*/
#endif
#ifdef CALC_JITTER
	timeval32 tlast;						/**< Instante de tempo de chegada do pacote anterior. >*/
	double min_jitter;						/**< Jitter mínimo. >*/
	double max_jitter;						/**< Jitter máximo. >*/
	double med_jitter;						/**< Jitter médio. >*/
#endif
	char *log_file;							/**< Arquivo para log de execução do teste. >*/
} received_probe;

/**\struct data_test
 * \brief Estrutura de dados utilizada para  comunicação  entre  as  threads  de
 * recepção do teste, timeout e o controle do teste.
 */
typedef struct {
	settings *conf_settings;				/**< Configurações de teste.>*/
	resume *result;							/**< Resultados obtidos após fim do teste.>*/
	received_probe *probe;					/**< Estrutura de dados com informações do teste em execução.>*/
	void *handle;							/**< Ponteiro para handle do filtro (libpcap). >*/
} data_test;

#endif /* __DEFINES_H__ */
