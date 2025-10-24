#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "uinteger.h"
#include "attribute_enum.h"
#include "member.h"

/** @file
 * @brief Declaração de funções utilitárias para o restante do projeto.
 */

/**
 * @brief Troca os dois bytes de um inteiro de 16 bits.
 *
 * Esta função recebe um inteiro de 16 bits e troca seus _bytes_ inferior e superior.
 * 
 * A implementação se baseia em operações _bitwise_. Especificamente, os operadores `<<` e `>>` são 
 * usados para mover os pares de dígitos hexadecimais para as posições desejadas. O operador `|` é 
 * utilizado para "concatenar" os pares em um único número. \n
 * Por exemplo: \n
 * ```python
 *  n = 0x1234
 *  a = (n << 8) = (0x1234 << 8) = 0x3400
 *  b = (n >> 8) = (0x1234 >> 8) = 0x0012
 *  a | b = 0x3400 | 0x0012 = 0x3412
 * ```
 *
 * @param n O inteiro de 16 bits para trocar os bytes.
 * @return O inteiro de 16 bits com os bytes trocados.
 */
unsigned int u2swap(unsigned int);

/**
 * @brief Inverte a ordem dos bytes de um inteiro de 32 bits.
 *
 * Esta função reverte a ordem dos bytes em um inteiro de 32 bits.
 * A implementação se baseia em operações _bitwise_. Especificamente, os operadores `<<` e `>>` são 
 * usados em conjunto com o operador `&` para mover os pares de dígitos hexadecimais para as posições desejadas. 
 * O operador `|` é utilizado para "concatenar" os pares em um único número. \n
 * Por exemplo: \n
 * ```python
 *  n = 0x12345678
 *  a = n >> 24 = 0x00000012
 *  b = (n >> 8) & 0xff00 = 0x00123456 & 0x0000ff00 = 0x00003400
 *  c = (n << 8) & 0xff0000 = 0x34567800 & 0x00ff0000 = 0x00560000
 *  a = n << 24 = 0x78000000
 *  a | b | c | d = 0x00000012 | 0x00003400 | 0x00560000 | 0x78000000 = 0x78563412
 * ```
 * @param n O inteiro de 32 bits para inverter.
 * @return O inteiro de 32 bits com os bytes invertidos.
 */
unsigned int u4swap(unsigned int);

/**
 * @brief Retorna o número de dígitos de um inteiro não negativo.
 *
 * Essa função conta a quantidade de vezes que um inteiro não negativo pode ser dividido
 * por 10 com o valor resultante maior ou igual a 10. \n
 * A contagem final corresponde ao número de dígitos do inteiro.
 * 
 * @param n Um inteiro não negativo.
 * @return O número de dígitos de `n`.
 */
unsigned int num_digits(unsigned int);

/**
 * @brief Aplica um mapeamento de flags para o nome correspondente e concatena
 * cada nome em uma string, com um separador opcional.
 * 
 * A partir de um dado mapeamento entre _flags_ e o nome correspondente, essa função
 * concatena os nomes das _flags_ em uma única _string_, separadas por uma dada _string_. \n
 * Assume-se que o mapeamento corresponda a uma função bijetora. Logo, a função é baseada em
 * uma única travessia do mapa de _flags_, verificando a presença de cada _flag_ nas
 * _flags_ passadas para a função. Caso esteja presente, o nome da _flag_ é inserido
 * na _string_ retornada.
 * 
 * @param flags String de 2 bytes ou 16 bits com "1" representando a presença de uma flag.
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
 * Essa função retorna um valor enumerado (`attribute_name`) a partir do nome de um atributo, como
 * definido pela especificação da JVM 8.
 * 
 * @param name Nome do atributo.
 * @return Valor enumerado correspondente ao nome do atributo.
 */
const attribute_name *convert_attr_name(const char *);

/**
 * @brief Converte um descritor na sua versão intelígivel (original),
 * de acordo com a especificação da JVM 8.
 * 
 * Essa função avalia um descritor com base na especificação da JVM 8 e monta uma _string_
 * com a versão intelígivel/original do descritor. \n
 * Um separador pode ser passado para ser inserido entre cada _token_ do descritor.
 * 
 * @param descriptor String do descritor de membro (campo ou método).
 * @param sep String a ser utilizada como separador entre cada símbolo do descritor.
 * @return String com a versão intelígivel do descritor.
 */
char *parse_descriptor(const char *, char *);

#endif