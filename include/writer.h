/**
 * @file writer.h
 * @brief Declarações para funções de exibição (printing) de estruturas de ClassFile.
 *
 * Este header declara as funções responsáveis por imprimir/formatar informações
 * extraídas de um arquivo .class (ClassFile) em formato legível para o usuário.
 * Tipicamente usadas pelo componente exibidor do projeto para mostrar:
 *  - versão do classfile
 *  - entradas do constant pool
 *  - campos e métodos com seus descritores e atributos
 *
 * Inclui os headers necessários para acessar as estruturas envolvidas.
 */

#ifndef WRITER_H
#define WRITER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <wchar.h>

#include "Classfile.h"
#include "member.h"
#include "utils.h"
#include "cp/writer.h"
#include "methods.h"
#include "fields.h"

/**
 * @brief Exibe em stdout informações detalhadas do ClassFile.
 *
 * Esta função percorre a estrutura `ClassFile` e imprime informações relevantes
 * em formato legível, incluindo (mas não limitado a):
 *  - magic e versões
 *  - constant pool (entradas significativas)
 *  - flags de acesso, nome da classe e superclasse
 *  - interfaces, fields e methods com descritores e atributos
 *
 * A função não altera a estrutura passada e não libera memória; é responsabilidade
 * do chamador gerenciar a alocação/desalocação.
 *
 * @param cf Ponteiro para a estrutura ClassFile a ser exibida. Se NULL, a função
 *           deve retornar sem produzir saída.
 */
void show_classfile(ClassFile *cf);

#endif /* WRITER_H */
