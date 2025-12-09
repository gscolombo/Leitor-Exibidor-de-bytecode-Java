/**
 * @file references.h
 * @brief Implementações das instruções JVM relacionadas a referências:
 *        objetos, campos, métodos e arrays.
 *
 * Este módulo contém as instruções responsáveis por:
 *  - acessar campos estáticos e de instância;
 *  - criar objetos e arrays;
 *  - invocar métodos (estáticos, especiais, virtuais e de interface);
 *  - obter o comprimento de arrays.
 *
 * Essas instruções fazem parte do núcleo do modelo orientado a objetos da JVM.
 */

#ifndef BYTECODE_IMPL_REFERENCES_H
#define BYTECODE_IMPL_REFERENCES_H

#include "Frame.h"
#include "interpreter.h"
#include "bootstrap_loader.h"
#include "bytecode/utils.h"

/* ========================================================================== */
/* Acesso a campos                                                            */
/* ========================================================================== */

/**
 * @brief Implementa a instrução JVM `getstatic`.
 *
 * Recupera o valor de um campo estático da classe especificada.
 *
 * Semântica:
 * - Resolve o campo via constant pool.
 * - Carrega o valor estático e o empilha na operand stack.
 *
 * @param frame Frame atual.
 */
void getstatic(Frame *frame);

/**
 * @brief Implementa a instrução JVM `putstatic`.
 *
 * Armazena um valor em um campo estático.
 *
 * Semântica:
 * - Retira um valor da operand stack.
 * - Resolve o campo e armazena o valor.
 *
 * @param frame Frame atual.
 */
void putstatic(Frame *frame);

/**
 * @brief Implementa a instrução JVM `getfield`.
 *
 * Obtém o valor de um campo de instância.
 *
 * Semântica:
 * - Recebe do topo da pilha uma referência a objeto.
 * - Resolve o campo (via constant pool).
 * - Acessa o valor dentro da instância.
 * - Empilha o valor resultante.
 *
 * @param frame Frame atual.
 */
void getfield(Frame *frame);

/**
 * @brief Implementa a instrução JVM `putfield`.
 *
 * Armazena valor em campo de instância.
 *
 * Semântica:
 * - Retira um valor do topo da pilha.
 * - Retira a referência ao objeto.
 * - Resolve e grava no campo correspondente.
 *
 * @param frame Frame atual.
 */
void putfield(Frame *frame);

/* ========================================================================== */
/* Invocação de métodos                                                       */
/* ========================================================================== */

/**
 * @brief Implementa `invokespecial`.
 *
 * Usado para:
 *  - chamadas a construtores (`<init>`)
 *  - chamadas a métodos privados
 *  - chamadas explícitas a superclasse (`super.method()`)
 *
 * @param frame Frame atual.
 */
void invokespecial(Frame *frame);

/**
 * @brief Implementa `invokevirtual`.
 *
 * Invocação polimórfica:
 * - A resolução ocorre dinamicamente de acordo com o objeto no topo da pilha.
 *
 * Semântica:
 * - Recupera a referência do objeto.
 * - Resolve método virtual final.
 * - Cria um novo frame e executa o método.
 *
 * @param frame Frame atual.
 */
void invokevirtual(Frame *frame);

/**
 * @brief Implementa `invokestatic`.
 *
 * Invocação de método estático — não depende de instância.
 *
 * @param frame Frame atual.
 */
void invokestatic(Frame *frame);

/**
 * @brief Implementa `invokeinterface`.
 *
 * Semântica semelhante a `invokevirtual`, mas via referência a interface.
 *
 * JVM especifica resolução dinâmica a partir da classe real do objeto.
 *
 * @param frame Frame atual.
 */
void invokeinterface(Frame *frame);

/* ========================================================================== */
/* Criação e manipulação de arrays                                            */
/* ========================================================================== */

/**
 * @brief Implementa `newarray` (arrays primitivos).
 *
 * O tipo é determinado pelo operando imediato do bytecode.
 *
 * Exemplo:
 *  - T_INT (10)
 *  - T_FLOAT (6)
 *
 * @param frame Frame atual.
 */
void newarray(Frame *frame);

/**
 * @brief Implementa `anewarray` (arrays de referência).
 *
 * O tipo base é um CONSTANT_Class referenciado pelo pool.
 *
 * @param frame Frame atual.
 */
void anewarray(Frame *frame);

/**
 * @brief Implementa `_arraylength`.
 *
 * Obtém o comprimento de um array, semelhante a `arrayref.length` em Java.
 *
 * Semântica:
 * - Retira referência a array do topo.
 * - Empilha o tamanho (`arraylength`).
 *
 * @param frame Frame atual.
 */
void _arraylength(Frame *frame);

/* ========================================================================== */
/* Criação de objetos                                                         */
/* ========================================================================== */

/**
 * @brief Implementa a instrução JVM `new`.
 *
 * Semântica:
 *  - Resolve o nome da classe.
 *  - Aloca um novo objeto (não inicializa).
 *  - Empilha a referência ao objeto recém-criado.
 *  - A inicialização ocorre no `invokespecial` que chama `<init>`.
 *
 * @param frame Frame atual.
 */
void new(Frame *frame);

#endif /* BYTECODE_IMPL_REFERENCES_H */
