#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "uinteger.h"
#include "attribute_enum.h"
#include "member.h"

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

/**
 * @brief Aplica um mapeamento de flags para o nome correspondente e concatena
 * cada nome em uma string, com um separador opcional.
 * 
 * @param flags String de 2 bytes ou 16 bits com "1" representando a ativação de uma flag.
 * @param n Número de nomes distintos de flags.
 * @param sep
 * @parblock
 * String representando o separador entre cada nome de flag. 
 * Caso seja passado um ponteiro nulo, cada nome será separado
 * por um espaço.
 * @endparblock
 * @param flag_map Mapa entre flags no formato hexadecimal e respectivos nomes.
 * @return String com nomes de flags separadas por `sep`.
 */
char *parse_flags(u2, size_t, const char *, const FlagMap[]);

/**
 * @brief Converte o nome de um atributo no seu respectivo valor enumerado.
 * 
 * @param name Nome do atributo.
 * @return Valor enumerado correspondente ao nome do atributo.
 */
const attribute_name *convert_attr_name(const wchar_t *);

/**
 * @brief Converte um descritor na sua versão intelígivel (original),
 * de acordo com a especificação da JVM 8.
 * 
 * @param descriptor String do descritor de membro (campo ou método).
 * @param sep String a ser utilizada como separador entre cada símbolo do descritor.
 * @return String com a versão intelígivel do descritor.
 */
wchar_t *parse_descriptor(const wchar_t *, wchar_t *);

#endif