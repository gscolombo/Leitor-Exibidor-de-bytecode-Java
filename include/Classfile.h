/**
 * @file Classfile.h
 * @brief Estruturas e funções para representar e liberar um arquivo .class Java (ClassFile).
 *
 * Este header contém a definição da estrutura `ClassFile` que representa o conteúdo
 * de um arquivo de bytecode Java segundo a JVM Specification (cabecalho, constant pool,
 * campos, métodos e atributos), além de funções utilitárias para liberar memória
 * associada.
 */

#ifndef CLASSFILE_H_
#define CLASSFILE_H_

#include <stdlib.h>

#include "uinteger.h"
#include "member.h"
#include "constants.h"
#include "utils.h"

/**
 * @struct ClassFile
 * @brief Representa um arquivo .class Java carregado em memória.
 *
 * Campos principais:
 * - magic: número mágico do arquivo (.class) (0xCAFEBABE).
 * - minor_version / major_version: versão do formato da JVM.
 * - constant_pool_count: tamanho do constant pool (1 + número de entradas).
 * - constant_pool: array de entradas do constant pool (cp_info *).
 * - access_flags: flags de acesso da classe (public, final, etc).
 * - this_class: índice no constant pool para o nome desta classe.
 * - super_class: índice no constant pool para o nome da superclasse.
 * - interfaces_count / interfaces: número e lista de interfaces implementadas.
 * - fields_count / fields: número e array de info de campos (member_info).
 * - methods_count / methods: número e array de info de métodos (member_info).
 * - attributes_count / attributes: número e array de atributos da classe.
 */
typedef struct ClassFile
{
    u4 magic;
    u2 minor_version;
    u2 major_version;
    u2 constant_pool_count;
    cp_info *constant_pool;
    u2 access_flags;
    u2 this_class;
    u2 super_class;
    u2 interfaces_count;
    u2 *interfaces;
    u2 fields_count;
    member_info *fields;
    u2 methods_count;
    member_info *methods;
    u2 attributes_count;
    attribute *attributes;
} ClassFile;


/**
 * @brief Libera toda a memória alocada para uma instância de ClassFile.
 *
 * Esta função deve liberar:
 * - constant_pool (e seus sub-objetos),
 * - arrays de interfaces, fields, methods,
 * - atributos associados à classe.
 *
 * @param cf Ponteiro para o ClassFile a ser liberado. Se NULL, a função não faz nada.
 */
void free_classfile(ClassFile *cf);


/**
 * @brief Libera um array de atributos (helper).
 *
 * Libera `attributes_count` atributos começando no ponteiro `attributes`.
 * Geralmente usado para liberar o campo de atributos de classes, métodos ou fields.
 *
 * @param cp Ponteiro para o constant pool associado (necessário se os atributos
 *           possuem referências ao constant pool que precisam ser liberadas).
 * @param attributes_count Número de atributos no array.
 * @param attributes Ponteiro para o primeiro atributo do array.
 */
void free_attributes(cp_info *cp, u2 attributes_count, attribute *attributes);

#endif /* CLASSFILE_H_ */


