/**\file debug.h
 * \brief Declaração de métodos para mensagens de debug UdpTester.
 *  
 * \author Alberto Carvalho
 * \date Aug 1, 2013.
 */
 
#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DEBUG_LEVEL_LOW			0
#define DEBUG_LEVEL_MED			3
#define DEBUG_LEVEL_HIGH		5

#ifndef DEBUG_LEVEL_DEFAULT
#define DEBUG_LEVEL_DEFAULT		DEBUG_LEVEL_HIGH
#endif

#ifdef DEBUG
	#define DEBUG_MSG(fmt, ...)				{ fprintf(stderr, "%s:%s:%d: "fmt, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__); }
#else
	#define DEBUG_MSG(fmt, ...)
#endif

#ifdef DEBUG_LEVEL
	#define DEBUG_LEVEL_MSG(lvl, fmt, ...)		{ if (lvl >= DEBUG_LEVEL_DEFAULT) \
													fprintf(stderr, "%s:%s:%d: "fmt, __FILE__, __FUNCTION__, __LINE__, ## __VA_ARGS__); }
#else
	#define DEBUG_LEVEL_MSG(lvl, fmt, ...)
#endif

#define LOG_FILE(fname, fmt, ...)				{ FILE *fp = fopen (fname, "a"); \
												  if ( fp != NULL ) { \
													fprintf (fp, fmt, ## __VA_ARGS__); \
													fflush (fp); \
													fclose (fp); \
												  } \
												}

#endif /* __DEBUG_H__ */
