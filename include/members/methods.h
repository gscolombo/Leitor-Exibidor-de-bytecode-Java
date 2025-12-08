/**
 * @file methods.h
 * @brief Funções utilitárias para exibição dos métodos declarados em um arquivo `.class`.
 *
 * Este módulo contém a função `show_methods`, responsável por imprimir de forma
 * legível informações sobre todos os métodos presentes na estrutura `ClassFile`,
 * incluindo:
 *   - flags de acesso (public, static, final…)
 *   - nome do método
 *   - descritor JVM (como "(I)V" ou "([Ljava/lang/String;)V")
 *   - atributos associados, especialmente o atributo Code
 *
 * A apresentação segue estilo semelhante ao do comando `javap -v`.
 */

#ifndef MEMBERS_METHODS_H
#define MEMBERS_METHODS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Classfile.h"
#include "uinteger.h"
#include "member.h"
#include "utils.h"
#include "wchar.h"

/**
 * @brief Exibe informações sobre todos os métodos de uma classe.
 *
 * Para cada entrada em `cf->methods`, a função imprime:
 *  - índice e flags de acesso (ex.: public, private, static…)
 *  - nome do método (via constant pool)
 *  - descritor JVM convertido para forma legível (opcional)
 *  - informações do atributo Code (se presente):
 *        * max_stack
 *        * max_locals
 *        * code_length
 *        * exceções mapeadas na exception_table
 *        * atributos aninhados (LineNumberTable, LocalVariableTable…)
 *
 * A função não modifica o conteúdo de `cf`; apenas lê e formata a saída.
 *
 * @param cf Ponteiro para a estrutura `ClassFile` cujos métodos devem ser exibidos.
 */
void show_methods(const ClassFile *cf);

#endif /* MEMBERS_METHODS_H */
