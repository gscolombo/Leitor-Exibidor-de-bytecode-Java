/**
 * @file OpcodeInfo.h
 * @brief Estruturas auxiliares para mapeamento de opcodes JVM a suas operações.
 *
 * A JVM utiliza códigos numéricos (opcodes) para identificar instruções
 * como `iload`, `invokevirtual`, `return`, etc.  
 * Este arquivo define a estrutura usada para armazenar:
 *   - o *mnemonic* textual da instrução,
 *   - o ponteiro para a função responsável por executar essa instrução.
 *
 * Essa estrutura é utilizada por `opcode_table.h` e pelo interpretador.
 */

#ifndef OPCODE_INFO_H
#define OPCODE_INFO_H

#include "uinteger.h"
#include "Frame.h"

/**
 * @struct OpcodeInfo
 * @brief Metadados associados a um opcode JVM.
 *
 * Cada opcode possui:
 *  - um *mnemonic* textual (ex.: `"iload"`, `"iadd"`, `"return"`)
 *  - uma função executora que implementa seu comportamento no interpretador
 *
 * Este struct é organizado em tabelas (arrays) indexadas pelo valor numérico
 * do opcode, permitindo consulta direta e eficiente durante a execução.
 */
typedef struct
{
    /**
     * @brief Nome textual da instrução (mnemônico).
     *
     * Exemplos:
     *  - `"iconst_0"`
     *  - `"invokevirtual"`
     *  - `"goto"`
     */
    const char *mnemonic;

    /**
     * @brief Ponteiro para a função que executa a instrução.
     *
     * A função recebe o `Frame *` atual, modifica:
     *   - PC (program counter)
     *   - operand stack
     *   - local variables
     *   - ou realiza chamadas de método
     *
     * Cada executor implementa exatamente a semântica especificada pela JVM.
     */
    void (*exec)(Frame *);
} OpcodeInfo;

#endif /* OPCODE_INFO_H */
