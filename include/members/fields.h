/**
 * @file fields.h
 * @brief Funções utilitárias para exibição de campos (fields) de um arquivo `.class`.
 *
 * Este módulo disponibiliza a função `show_fields`, responsável por imprimir
 * informações sobre todos os campos declarados na estrutura `ClassFile`,
 * incluindo:
 *  - flags de acesso (public, private, static, etc.),
 *  - nome do campo,
 *  - descritor,
 *  - atributos associados (como ConstantValue).
 *
 * A exibição segue estilo semelhante ao comando `javap -v`.
 */

#ifndef MEMBERS_FIELDS_H
#define MEMBERS_FIELDS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Classfile.h"
#include "uinteger.h"
#include "member.h"
#include "utils.h"
#include "wchar.h"

/**
 * @brief Exibe todos os campos (fields) de uma classe presente em `ClassFile`.
 *
 * A função percorre o vetor `cf->fields`, imprimindo:
 *  - índice e acesso (public, private, protected, static…)
 *  - nome decodificado a partir do constant pool
 *  - descritor (ex.: `I`, `Ljava/lang/String;`)
 *  - atributos específicos do campo, como:
 *      - ConstantValue
 *
 * Esta função não altera o conteúdo de `cf`; apenas realiza leitura e formatação.
 *
 * @param cf Ponteiro para a estrutura `ClassFile` que contém os campos a serem exibidos.
 */
void show_fields(const ClassFile *cf);

#endif /* MEMBERS_FIELDS_H */
