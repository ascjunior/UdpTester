/**\file tmath.c
 * \brief Definição de métodos auxiliares para conversão de estruturas de  dados
 * e operações matemáticas.
 *  
 * \author Alberto Carvalho
 * \date August 22, 2013.
 */
 
 #include <defines.h>
 #include <tools/tmath.h>
 
 
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <time.h>
#include <string.h>
 
int compare_time(timeval32 *t1, timeval32 *t2) {
	if ((t2->tv_sec > t1->tv_sec) ||
		((t2->tv_sec == t1->tv_sec) && (t2->tv_usec >= t1->tv_usec)))
		return 1;
	else
		return 0;
}

 int double2timeval (double d, timeval32 *t) {
	int ret = -1;
	
	if (d > 0.0) {
		ret = 0;
		t->tv_sec = (int)d/1;
		t->tv_usec = (int)((d - t->tv_sec) * 1000000);
	}

	return ret;
}

int difftimeval(timeval32 *t1, timeval32 *t2, timeval32 *t3) {

	t3->tv_sec = t2->tv_sec - t1->tv_sec;
	t3->tv_usec = t2->tv_usec - t1->tv_usec;

	if (t3->tv_usec < 0) {
		t3->tv_usec += 1000000;
		t3->tv_sec -= 1;
		if (t3->tv_sec < 0) {	/* ??? -- BSD/OS does this */
			t3->tv_sec = 0;
			t3->tv_usec = 0;
		}
    }
    return 0; 
}

double difftimeval2db(timeval32 *t1, timeval32 *t2) {
	timeval32 time_diff;

	memset(&time_diff, 0, sizeof(struct timeval));
	time_diff.tv_sec = t2->tv_sec - t1->tv_sec;
	time_diff.tv_usec = t2->tv_usec - t1->tv_usec;

	if (time_diff.tv_usec < 0) {
		time_diff.tv_usec += 1000000;
		time_diff.tv_sec -= 1;
		if (time_diff.tv_sec < 0) {	/* ??? -- BSD/OS does this */
			time_diff.tv_sec = 0;
			time_diff.tv_usec = 0;
		}
    }
    return (double)(time_diff.tv_sec)+((double)(time_diff.tv_usec)/1000000.0); 
}

int difftimeval2us(timeval32 *t1, timeval32 *t2) {
	timeval32 time_diff;

	memset(&time_diff, 0, sizeof(struct timeval));
	time_diff.tv_sec = t2->tv_sec - t1->tv_sec;
	time_diff.tv_usec = t2->tv_usec - t1->tv_usec;

	if (time_diff.tv_usec < 0) {
		time_diff.tv_usec += 1000000;
		time_diff.tv_sec -= 1;
		if (time_diff.tv_sec < 0) {	/* ??? -- BSD/OS does this */
			time_diff.tv_sec = 0;
			time_diff.tv_usec = 0;
		}
    }
    return (int)((time_diff.tv_sec*1000000)+(time_diff.tv_usec)); 
}

int get_currentDateTime (char *buffer, int size) {
	time_t now = time (0);
	struct tm  tstruct;

	tstruct = *localtime (&now);
	strftime (buffer, size, "%Y-%m-%d.%X", &tstruct);

	return 0;
}

