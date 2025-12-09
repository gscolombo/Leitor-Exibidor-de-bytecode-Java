/**
 * @file printer.h
 * @brief Funções responsáveis por exibir o bytecode de métodos em formato legível.
 *
 * Este módulo contém utilitários para imprimir instruções JVM (opcodes)
 * de forma semelhante ao `javap -v`, incluindo:
 *  - decodificação de instruções,
 *  - identificação de operandos,
 *  - exibição de referências ao constant pool,
 *  - manipulação de instruções especiais (tableswitch, lookupswitch).
 *
 * A função principal, `show_opcodes`, é usada durante a exibição de métodos,
 * especialmente quando o atributo Code está presente.
 */

#ifndef PRINTER_H
#define PRINTER_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "uinteger.h"
#include "cp/writer.h"
#include "opcode_table.h"
#include "bytecode/utils.h"

/**
 * @brief Exibe cada instrução do bytecode de um método de forma detalhada.
 *
 * Esta função percorre o vetor de bytes `code` interpretando:
 *  - opcode da instrução (via `opcode_table`)
 *  - quantidade de operandos
 *  - operandos específicos da instrução (ex.: índices de CP, offsets)
 *  - instruções especiais como:
 *        * `tableswitch`
 *        * `lookupswitch`
 *
 * Para cada instrução, a função imprime:
 *  - o índice (PC – program counter)
 *  - o nome da instrução
 *  - valores decodificados (índices, deslocamentos, flags, etc.)
 *  - resoluções opcionais no constant pool (via `get_constant_UTF8_value`)
 *
 * @param code Ponteiro para o array de bytes contendo o bytecode.
 * @param length Comprimento total do bytecode (`code_length` do atributo Code).
 * @param cp Ponteiro para o constant pool, usado para resolver nomes/descritores.
 * @param cp_count Quantidade de entradas no constant pool.
 *
 * @note A função **não altera** o bytecode; apenas lê e imprime.
 * @note O comportamento é semelhante ao do `javap -v`, mas focado em depuração.
 */
void show_opcodes(const u1 *code, u4 length, const cp_info *cp, u2 cp_count);

#endif /* PRINTER_H */
