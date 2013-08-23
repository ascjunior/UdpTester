/**\file tmath.h
 * \brief Declaração de métodos auxiliares para conversão de estruturas de dados
 * e operações matemáticas.
 *  
 * \author Alberto Carvalho
 * \date August 22, 2013.
 */


#ifndef __TMATH_H__
#define __TMATH_H__


/**\fn int compare_time(struct timeval *t1, struct timeval *t2)
 * \brief Verifica se já transcorreu o tempo para report do teste. Compara  duas
 * struct timeval.
 * 
 * \param t1 Primeiro valor.
 * \param t2 Segundo valor.
 * \return 1 caso t2>t1, 0 caso contrário.
 */
int compare_time(timeval32 *t1, timeval32 *t2);

/**\fn int difftimeval (timeval32 *t1, timeval32 *t2, timeval32 *t3)
 * \brief Calcula a diferença entre os instantes de tempo, t1 e t2, e retorna  o
 * resultado em t3.
 * 
 * \param t1 Primeiro instante de tempo (input).
 * \param t2 Segundo instante de tempo (input).
 * \param t3 Resultado (output).
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int difftimeval (timeval32 *t1, timeval32 *t2, timeval32 *t3);

/**\fn double difftimeval2db (timeval32 *t1, timeval32 *t2)
 * \brief Calcula a diferença entre instantes de tempo e retorna o resultado.
 * 
 * \param t1 Primeiro instante de tempo.
 * \param t2 Segundo instante de tempo.
 * \return A diferença t2-t1 no formato double.
 */
double difftimeval2db (timeval32 *t1, timeval32 *t2);

/**\fn int difftimeval2us (timeval32 *t1, timeval32 *t2)
 * \brief Calcula a diferença entre instantes de tempo e retorna o resultado.
 * 
 * \param t1 Primeiro instante de tempo.
 * \param t2 Segundo instante de tempo.
 * \return A diferença t2-t1 no formato inteiro decimal em us.
 */
int difftimeval2us (timeval32 *t1, timeval32 *t2);

/**\fn int double2timeval (double d, timeval32 *t)
 * \brief Converte um valor em double para timeval.
 * 
 * \param d Valor de entrada em double (input).
 * \param t Resultado da conversão em timeval (output).
 * \return 0 em caso de sucesso, ~0 caso contrário.
 */
int double2timeval (double d, timeval32 *t);

#endif /* __TMATH_H__ */
