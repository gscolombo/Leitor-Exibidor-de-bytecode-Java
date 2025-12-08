/**
 * @file opcode_table.h
 * @brief Declaração da tabela principal de opcodes JVM usados pelo interpretador.
 *
 * A JVM identifica cada instrução por um opcode numérico (0x00 a 0xC9+).
 * Esta tabela mapeia cada opcode para:
 *   - seu mnemônico textual,
 *   - um ponteiro para a função que implementa sua execução.
 *
 * A estrutura usada é `OpcodeInfo`, definida em OpcodeInfo.h.
 *
 * O interpretador consulta esta tabela diretamente durante a execução, usando
 * o byte lido do bytecode como índice para encontrar a função correta.
 *
 * A tabela contém 202 entradas, cobrindo:
 *   - instruções clássicas do Java (aload, iload, iadd, return…)
 *   - instruções de controle de fluxo (goto, if_xxx, switch…)
 *   - instruções de criação e manipulação de objetos
 *   - instruções de método (invokevirtual, invokestatic, invokespecial…)
 *
 * A implementação real das instruções está localizada em opcode_functions.h / .c.
 */

#ifndef OPCODE_TABLE_H
#define OPCODE_TABLE_H

#include <stdlib.h>
#include "OpcodeInfo.h"
#include "opcode_functions.h"

/**
 * @brief Tabela global contendo metadados e funções executoras para cada opcode JVM.
 *
 * O índice da tabela corresponde ao valor numérico do opcode.  
 * Exemplo:
 *   - opcode_table[0x10] → informação da instrução `bipush`
 *   - opcode_table[0x60] → informação da instrução `iadd`
 *   - opcode_table[0xb6] → informação da instrução `invokevirtual`
 *
 * Cada entrada contém:
 *   - `mnemonic`: string do nome da instrução
 *   - `exec`: ponteiro para a função que implementa a instrução
 *
 * @note Nem todos os 202 opcodes possuem implementação completa dependendo do escopo do projeto,
 *       mas a tabela define espaço para todos.
 */
extern const OpcodeInfo opcode_table[202];

#endif /* OPCODE_TABLE_H */
