/**
 * @file opcode_functions.h
 * @brief Agrupa todos os módulos que implementam as instruções (opcodes) da JVM.
 *
 * Este arquivo não define funções diretamente, mas reúne *todos* os cabeçalhos
 * responsáveis pela implementação das instruções da JVM, divididas por categoria:
 *
 *  - constantes (`ldc`, `bipush`, `sipush`…)
 *  - loads (`iload`, `aload`, variações indexadas)
 *  - stores (`istore`, `astore`…)
 *  - operações aritméticas (`iadd`, `_math.h`)
 *  - conversões (`i2f`, `d2i`…)
 *  - comparações (`if_icmpeq`, `lcmp`…)
 *  - controle de fluxo (`goto`, `tableswitch`, `lookupswitch`)
 *  - referência / objeto (`new`, `getfield`, `putstatic`, `invokestatic`…)
 *  - instruções estendidas (`wide`, `multianewarray`)
 *
 * O interpretador faz uso deste agrupamento por meio da `opcode_table.h`,
 * que associa cada opcode a uma função executora definida em um dos headers incluídos abaixo.
 */

#ifndef OPCODE_FUNCTIONS_H
#define OPCODE_FUNCTIONS_H

#include "bytecode/impl/constants.h"     /**< Instruções de carregamento de constantes. */
#include "bytecode/impl/loads.h"         /**< Instruções de carregamento (load). */
#include "bytecode/impl/comparisons.h"   /**< Instruções de comparação e branching condicional. */
#include "bytecode/impl/control.h"       /**< Instruções de controle de fluxo (goto, switch…). */
#include "bytecode/impl/conversions.h"   /**< Instruções de conversão entre tipos primitivos. */
#include "bytecode/impl/_math.h"         /**< Instruções aritméticas e lógicas. */
#include "bytecode/impl/references.h"    /**< Instruções relacionadas a objetos, campos, métodos e arrays. */
#include "bytecode/impl/stack.h"         /**< Instruções de manipulação da pilha (dup, pop…). */
#include "bytecode/impl/stores.h"        /**< Instruções de armazenamento (store). */
#include "bytecode/impl/extended.h"      /**< Instruções estendidas da JVM (wide, multianewarray). */

#endif /* OPCODE_FUNCTIONS_H */
