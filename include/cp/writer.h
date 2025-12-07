/**
 * @file cp/writer.h
 * @brief Funções para exibição do Constant Pool de um ClassFile.
 *
 * Este módulo fornece rotinas para imprimir em formato legível
 * cada entrada do constant pool, interpretando:
 *  - literais numéricos
 *  - referências a classes, métodos e campos
 *  - strings UTF-8 decodificadas
 *  - NameAndType
 *  - MethodHandle / MethodType / InvokeDynamic
 *
 * É usado internamente por show_classfile() em writer.c.
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
 * @brief Exibe todas as entradas do constant pool.
 *
 * Percorre o array de `cp_info` e imprime cada entrada no formato:
 *
 * ```
 * #<index> = <TagName> <Detalhes>
 * ```
 *
 * Exemplos:
 * ```
 * #1 = Utf8 "java/lang/Object"
 * #2 = Class #1
 * #3 = Methodref #2.#5   // java/lang/Object.<init>()V
 * ```
 *
 * Observações:
 * - A função apenas exibe, não altera nem libera o constant pool.
 * - As strings UTF-8 já devem estar decodificadas (campo `UTF8.str`).
 *
 * @param count Número total de entradas no constant pool (u2).
 * @param cp Ponteiro para o array de entradas `cp_info`.
 */
void show_constants(u2 count, cp_info *cp);

#endif /* CP_WRITER_H */
