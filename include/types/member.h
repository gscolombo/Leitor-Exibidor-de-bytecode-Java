/**
 * @file include/types/member.h
 * @brief Estruturas relacionadas a membros (fields e methods) do ClassFile.
 *
 * A especificação da JVM define a estrutura `member_info` para representar tanto
 * campos (fields) quanto métodos (methods) de uma classe. Cada membro possui:
 *  - flags de acesso (public, static, final, ...)
 *  - índice para o nome (UTF-8) no constant pool
 *  - índice para o descritor no constant pool
 *  - lista de atributos associados
 *
 * Este header contém a estrutura `member_info` utilizada por fields e methods,
 * além da estrutura `FlagMap`, usada para mapear flags binárias para strings
 * legíveis nas funções de exibição.
 */

#ifndef TYPES_INFO_H
#define TYPES_INFO_H

#include "uinteger.h"
#include "attributes.h"

/**
 * @struct FlagMap
 * @brief Mapeamento entre um bit de flag (u2) e seu nome textual.
 *
 * Usado por funções como `parse_flags()` para converter valores binários
 * de `access_flags` em representações de texto, como:
 *
 * ```
 * ACC_PUBLIC, ACC_STATIC, ACC_FINAL
 * ```
 */
typedef struct FlagMap
{
    u2 flag;          /**< Valor do bit correspondente à flag. */
    const char *name; /**< Nome textual da flag (ex.: "ACC_PUBLIC"). */
} FlagMap;

/**
 * @struct member_info
 * @brief Representa um campo (field) ou método (method) definido em um ClassFile.
 *
 * Cada `member_info` contém índices para o constant pool e uma lista de atributos
 * específicos para o membro. A estrutura corresponde exatamente ao formato definido
 * na Java Virtual Machine Specification.
 *
 * Campos:
 * - **access_flags**: combinações de ACC_PUBLIC, ACC_STATIC, ACC_FINAL, etc.
 * - **name_index**: índice no constant pool que aponta para uma entrada UTF8 com o nome.
 * - **descriptor_index**: índice que aponta para o descritor JVM (ex.: "(I)V", "Ljava/lang/String;").
 * - **attributes_count**: número de atributos do membro.
 * - **attributes**: lista alocada dinamicamente de atributos (struct `attribute`).
 */
typedef struct member_info
{
    u2 access_flags;      /**< Flags de acesso (ACC_*). */
    u2 name_index;        /**< Índice no constant pool para o nome UTF8 do membro. */
    u2 descriptor_index;  /**< Índice para o descritor do membro (parâmetros e retorno). */
    u2 attributes_count;  /**< Quantidade de atributos associados ao campo ou método. */
    attribute *attributes;/**< Lista de atributos deste membro. */
} member_info;

#endif /* TYPES_INFO_H */
