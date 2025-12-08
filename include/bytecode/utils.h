/**
 * @file utils.h
 * @brief Funções auxiliares para manipulação e interpretação de bytecodes JVM.
 *
 * Este módulo contém funções utilitárias usadas por instruções específicas,
 * pela implementação de `tableswitch`/`lookupswitch`, pela manipulação de
 * StringBuffer/StringBuilder e por mecanismos de alocação de referências em
 * tempo de execução.
 *
 * As rotinas aqui definidas complementam o interpretador principal.
 */

#ifndef BYTECODE_UTILS_H
#define BYTECODE_UTILS_H

#include <stdint.h>
#include "uinteger.h"
#include "Frame.h"
#include "interpreter.h"

/* ========================================================================== */
/* Switch Helpers                                                             */
/* ========================================================================== */

/**
 * @brief Recupera um valor de 32 bits utilizado em instruções `tableswitch` e `lookupswitch`.
 *
 * Bytecodes de switch da JVM possuem uma estrutura complexa com alinhamento em
 * múltiplos de 4 bytes. Esta função interpreta corretamente valores de 4 bytes
 * (u4) obtidos do bytecode.
 *
 * @param raw_value Valor de 4 bytes bruto lido como `u4`.
 * @param code Ponteiro para o vetor de bytes do bytecode.
 * @return Valor inteiro de 32 bits interpretado.
 *
 * @note A função é implementada em outro módulo e declarada como `extern`.
 */
extern int32_t get_switch_32B_values(u4 raw_value, const u1 *code);

/* ========================================================================== */
/* Debug / Output Helpers                                                     */
/* ========================================================================== */

/**
 * @brief Imprime informações formatadas durante a execução do interpretador.
 *
 * Usado tipicamente para instruções de debugging, rastreamento e mensagens
 * internas do interpretador.
 *
 * @param frame Frame atual em execução.
 * @param msg Mensagem a ser exibida.
 * @param prefix Caractere opcional usado como prefixo.
 * @param suffix Caractere opcional usado como sufixo.
 */
void _print(Frame *frame, const char *msg, char prefix, char suffix);

/* ========================================================================== */
/* StringBuffer / StringBuilder Helpers                                       */
/* ========================================================================== */

/**
 * @brief Inicializa um objeto do tipo StringBuffer na área de referências.
 *
 * Esta função é usada por bytecodes como `new`, `invokespecial` e métodos
 * específicos que manipulam strings mutáveis em tempo de execução.
 *
 * @param frame Frame atual.
 * @param out_dtype Variável de saída contendo a referência inicializada.
 */
void init_stringbuffer(Frame *frame, dtype *out_dtype);

/**
 * @brief Implementação do método `append` de StringBuffer/StringBuilder.
 *
 * A JVM trata chamadas a `java/lang/StringBuffer.append` como operações
 * internas de manipulação de referências. Esta função executa essa manipulação.
 *
 * @param frame Frame atual.
 */
void strbuf_append(Frame *frame);

/**
 * @brief Implementação do método `toString` de StringBuffer/StringBuilder.
 *
 * Constrói uma nova string a partir do buffer interno e empilha a referência
 * resultante na pilha de operandos.
 *
 * @param frame Frame atual.
 */
void strbuf_tostring(Frame *frame);

/* ========================================================================== */
/* Alloc Helpers (Runtime Heap References)                                    */
/* ========================================================================== */

/**
 * @brief Adiciona uma referência a um objeto recém-criado ao sistema de referências.
 *
 * Usado após a criação de objetos, arrays ou strings para registrar a referência
 * no sistema de rastreamento de objetos (para GC ou contagem de referências).
 *
 * @param frame Frame atual.
 * @param ref Ponteiro para o objeto/alocação.
 */
void appendref(Frame *frame, void *ref);

/**
 * @brief Registra e inicializa estrutura de referência para um novo objeto.
 *
 * Função auxiliar chamada por bytecodes de alocação (ex.: `new`, `anewarray`),
 * configurando adequadamente o registro da referência dentro do ambiente de
 * execução.
 *
 * @param frame Frame atual.
 */
void allocref(Frame *frame);

#endif /* BYTECODE_UTILS_H */
