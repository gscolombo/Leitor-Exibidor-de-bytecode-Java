/**
 * @file utils.h
 * @brief Funções auxiliares para manipulação de inteiros, flags, nomes de atributos
 *        e descritores utilizados no processamento de arquivos .class.
 *
 * Este header reúne utilidades usadas em vários módulos do leitor-exibidor de bytecode:
 *  - conversão de endianness (u2swap, u4swap)
 *  - contagem de dígitos (num_digits)
 *  - conversão de flags de acesso em texto (parse_flags)
 *  - conversão de nomes de atributos do constant pool para enums (convert_attr_name)
 *  - análise de descritores de tipos de campos e métodos da JVM (parse_descriptor)
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "uinteger.h"
#include "attributes.h"
#include "member.h"

/**
 * @brief Troca os dois bytes de um inteiro de 16 bits.
 *
 * Exemplo:
 * - Entrada: 0x1234  
 * - Saída:   0x3412  
 *
 * Utilizado quando a máquina é little-endian e o arquivo JVM exige leitura em big-endian.
 *
 * @param n Valor de 16 bits cujo endianness será invertido.
 * @return u2 Valor com bytes trocados.
 */
unsigned int u2swap(unsigned int);

/**
 * @brief Inverte a ordem dos bytes de um inteiro de 32 bits.
 *
 * Exemplo:
 * - Entrada: 0x12345678  
 * - Saída:   0x78563412  
 *
 * Necessário para interpretar corretamente valores big-endian da JVM.
 *
 * @param n Valor de 32 bits a ser convertido.
 * @return u4 Valor com bytes invertidos.
 */
unsigned int u4swap(unsigned int);

/**
 * @brief Calcula o número de dígitos necessários para representar um inteiro positivo.
 *
 * Exemplo:
 * - `num_digits(7)`  → 1  
 * - `num_digits(42)` → 2  
 * - `num_digits(999)`→ 3  
 *
 * Útil para formatação e exibição de valores numéricos.
 *
 * @param n Inteiro não negativo.
 * @return Número de dígitos de `n`.
 */
unsigned int num_digits(unsigned int);


/**
 * @brief Converte um conjunto de flags (u2) em uma string formatada.
 *
 * Flags de acesso da JVM (como ACC_PUBLIC, ACC_STATIC, ACC_FINAL) são representadas
 * em bits dentro de um u2. Esta função converte esses bits em um texto legível,
 * utilizando um mapa fornecido pelo chamador.
 *
 * Exemplo do formato retornado:
 * ```
 * "public static final"
 * ```
 *
 * @param flags Valor u2 contendo bits correspondentes a flags da JVM.
 * @param count Quantidade de entradas no array `map`.
 * @param separator String usada para separar flags no texto final.
 * @param map Array de mapeamento entre bits e nomes (FlagMap[], definido em member.h).
 * @return char* String alocada dinamicamente contendo as flags convertidas.  
 *         Deve ser liberada com free().
 */
char *parse_flags(u2 flags, size_t count, const char *separator, const FlagMap map[]);


/**
 * @brief Converte um nome de atributo (em UTF-8 wide) para o enum attribute_name.
 *
 * Esta função recebe o nome de um atributo lido do constant pool e retorna
 * um ponteiro para o enum correspondente — por exemplo:
 *
 * | Nome no .class | Enum retornado         |
 * |----------------|-------------------------|
 * | "Code"         | attribute_name::Code    |
 * | "LineNumberTable" | attribute_name::LineNumberTable |
 * | "ConstantValue"| attribute_name::ConstantValue |
 *
 * Usado em reader.c para decidir como ler o conteúdo do atributo.
 *
 * @param name Nome do atributo como wide-string (wchar_t*).
 * @return Ponteiro para um valor de enum attribute_name, ou NULL se o nome não for reconhecido.
 */
const attribute_name *convert_attr_name(const wchar_t *name);


/**
 * @brief Analisa um descritor JVM e retorna sua representação legível.
 *
 * Um descritor JVM pode representar:
 * - tipos primitivos: `I`, `F`, `J`, `D`, `Z`...
 * - objetos: `Ljava/lang/String;`
 * - arrays: `[I`, `[[Ljava/lang/Object;`
 * - descritores de métodos: `(ILjava/lang/String;)V`
 *
 * Esta função interpreta o descritor e escreve o resultado em forma textual.
 *
 * Exemplos:
 * - Entrada: `"I"` → Saída: `"int"`
 * - Entrada: `"[I"` → `"int[]"`
 * - Entrada: `"(I)V"` → `"void method(int)"`
 *
 * @param desc Descritor de tipo da JVM (wide-string).
 * @param out Buffer wide-string onde o resultado será armazenado.
 *            Deve ser previamente alocado pelo chamador.
 * @return wchar_t* Ponteiro para `out`, contendo o texto convertido.
 */
wchar_t *parse_descriptor(const wchar_t *desc, wchar_t *out);

#endif /* UTILS_H_ */
