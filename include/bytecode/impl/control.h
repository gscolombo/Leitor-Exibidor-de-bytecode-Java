/**
 * @file control.h
 * @brief Declarações das instruções JVM relacionadas ao fluxo de controle.
 *
 * Este módulo implementa instruções responsáveis por alterar o fluxo de execução
 * dentro da JVM, como desvios condicionais e incondicionais, instruções de switch
 * e retorno de métodos. Elas afetam diretamente o contador de programa (PC) e a
 * pilha de frames da JVM.
 *
 * Instruções contempladas:
 *  - `goto` — desvio incondicional para um novo offset.
 *  - `tableswitch` — seleção de destino baseada em índice contínuo.
 *  - `lookupswitch` — seleção de destino baseada em pares chave→offset.
 *  - `return` — retorna de um método `void`.
 *  - `xreturn` — retorna valores específicos (`ireturn`, `freturn`, etc.), representados por `Treturn`.
 */

#ifndef BYTECODE_IMPL_CONTROL_H
#define BYTECODE_IMPL_CONTROL_H

#include "Frame.h"
#include "interpreter.h"
#include "bytecode/utils.h"

/**
 * @brief Implementa a instrução `goto`.
 *
 * Realiza um salto incondicional para um novo endereço de execução.
 * O offset é lido do bytecode e somado ao PC atual.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void _goto(Frame *);

/**
 * @brief Implementa a instrução `tableswitch`.
 *
 * Executa um salto baseado em um índice contínuo onde o destino é
 * determinado por uma tabela de offsets. A instrução alinha o PC,
 * lê o valor da pilha e usa-o como índice entre `low` e `high`.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void tableswitch(Frame *);

/**
 * @brief Implementa a instrução `lookupswitch`.
 *
 * Realiza salto baseado em tabela esparsa de pares chave→offset.
 * O valor no topo da pilha é comparado com cada chave até encontrar
 * um match; caso contrário, o salto padrão (`default`) é executado.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void lookupswitch(Frame *);

/**
 * @brief Implementa instruções de retorno que devolvem valores (ex.: `ireturn`, `freturn`).
 *
 * Remove o frame atual da pilha de execução e empilha o valor de retorno
 * no frame chamador, respeitando o tipo apropriado.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void Treturn(Frame *);

/**
 * @brief Implementa a instrução `return` (retorno sem valor).
 *
 * Encerra a execução do método atual removendo o frame da pilha sem
 * transferir valores para o chamador.
 *
 * @param frame Ponteiro para o frame atual da JVM.
 */
void _return(Frame *);

#endif
