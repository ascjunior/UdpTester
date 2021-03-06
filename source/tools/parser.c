/**\file parser.c
 * \brief Definição de métodos para parser de linha de comando no UdpTester.
 *  
 * \author Alberto Carvalho
 * \date June 26, 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <defines.h>
#include <tools/parser.h>
#include <tools/debug.h>

#include <netinet/in.h>
#include <arpa/inet.h>

/* TODO: 
 * 		- Implementar métodos corretos para parser da linha de comando.
 * 			- Suporte a parâmetros por extenso.
 * 
 * 
const option long_options[] = {
								{"bandwidth",			REQ_ARGUMENT,	NULL, 'b'},
								{"help",				NO_ARGUMENT,	NULL, 'h'},
								{"train_interval",		REQ_ARGUMENT,	NULL, 'i'},
								{"loop_test_interval",	REQ_ARGUMENT,	NULL, 'l'},
								{"train_num",      		REQ_ARGUMENT,	NULL, 'n'},
								{"port",     			REQ_ARGUMENT,	NULL, 'p'},
								{"report_interval",		REQ_ARGUMENT,	NULL, 'r'},
								{"packet_size",   		REQ_ARGUMENT,	NULL, 's'},
								{"train_size",   		REQ_ARGUMENT,	NULL, 't'},
								{"version",				NO_ARGUMENT,	NULL, 'v'},
								{"soc_buffer",     		REQ_ARGUMENT,	NULL, 'B'},
								{"test_interval",		REQ_ARGUMENT,	NULL, 'I'},
								{"burst_size",			REQ_ARGUMENT,	NULL, 'L'},
								{"receiver",  			NO_ARGUMENT,	NULL, 'R'},
								{"sender",     			REQ_ARGUMENT,	NULL, 'S'}, 
								{0, 0, 0, 0}};

const char short_options[] = "b:i:h:l:n:p:r:s:t:v:B:I:L:R:S:";
* 
* 
*/

