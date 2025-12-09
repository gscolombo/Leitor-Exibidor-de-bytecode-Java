#ifndef FREE_H_
#define FREE_H_

#include <stdlib.h>

#include "utils.h"
#include "Classfile.h"

/**
 * @file free.h
 * @brief Declarações de funções para liberação de memória alocada pelas estruturas do projeto.
 *
 * Este módulo fornece rotinas para liberar recursivamente memória alocada
 * durante o parsing e carregamento de arquivos `.class`, incluindo pools de constantes,
 * atributos, campos, métodos e estruturas aninhadas.
 */

 /**
  * @brief Libera toda a memória associada a uma estrutura `ClassFile`.
  *
  * A função deve liberar:
  * - entradas do constant pool (incluindo strings UTF8, classes, nomes/descritores);
  * - arrays de interfaces;
  * - fields e seus atributos;
  * - methods e seus atributos (incluindo Code.code, exception_table, atributos aninhados);
  * - atributos de classe;
  * - quaisquer outros ponteiros alocados presentes na estrutura `ClassFile`.
  *
  * @param cf Ponteiro para a estrutura `ClassFile` que será liberada.
  *
  * @note Após a chamada, o conteúdo apontado por `cf` não deve mais ser usado.
  */
void free_classfile(ClassFile *cf);

/**
 * @brief Libera um vetor de atributos (`attribute`) usando informações do constant pool.
 *
 * Para cada atributo, a função deve:
 * - identificar o tipo do atributo (ex.: Code, Exceptions, InnerClasses, etc.);
 * - liberar subestruturas alocadas (ex.: code, exception_table, nested attributes);
 * - finalmente, liberar o próprio array `attr` (se aplicável ao contexto).
 *
 * @param cp Ponteiro para o constant pool (`cp_info[]`) associado ao `ClassFile`.
 *           É necessário para interpretar nomes e decidir como liberar cada atributo.
 * @param count Número de atributos no vetor `attr`.
 * @param attr Ponteiro para o vetor de `attribute` a ser liberado.
 *
 * @note A função garante que, mesmo se partes internas estiverem NULL devido a falhas
 *       de alocação, nenhum acesso inválido à memória será realizado.
 */
void free_attributes(cp_info *cp, u2 count, attribute *attr);

#endif /* FREE_H_ */
