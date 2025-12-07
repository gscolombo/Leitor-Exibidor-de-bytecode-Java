/**
 * @file include/members/fields.h
 * @brief Declarações para exibição (printing) dos campos (fields) de um ClassFile.
 *
 * Este header declara a função responsável por percorrer e imprimir os campos
 * (member_info) presentes em uma estrutura `ClassFile`. A implementação usa o
 * constant pool para resolver nomes e descritores e utiliza utilitários como
 * `parse_descriptor()` e `parse_flags()` para formatar a saída.
 *
 * Observações:
 * - A função apenas exibe informações; não altera nem libera a estrutura
 *   `ClassFile`. A responsabilidade pela alocação/desalocação permanece com o
 *   chamador.
 * - Os nomes e descritores devem já estar disponíveis no constant pool (UTF-8
 *   decodificado em `wchar_t *`) antes da chamada.
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
 * @brief Exibe os campos (fields) definidos em um ClassFile.
 *
 * Para cada campo, a função imprime detalhes como:
 *  - flags de acesso (formatadas com parse_flags)
 *  - nome do campo (resolvido via constant pool)
 *  - descritor/ tipo do campo (resolvido e possivelmente convertido por parse_descriptor)
 *  - lista de atributos associada (se houver)
 *
 * A saída é formatada de forma legível ao usuário, semelhante ao `javap`.
 *
 * @param cf Ponteiro constante para a estrutura ClassFile cujo conjunto de fields
 *           será exibido. Se NULL, a função deve retornar sem produzir saída.
 */
void show_fields(const ClassFile *cf);

#endif /* MEMBERS_FIELDS_H */
