/**
 * @file interpreter.h
 * @brief Declarações da API do interpretador de bytecode.
 *
 * Este cabeçalho declara as funções principais usadas pelo interpretador
 * para invocar métodos, executar frames, manipular a pilha de operandos
 * e inicializar variáveis locais de acordo com tipos da JVM.
 *
 * As estruturas usadas (Class, Method, Frame, MethodArea, dtype, type_enum)
 * são definidas em outros módulos do projeto.
 */

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "opcode_table.h"

/**
 * @brief Invoca um método em contexto de execução.
 *
 * Prepara um novo \c Frame (se necessário), define variáveis locais iniciais,
 * empilha o frame na pilha de execução e inicia a execução do método passado.
 *
 * @param cls Ponteiro para a \c Class que contém o método.
 * @param mtd Ponteiro para a estrutura \c Method a ser invocada.
 * @param local_vars Vetor pré-alocado de \c dtype representando as variáveis locais iniciais (pode ser NULL).
 * @param caller_frame Ponteiro para o \c Frame do chamador (pode ser NULL se invocação for de nível root).
 * @param method_area Ponteiro para a \c MethodArea (usada para resolução de classes, métodos e campos).
 */
void invoke_method(Class *cls, Method *mtd, dtype *local_vars, Frame *caller_frame, MethodArea *method_area);

/**
 * @brief Executa o bytecode contido no frame atual.
 *
 * Esta função lê instruções a partir do programa counter do \c Frame, busca
 * handlers na \c opcode_table e realiza a semântica do bytecode até que o
 * método retorne ou ocorra exceção/report de erro.
 *
 * @param frame Ponteiro para o \c Frame que contém o método/bytecode a ser executado.
 */
void execute_method(Frame *frame);

/**
 * @brief Empilha um valor na pilha de operandos do frame.
 *
 * Adiciona \c value ao topo da pilha de operandos do \c Frame, atualizando
 * o índice/topo conforme a implementação da pilha.
 *
 * @param frame Ponteiro para o \c Frame alvo.
 * @param value Valor do tipo \c dtype a ser empilhado.
 */
void push_operand(Frame *frame, dtype value);

/**
 * @brief Desempilha e retorna um valor da pilha de operandos do frame.
 *
 * Remove o elemento do topo da pilha de operandos do \c Frame e o retorna.
 * O comportamento é indefinido se a pilha estiver vazia.
 *
 * @param frame Ponteiro para o \c Frame alvo.
 * @return Valor do tipo \c dtype que estava no topo da pilha.
 */
dtype pop_operand(Frame *frame);

/**
 * @brief Inicializa uma variável local de acordo com o tipo JVM informado.
 *
 * Cria e retorna um valor \c dtype inicializado com o valor padrão para o tipo
 * especificado por \c type (por exemplo, 0 para inteiros, 0.0 para floats,
 * NULL/zero para referências).
 *
 * @param type Valor enumerado representando o tipo JVM (definido em type_enum).
 * @param frame Ponteiro para o \c Frame onde a variável será inicializada (pode ser usado para contexto).
 * @return Valor \c dtype inicializado conforme o tipo.
 */
dtype initialize_var(type_enum type, Frame *frame);

#endif /* INTERPRETER_H */
