/**
 * @file member.h
 * @brief Estruturas que representam campos e métodos de um arquivo `.class` da JVM.
 *
 * Este módulo fornece dois tipos principais:
 * - `FlagMap`: utilizado para traduzir valores numéricos de flags em seus nomes legíveis.
 * - `member_info`: estrutura compartilhada entre fields e methods conforme a especificação da JVM.
 *
 * Ambas são usadas durante a leitura do ClassFile e durante a exibição e interpretação do bytecode.
 */

#ifndef TYPES_INFO_H
#define TYPES_INFO_H

#include "uinteger.h"
#include "attribute_info.h"

/**
 * @struct FlagMap
 * @brief Estrutura usada para mapear uma flag de acesso (u2) para seu nome textual.
 *
 * É usada principalmente por funções como `parse_flags()` para gerar strings
 * contendo nomes das flags de acesso (ex.: "public static final").
 */
typedef struct FlagMap
{
    /**
     * @brief Valor numérico da flag (máscara de bits).
     *
     * Exemplos comuns:
     * - 0x0001 → ACC_PUBLIC  
     * - 0x0008 → ACC_STATIC  
     * - 0x0010 → ACC_FINAL
     */
    u2 flag;

    /**
     * @brief Nome textual correspondente à flag.
     *
     * Geralmente corresponde à nomenclatura oficial da JVM (ex.: `"public"`, `"static"`).
     */
    const char *name;

} FlagMap;

/**
 * @struct member_info
 * @brief Estrutura que representa um *field* ou *method* no arquivo `.class`.
 *
 * De acordo com a especificação da JVM, tanto campos quanto métodos
 * compartilham a mesma estrutura física no arquivo `.class`, chamada `member_info`.
 *
 * Ela contém:
 * - flags de acesso (public, static, final…)
 * - índices para nome e descritor
 * - lista de atributos específicos (Code, Exceptions, ConstantValue…)
 *
 * Esta estrutura é usada diretamente por `read_member()` durante o parsing.
 */
typedef struct member_info
{
    /**
     * @brief Máscara de bits contendo flags de acesso.
     *
     * Valores possíveis estão definidos pela JVM, ex.:
     * - ACC_PUBLIC  
     * - ACC_PRIVATE  
     * - ACC_PROTECTED  
     * - ACC_STATIC  
     * - ACC_FINAL  
     * e outros.
     */
    u2 access_flags;

    /**
     * @brief Índice no constant pool apontando para a string do nome.
     *
     * O nome é uma entrada UTF8 (tag = 1) no constant pool.
     */
    u2 name_index;

    /**
     * @brief Índice no constant pool apontando para o descritor do campo/método.
     *
     * O descritor segue a sintaxe da JVM, por exemplo:
     * - para fields: `"I"`, `"Ljava/lang/String;"`, `"[I"`, etc.
     * - para methods: `"(II)I"`, `"([Ljava/lang/String;)V"`, etc.
     */
    u2 descriptor_index;

    /**
     * @brief Número de atributos associados ao campo/método.
     *
     * Pode incluir atributos como:
     * - `Code`
     * - `Exceptions`
     * - `LineNumberTable`
     * - `Signature`
     * etc.
     */
    u2 attributes_count;

    /**
     * @brief Vetor de atributos do campo/método.
     *
     * Preenchido por `read_attributes()`.  
     * O chamador deve liberar essa memória com `free_attributes()`
     * ou `free_classfile()`.
     */
    attribute *attributes;

} member_info;

#endif /* TYPES_INFO_H */
