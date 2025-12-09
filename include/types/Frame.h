/**
 * @file Frame.h
 * @brief Estruturas que representam um *Frame* da JVM e sua pilha de operandos.
 *
 * Um Frame é criado a cada invocação de método e contém:
 * - contador de programa (pc)
 * - variáveis locais
 * - pilha de operandos
 * - ponteiro para o frame anterior (call stack)
 * - referência ao método e classe atuais
 *
 * O interpretador manipula Frames continuamente durante a execução do bytecode.
 */

#ifndef FRAME_H
#define FRAME_H

#include <stdbool.h>
#include "uinteger.h"
#include "constants.h"
#include "Classfile.h"
#include "MethodArea.h"
#include "java_type.h"

/**
 * @struct OperandStack
 * @brief Estrutura que representa a pilha de operandos do Frame.
 *
 * A pilha de operandos é usada para:
 * - operações aritméticas,
 * - passagem de parâmetros para métodos,
 * - instruções de carregamento/armazenamento,
 * - manipulação de valores de retorno.
 *
 * Cada entrada é um `dtype`, que pode representar valores primitivos ou referências.
 */
typedef struct
{
    /**
     * @brief Índice do topo da pilha.
     *
     * - `top == -1` significa pilha vazia.  
     * - Incrementado em push, decrementado em pop.
     */
    int16_t top;

    /**
     * @brief Capacidade total em número de elementos (slots).
     *
     * É definida pelo atributo `max_stack` do método.
     */
    u2 capacity;

    /**
     * @brief Vetor de elementos armazenados na pilha.
     *
     * O interpretador manipula esse vetor diretamente via push/pop.
     */
    dtype *stack;

} OperandStack;

/**
 * @struct Frame
 * @brief Estrutura que representa um frame de execução da JVM simplificada.
 *
 * Cada método invocado cria um novo Frame.  
 * Ele contém:
 * - contador de programa (`pc`)
 * - variáveis locais (`local_variables`)
 * - pilha de operandos (`operand_stack`)
 * - referência ao frame anterior (stack de chamadas)
 * - metadados sobre a classe e método atuais
 * - ponteiro para MethodArea para resolução dinâmica
 */
typedef struct Frame
{
    /**
     * @brief Program Counter — índice do próximo bytecode a ser executado.
     *
     * Avançado ou alterado por instruções de controle de fluxo (goto, if, tableswitch…).
     */
    u4 pc;

    /**
     * @brief Array de variáveis locais do método.
     *
     * Seu tamanho é igual a `max_locals` determinado pelo atributo `Code` do método.
     */
    dtype *local_variables;

    /**
     * @brief Pilha de operandos associada ao frame.
     *
     * Empilhada e desempilhada conforme as instruções JVM.
     */
    OperandStack operand_stack;

    /**
     * @brief Ponteiro para o frame anterior na call stack.
     *
     * - NULL caso este seja o frame inicial (ex.: método main).
     */
    struct Frame *previous_frame;

    /**
     * @brief Ponteiro para a estrutura Method executada neste frame.
     *
     * Contém:
     * - nome, descritor
     * - bytecode
     * - max_stack / max_locals
     */
    Method *method;

    /**
     * @brief Ponteiro para a classe dona do método.
     */
    Class *class;

    /**
     * @brief Nome da classe em forma de string (para atualizações de ponteiro).
     *
     * Utilizado em casos onde a referência à classe precisa ser atualizada dinamicamente.
     */
    const char *class_name;

    /**
     * @brief Ponteiro para a MethodArea usada para resolver classes, métodos e campos.
     */
    MethodArea *method_area;

} Frame;

#endif /* FRAME_H */
