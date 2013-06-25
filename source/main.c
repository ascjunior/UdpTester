/**\file main.c
 * \brief Inicialização do UdpTester.
 * 
 * \author Alberto Carvalho
 * \date June 25, 2013
 */
 
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <defines.h>

/**\fn void usage(void) 
 * \brief Help básico para uso do UdpTester.
 */
void usage(void) {
	printf("\nUsage: ./UdpTester [--server recv_addr|-receiver] [options]"
		   "\nTry `./UdpTester --help' for more information.\n\n");
}

/**\fn void help(void) 
 * \brief Help completo para uso do UdpTester.
 */
void help(void) {
	printf("\nUsage: ./UdpTester [--sender recv_addr|--receiver] [options]"
		   "\n       ./UdpTester [--help |--version]\n");
	printf("\nSender/Receiver:"
		   "\n --soc_buffer <size>                 Socket Buffer Size (min  %d, max %d, default %d Kbytes)"
		   "\n --report_interval <time>            Report interval time  (min  %d, max %d, default %d seconds)"
		   "\n --port <port>                       Control Port number (default %d)\n",
		   MIN_SOC_BUFFER, MAX_SOC_BUFFER, DEFAULT_SOC_BUFFER,
		   MIN_REPORT_INTERVAL, MAX_REPORT_INTERVAL, DEFAULT_REPORT_INTERVAL,
		   DEFAULT_CONTROL_PORT);
	printf("\nSender:"
		   "\n --packet_size <size>                Packet size (min %d, max %d, default %d bytes)"
		   "\n --train_size <size>                 Train size by probe (min  %d, max %d, default %d packets)"
		   "\n --train_num <size>                  Number of train probes (min %d, max %d, default %d probes)"
		   "\n --train_interval <time>             Interval time between probes trains of packets (min  %d, max %d, default %d miliseconds)"
		   "\n --test_interval <time>              Interval to continuo test (min  %d, max %d, default %d seconds)"
		   "\n --loop_test_interval <time>         Interval between tests (min  %d, max %d, default %d minutes)"
		   "\n --rate <value>                      Bandwidth (min  %d, max %d, default %d Mbps)\n\n",
		   MIN_PACKET_SIZE, MAX_PACKET_SIZE, DEFAULT_PACKET_SIZE,
		   MIN_TRAIN_SIZE, MAX_TRAIN_SIZE, DEFAULT_TRAIN_SIZE,
		   MIN_TRAIN_NUM, MAX_TRAIN_NUM, DEFAULT_TRAIN_NUM,
		   MIN_TRAIN_INTERVAL, MAX_TRAIN_INTERVAL, DEFAULT_TRAIN_INTERVAL,
		   MIN_TEST_INTERVAL, MAX_TEST_INTERVAL, DEFAULT_TEST_INTERVAL,
		   MIN_LOOP_TEST_INTERVAL, MAX_LOOP_TEST_INTERVAL, DEFAULT_LOOP_TEST_INTERVAL,
		   MIN_SEND_RATE, MAX_SEND_RATE, DEFAULT_SEND_RATE);
}

/**\fn int  main( int argc, char **argv )
 * \brief Inicialização da Buscard.
 * 
 * \param argc Número de argumentos da linha de comando.
 * \param argv Lista de argumentos da linha de comando.
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int main (int argc, char **argv) {

	if (argc <= 1)
		usage();
	else
		help();
	
	return 0;
}
