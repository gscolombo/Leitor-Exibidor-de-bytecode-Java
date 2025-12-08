/**
 * @file writer.h
 * @brief Funções para exibição e manipulação do Constant Pool de um arquivo `.class`.
 *
 * Este módulo contém utilitários para:
 *   - imprimir cada entrada do constant pool em formato legível,
 *   - recuperar valores decodificados de constantes UTF8.
 *
 * A exibição segue estilo semelhante ao comando `javap -v`, permitindo análise
 * detalhada de cada item armazenado no constant pool.
 */

#ifndef CP_WRITER_H
#define CP_WRITER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "constants.h"
#include "uinteger.h"
#include "utils.h"
#include "wchar.h"

/**
 * @brief Exibe todas as entradas do pool de constantes.
 *
 * Para cada entrada do vetor `cp`, esta função:
 *   - lê sua tag (`CONSTANT_Class`, `CONSTANT_Utf8`, `CONSTANT_Methodref`, etc.);
 *   - interpreta internamente sua estrutura correspondente (via `cp_info.info`);
 *   - formata a saída compatível com o estilo do `javap -v`;
 *   - apresenta valores numéricos, nomes decodificados, referências e vínculos.
 *
 * Itens exibidos incluem:
 *   - nomes de classes e interfaces,
 *   - constantes numéricas (int, float, long, double),
 *   - strings,
 *   - campos, métodos e referências NameAndType,
 *   - tipos de método,
 *   - itens invokedynamic e methodhandles.
 *
 * @param count Quantidade total de entradas no constant pool.
 * @param _cp   Ponteiro para o vetor de `cp_info` contendo as entradas do pool.
 *
 * @note O índice 0 do constant pool é inválido; iterações normalmente começam em 1.
 */
void show_constants(u2 count, cp_info *_cp);

/**
 * @brief Recupera uma string UTF-8 de uma constante `CONSTANT_UTF8` no constant pool.
 *
 * Esta função facilita a extração de textos armazenados em constantes UTF8,
 * retornando a string decodificada para uso externo.
 *
 * @param index Índice da constante no constant pool (deve apontar para uma CONSTANT_Utf8).
 * @param cp    Ponteiro para o vetor do constant pool.
 * @return Ponteiro para a string UTF-8 decodificada.
 *
 * @warning A string retornada pertence ao próprio constant pool.  
 *          **Não deve ser liberada manualmente.**
 */
char *get_constant_UTF8_value(u2 index, const cp_info *cp);

#endif /* CP_WRITER_H */
