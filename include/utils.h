#ifndef UTILS_H_
#define UTILS_H_

/**
 * @brief Troca os dois bytes de um inteiro de 16 bits.
 *
 * Esta função recebe um inteiro de 16 bits e troca seus bytes inferior e superior.
 * Por exemplo, se a entrada for 0x1234, a saída será 0x3412.
 *
 * @param n O inteiro de 16 bits para trocar os bytes.
 * @return O inteiro de 16 bits com os bytes trocados.
 */
unsigned int u2swap(unsigned int);

/**
 * @brief Inverte a ordem dos bytes de um inteiro de 32 bits.
 *
 * Esta função reverte a ordem dos bytes em um inteiro de 32 bits.
 * Por exemplo, dado uma entrada de 0x12345678, a saída será 0x78563412.
 *
 * @param n O inteiro de 32 bits para inverter.
 * @return O inteiro de 32 bits com os bytes invertidos.
 */
unsigned int u4swap(unsigned int);

/**
 * @brief Retorna o número de dígitos de um inteiro não negativo.
 * 
 * @param n Um inteiro não negativo.
 * @return O número de dígitos de `n`.
 */
unsigned int num_digits(unsigned int);

#endif