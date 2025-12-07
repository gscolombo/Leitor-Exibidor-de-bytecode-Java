/**
 * @file include/members/methods.h
 * @brief Declarações para exibição (printing) dos métodos (methods) de um ClassFile.
 *
 * Este header declara a função responsável por percorrer e imprimir os métodos
 * (member_info) presentes em uma estrutura `ClassFile`. A implementação normalmente
 * resolve nomes e descritores no constant pool e utiliza utilitários como
 * `parse_descriptor()` e `parse_flags()` para formatar a saída de forma legível.
 *
 * Observações:
 * - A função apenas exibe informações; não altera nem libera a estrutura `ClassFile`.
 * - Os nomes e descritores devem já estar disponíveis no constant pool (UTF-8
 *   decodificado em `wchar_t *`) antes da chamada.
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
 * @brief Exibe os métodos (methods) definidos em um ClassFile.
 *
 * Para cada método, a função imprime detalhes como:
 *  - modificadores/flags de acesso (formatadas com parse_flags)
 *  - nome do método (resolvido via constant pool)
 *  - assinatura/descritor do método (resolvido e convertido por parse_descriptor)
 *  - informações sobre atributos do método (Code, Exceptions, LineNumberTable, etc.)
 *
 * A função deve produzir uma saída legível, similar à ferramenta `javap`.
 *
 * @param cf Ponteiro constante para a estrutura ClassFile cujos métodos serão exibidos.
 *           Se NULL, a função deve retornar sem produzir saída.
 */
void show_methods(const ClassFile *cf);

#endif /* MEMBERS_METHODS_H */