/**\fn int read_config_file (char *argv, settings *conf_settings)
 * \brief Realiza o parser do arquivo de configurações de teste.
 * 
 *    O arquivo de configuração  de  testes  permite especificar  uma  lista  de
 * testes sequencias com diferentes perfis.
 *    O arquivo deve seguir o seguinte template:
 *    TIPO RATE <configs>
 *    Tipos de testes aceitos:
 *      0 - UDP continuo, com as seguintes configurações: SIZE(MB) TIME(ms) REPORT(ms)
 *      1 - UDP trem de pacotes, com as seguintes configurações: #TRAIN(int) SIZE(int) INTERVAL(ms) 
 * 
 * \param fname Path para o arquivo de configurações.
 * \param conf_settings Estrutura de dados com configurações de teste.
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int read_config_file (char *fname, settings *conf_settings);

void usage(void) {
	printf("\nUsage: ./UdpTester [-S, --sender |-R, --receiver server_addr] [options]"
		   "\nTry `./UdpTester --help' for more information.\n\n");
}

void help(void) {
	printf("\nUsage: ./UdpTester [-S, --sender |-R, --receiver server_addr] [options]"
		   "\n       ./UdpTester [-h, --help |-v, --version]\n");
	printf("\nSender/Receiver:"
		   "\n -B, --soc_buffer <size>                 Socket Buffer Size (min  %d, max %d, default %d Kbytes)"
		   "\n -r, --report_interval <time>            Report interval time  (min  %d, max %d, default %d seconds)"
		   "\n -d, --device <if_name>                  Iface Name to Monitoring (default %s)"
		   "\n -p, --port <port>                       Data Port number (default %d)\n",
		   MIN_SOC_BUFFER, MAX_SOC_BUFFER, DEFAULT_SOC_BUFFER,
		   MIN_REPORT_INTERVAL, MAX_REPORT_INTERVAL, DEFAULT_REPORT_INTERVAL,
		   DEFAULT_INTERFACE_NAME, DEFAULT_DATA_PORT);
	printf("\nSender:"
		   "\n -s, --packet_size <size>                Packet size (min %d, max %d, default %d bytes)"
		   "\n -t, --train_size <size>                 Train size by probe (min  %d, max %d, default %d packets)"
		   "\n -n, --train_num <size>                  Number of train probes (min %d, max %d, default %d probes)"
		   "\n -i, --train_interval <time>             Interval time between probes trains of packets (min  %d, max %d, default %d miliseconds)"
		   "\n -I, --test_interval <time>              Interval to continuo test (min  %d, max %d, default %d miliseconds)"
		   "\n -L, --length_test <size>                Burst size to test (min  %d, max %d, default %d MB)"
		   "\n -l, --loop_test_interval <time>         Interval between tests (min  %d, max %d, default %d minutes)"
		   "\n -b, --rate <value>                      Bandwidth (min  %d, max %d, default %d Mbps)\n\n"
		   "\n -f, --file <path>                       Test file configuration\n\n",
		   MIN_PACKET_SIZE, MAX_PACKET_SIZE, DEFAULT_PACKET_SIZE,
		   MIN_TRAIN_SIZE, MAX_TRAIN_SIZE, DEFAULT_TRAIN_SIZE,
		   MIN_TRAIN_NUM, MAX_TRAIN_NUM, DEFAULT_TRAIN_NUM,
		   MIN_TRAIN_INTERVAL, MAX_TRAIN_INTERVAL, DEFAULT_TRAIN_INTERVAL,
		   MIN_TEST_INTERVAL, MAX_TEST_INTERVAL, DEFAULT_TEST_INTERVAL,
		   MIN_BURST_SIZE, MAX_BURST_SIZE, DEFAULT_BURST_SIZE,
		   MIN_LOOP_TEST_INTERVAL, MAX_LOOP_TEST_INTERVAL, DEFAULT_LOOP_TEST_INTERVAL,
		   MIN_SEND_RATE, MAX_SEND_RATE, DEFAULT_SEND_RATE);
}

void init_settings (settings *conf_settings) {
	int i = 0;
	list_test *ag_test = &(conf_settings->ag_test);

	memset(conf_settings, 0, sizeof (settings));
	conf_settings->mode = RECEIVER;
	conf_settings->t_type = UDP_CONTINUO;
	conf_settings->udp_rate = DEFAULT_SEND_RATE;
	conf_settings->packet_size = DEFAULT_PACKET_SIZE;
	conf_settings->test.cont.size = DEFAULT_BURST_SIZE;
	conf_settings->test.cont.pkt_num = DEFAULT_TRAIN_SIZE;
	conf_settings->test.cont.interval = DEFAULT_TRAIN_INTERVAL;
	conf_settings->test.cont.report_interval = DEFAULT_REPORT_INTERVAL;
	conf_settings->loop_interval = DEFAULT_LOOP_TEST_INTERVAL;
	conf_settings->recvsock_buffer = DEFAULT_SOC_BUFFER;
	conf_settings->sendsock_buffer = DEFAULT_SOC_BUFFER;
	conf_settings->udp_port = DEFAULT_DATA_PORT;
	ag_test->size = 0;
	ag_test->next = 0;
	memset (ag_test->cfg_test, 0, (sizeof (config_test) * MAX_CONFIG_TEST));
	for (i = 0; i < MAX_CONFIG_TEST; i++)
		ag_test->cfg_test[i].t_type = UDP_INVALID;
	memset (conf_settings->iface, 0, STR_SIZE);
	memcpy (conf_settings->iface, DEFAULT_INTERFACE_NAME, strlen(DEFAULT_INTERFACE_NAME));
}

int parse_command_line (int argc, char **argv, settings *conf_settings) {
	int ret = 0;
	int i = 0;
	int config = 0;

	init_settings (conf_settings);

	if (argc > 1) {
		for (i = 1; i < argc; i++) {
			if (argv[i][0] != '-') {
				usage ();
				exit (1);
			}

			switch (argv[i][1]) {
				case 'b':
					i++;
					config = atoi(argv[i]);
					if ((config < MIN_SEND_RATE) ||
						 (config > MAX_SEND_RATE)) {
						return -1;
					}
					conf_settings->udp_rate = config;
					break;
				case 'd':
					i++;
					if (strlen(argv[i]) <= 0) {
						return -1;
					}
					memset (conf_settings->iface, 0, STR_SIZE);
					memcpy (conf_settings->iface, argv[i], strlen(argv[i]));
					break;
				case 'f':
					i++;
					if (strlen(argv[i]) <= 0) {
						return -1;
					}
					else {
						read_config_file (argv[i], conf_settings);
					}
					break;
				case 'i':
					i++;
					config = atoi(argv[i]);
					if ((config < MIN_TRAIN_INTERVAL) ||
						 (config > MAX_TRAIN_INTERVAL)) {
						return -1;
					}
					conf_settings->test.train.interval = config;
					break;
				case 'h':
					help();
					ret = 1;
					break;
				case 'l':
					i++;
					config = atoi(argv[i]);
					if ((config < MIN_LOOP_TEST_INTERVAL) ||
						 (config > MAX_LOOP_TEST_INTERVAL)) {
						return -1;
					}
					conf_settings->loop_interval = config;
					break;
				case 'n':
					i++;
					config = atoi(argv[i]);
					if ((config < MIN_TRAIN_NUM) ||
						 (config > MAX_TRAIN_NUM)) {
						return -1;
					}
					conf_settings->test.train.num = config;
					break;
				case 'p':
					i++;
					config = atoi(argv[i]);
					if ((config < 0) ||
						 (config > 65535)) {
						return -1;
					}
					conf_settings->udp_port = config;
					break;
				case 'r':
					i++;
					config = atoi(argv[i]);
					if ((config < MIN_REPORT_INTERVAL) ||
						 (config > MAX_REPORT_INTERVAL)) {
						return -1;
					}
					conf_settings->test.cont.report_interval = config;
					break;
				case 's':
					i++;
					config = atoi(argv[i]);
					if ((config < MIN_PACKET_SIZE) ||
						 (config > MAX_PACKET_SIZE)) {
						return -1;
					}
					conf_settings->packet_size = config;
					break;
				case 't':
					i++;
					config = atoi(argv[i]);
					if ((config < MIN_TRAIN_SIZE) ||
						 (config > MAX_TRAIN_SIZE)) {
						return -1;
					}
					conf_settings->test.train.size = config;
					break;
				case 'v':
					printf("\n%s %d.%d\n", STR_VERSION, VERSION_CODE_MAJOR, VERSION_CODE_MINOR);
					ret = 1;
					break;
				case 'I':
					i++;
					config = atoi(argv[i]);
					if ((config < MIN_TEST_INTERVAL) ||
						 (config > MAX_TEST_INTERVAL)) {
						return -1;
					}
					conf_settings->test.cont.interval = config;
					break;
				case 'L':
					i++;
					config = atoi(argv[i]);
					if ((config < MIN_BURST_SIZE) ||
						 (config > MAX_BURST_SIZE)) {
						return -1;
					}
					conf_settings->test.cont.size = config;
					break;
				case 'B':
					i++;
					config = atoi(argv[i]);
					if ((config < MIN_SOC_BUFFER) ||
						 (config > MAX_SOC_BUFFER)) {
						return -1;
					}
					if (conf_settings->mode == SENDER)
						conf_settings->recvsock_buffer = config;
					else
						conf_settings->sendsock_buffer = config;
					break;
				case 'S':
					conf_settings->mode = SENDER;
					break;
				case 'R':
					i++;
					struct sockaddr_in *server_addr_in = (struct sockaddr_in *)&(conf_settings->address);
					server_addr_in->sin_family = AF_INET;
					server_addr_in->sin_addr.s_addr = inet_addr(argv[i]);
					conf_settings->mode = RECEIVER;
					break;
				default:
					ret = -1;
					break;
			}
		}
	}
	else {
		ret = -1;
	}

	return ret;
}

int read_config_file (char *argv, settings *conf_settings) {
	int ret = 0, count = 0;
	int config[5];
	list_test *ag_test = &(conf_settings->ag_test);
	config_test *cfg_test = ag_test->cfg_test;
	FILE *fp;

	if ((argv == NULL) || (strlen (argv) <= 0))
		return -1;

	memset (&config, 0, (sizeof (int) * 5));
	fp = fopen (argv, "r");
	if (fp != NULL) {
		while (((fscanf (fp, "%d\t%d\t%d\t%d\t%d\n", &config[0],
				&config[1], &config[2], &config[3], &config[4])) == 5)
				&& (count < MAX_CONFIG_TEST)) {
			switch (config[0]) {
				case UDP_CONTINUO:
					cfg_test[count].test.cont.size = config[2];
					cfg_test[count].test.cont.interval = config[3];
					cfg_test[count].test.cont.report_interval = config[4];
					break;
				case UDP_PACKET_TRAIN:
					cfg_test[count].test.train.num = config[2];
					cfg_test[count].test.train.size = config[3];
					cfg_test[count].test.train.interval = config[4];				
					break;
				default:
					DEBUG_LEVEL_MSG (DEBUG_LEVEL_LOW, "Invalid test type %d\n", config[0]);
					return -1;
			}
			cfg_test[count].t_type = config[0];
			cfg_test[count].udp_rate = config[1];
			ag_test->size++;
			count++;
		}
		fclose (fp);
	}

	return ret;
}
