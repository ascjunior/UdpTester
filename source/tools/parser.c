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
								{"receiver",  			NO_ARGUMENT,	NULL, 'R'},
								{"sender",     			REQ_ARGUMENT,	NULL, 'S'}, 
								{0, 0, 0, 0}};

const char short_options[] = "b:i:h:l:n:p:r:s:t:v:I:B:R:S:";
* 
* 
*/

void usage(void) {
	printf("\nUsage: ./UdpTester [--server recv_addr|-receiver] [options]"
		   "\nTry `./UdpTester --help' for more information.\n\n");
}

void help(void) {
	printf("\nUsage: ./UdpTester [-S, --sender recv_addr|-R, --receiver] [options]"
		   "\n       ./UdpTester [-h, --help |-v, --version]\n");
	printf("\nSender/Receiver:"
		   "\n -B, --soc_buffer <size>                 Socket Buffer Size (min  %d, max %d, default %d Kbytes)"
		   "\n -r, --report_interval <time>            Report interval time  (min  %d, max %d, default %d seconds)"
		   "\n -p, --port <port>                       Data Port number (default %d)\n",
		   MIN_SOC_BUFFER, MAX_SOC_BUFFER, DEFAULT_SOC_BUFFER,
		   MIN_REPORT_INTERVAL, MAX_REPORT_INTERVAL, DEFAULT_REPORT_INTERVAL,
		   DEFAULT_DATA_PORT);
	printf("\nSender:"
		   "\n -s, --packet_size <size>                Packet size (min %d, max %d, default %d bytes)"
		   "\n -t, --train_size <size>                 Train size by probe (min  %d, max %d, default %d packets)"
		   "\n -n, --train_num <size>                  Number of train probes (min %d, max %d, default %d probes)"
		   "\n -i, --train_interval <time>             Interval time between probes trains of packets (min  %d, max %d, default %d miliseconds)"
		   "\n -I, --test_interval <time>              Interval to continuo test (min  %d, max %d, default %d seconds)"
		   "\n -l, --loop_test_interval <time>         Interval between tests (min  %d, max %d, default %d minutes)"
		   "\n -b, --rate <value>                      Bandwidth (min  %d, max %d, default %d Mbps)\n\n",
		   MIN_PACKET_SIZE, MAX_PACKET_SIZE, DEFAULT_PACKET_SIZE,
		   MIN_TRAIN_SIZE, MAX_TRAIN_SIZE, DEFAULT_TRAIN_SIZE,
		   MIN_TRAIN_NUM, MAX_TRAIN_NUM, DEFAULT_TRAIN_NUM,
		   MIN_TRAIN_INTERVAL, MAX_TRAIN_INTERVAL, DEFAULT_TRAIN_INTERVAL,
		   MIN_TEST_INTERVAL, MAX_TEST_INTERVAL, DEFAULT_TEST_INTERVAL,
		   MIN_LOOP_TEST_INTERVAL, MAX_LOOP_TEST_INTERVAL, DEFAULT_LOOP_TEST_INTERVAL,
		   MIN_SEND_RATE, MAX_SEND_RATE, DEFAULT_SEND_RATE);
}

void init_settings (settings *conf_settings) {

	memset(conf_settings, 0, sizeof (settings));
	conf_settings->mode = RECEIVER;
	conf_settings->bandwidth = DEFAULT_SEND_RATE;
	conf_settings->packet_size = DEFAULT_PACKET_SIZE ;
	conf_settings->train_num = DEFAULT_TRAIN_NUM;
	conf_settings->train_size = DEFAULT_TRAIN_SIZE;
	conf_settings->train_interval = DEFAULT_TRAIN_INTERVAL;
	conf_settings->loop_test_interval = DEFAULT_LOOP_TEST_INTERVAL;
	conf_settings->report_interval = DEFAULT_REPORT_INTERVAL;
	conf_settings->test_interval = DEFAULT_TEST_INTERVAL;
	conf_settings->soc_buffer = DEFAULT_SOC_BUFFER;
	conf_settings->port = DEFAULT_CONTROL_PORT;
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
					conf_settings->bandwidth = config;
					break;
				case 'i':
					i++;
					config = atoi(argv[i]);
					if ((config < MIN_TRAIN_INTERVAL) ||
						 (config > MAX_TRAIN_INTERVAL)) {
						return -1;
					}
					conf_settings->train_interval = config;
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
					conf_settings->loop_test_interval = config;
					break;
				case 'n':
					i++;
					config = atoi(argv[i]);
					if ((config < MIN_TRAIN_NUM) ||
						 (config > MAX_TRAIN_NUM)) {
						return -1;
					}
					conf_settings->train_num = config;
					break;
				case 'p':
					i++;
					config = atoi(argv[i]);
					if ((config < 0) ||
						 (config > 65535)) {
						return -1;
					}
					conf_settings->port = config;
					break;
				case 'r':
					i++;
					config = atoi(argv[i]);
					if ((config < MIN_REPORT_INTERVAL) ||
						 (config > MAX_REPORT_INTERVAL)) {
						return -1;
					}
					conf_settings->report_interval = config;
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
					conf_settings->train_size = config;
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
					conf_settings->test_interval = config;
					break;
				case 'B':
					i++;
					config = atoi(argv[i]);
					if ((config < MIN_SOC_BUFFER) ||
						 (config > MAX_SOC_BUFFER)) {
						return -1;
					}
					conf_settings->soc_buffer = config;
					break;
				case 'R':
					conf_settings->mode = RECEIVER;
					break;
				case 'S':
					i++;
					struct sockaddr_in *sender_addr_in = (struct sockaddr_in *)&(conf_settings->address);
					sender_addr_in->sin_family = AF_INET;
					sender_addr_in->sin_addr.s_addr = inet_addr(argv[i]);
					conf_settings->mode = SENDER;
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