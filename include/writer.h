/**
 * @file writer.h
 * @brief Declarações de funções para exibição de informações de um arquivo `.class`.
 *
 * Este módulo contém a função responsável por imprimir, em formato legível,
 * todas as estruturas internas de um `ClassFile`, de forma semelhante ao comando
 * `javap -v`. Ele percorre a constant pool, campos, métodos e atributos,
 * exibindo informações detalhadas conforme a especificação da JVM 8.
 */

#ifndef WRITER_H
#define WRITER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Classfile.h"
#include "member.h"
#include "utils.h"
#include "cp/writer.h"
#include "methods.h"
#include "fields.h"

/**
 * @brief Exibe as informações detalhadas de um `ClassFile`.
 *
 * A função imprime todas as seções da estrutura `ClassFile`, incluindo:
 * - Magic number, versões e flags de acesso;
 * - Constant pool com detalhes de cada entrada;
 * - Interfaces implementadas;
 * - Fields (campos) com seus atributos;
 * - Methods (métodos) incluindo atributos como Code, Exceptions, etc.;
 * - Atributos de classe.
 *
 * O formato de saída é propositalmente semelhante ao do comando
 * `javap -v`, facilitando comparação com ferramentas oficiais.
 *
 * @param cf Ponteiro para a estrutura `ClassFile` que será exibida.
 */
void show_classfile(ClassFile *cf);

#endif /* WRITER_H */
