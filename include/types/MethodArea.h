// include/types/MethodArea.h
#ifndef METHOD_AREA_H
#define METHOD_AREA_H

#include <stdio.h>

#include "attributes.h"
#include "Classfile.h"
#include "java_type.h"

/**
 * @file MethodArea.h
 * @brief Estruturas que representam classes carregadas em tempo de execução (Method Area).
 *
 * Estas estruturas representam a visão em tempo de execução de classes, campos,
 * métodos e constantes, usadas pelo interpretador para resolução e execução de bytecode.
 */

/**
 * @struct FieldImpl
 * @brief Representação em tempo de execução de um campo de classe.
 *
 * Campos armazenam nome, tipo legível, flags de acesso, valor atual (dtype) e
 * atributos associados (por exemplo, ConstantValue).
 */
typedef struct FieldImpl
{
    /** Nome do campo (string alocada dinamicamente). */
    char *name;
    /** Tipo do campo em formato legível (p.ex. "I", "Ljava/lang/String;"). */
    char *type;
    /** Flags de acesso (u2) conforme a especificação da JVM. */
    u2 access_flags;
    /** Valor em tempo de execução representado por dtype (definido em java_type.h). */
    dtype value;
    /** Ponteiro para atributos associados ao campo (array de attribute). */
    attribute *attrs;
} Field;

/**
 * @struct MethodImpl
 * @brief Representação em tempo de execução de um método de classe.
 *
 * Contém nome, descritor, representação dos parâmetros e tipo de retorno,
 * flags de acesso e uma subestrutura para o bytecode quando aplicável.
 */
typedef struct MethodImpl
{
    /** Nome do método (ex.: "main", "toString"). */
    char *name;
    /** Descritor JVM do método (ex.: "(I)V"). */
    char *descriptor;
    /** String com parâmetros (representação legível, p.ex. "int,String"). */
    char *params;
    /** Tipo de retorno em formato legível. */
    char *rettype;
    /** Flags de acesso do método. */
    u2 access_flags;
    /** Informação do bytecode (quando o método possui atributo Code). */
    struct
    {
        /** Número de argumentos esperados. */
        u2 nargs;
        /** Tamanho máximo da pilha de operandos (max_stack). */
        u2 max_stack;
        /** Número máximo de variáveis locais (max_locals). */
        u2 max_locals;
        /** Comprimento em bytes do array de código. */
        u4 code_length;
        /** Ponteiro para os bytes do código (array de u1). */
        u1 *code;
    } bytecode;
} Method;

/**
 * @struct RuntimeConstant
 * @brief Representação em tempo de execução de uma entrada de constant pool convertida.
 *
 * Guarda o tipo do constante e um union com o valor interpretado para uso em tempo de execução.
 */
typedef struct
{
    /** Tipo da constante em um inteiro auxiliar (por exemplo, para distinguir tags). */
    int type;
    /** Valor interpretado da constante (string, int, long, float ou double). */
    union constant
    {
        char *strref;   /**< Referência a string (UTF8) */
        int32_t i;      /**< Valor inteiro 32-bit */
        int64_t l;      /**< Valor inteiro 64-bit */
        float f;        /**< Valor float */
        double d;       /**< Valor double */
    } value;
} RuntimeConstant;

/**
 * @struct ClassImpl
 * @brief Estrutura que representa uma classe carregada na Method Area.
 *
 * Contém informações derivadas do ClassFile, além de estruturas auxiliares
 * para acesso em tempo de execução (runtime constant pool, campos e métodos).
 */
typedef struct ClassImpl
{
    /** Nome interno da classe (ex.: "com/example/MyClass"). */
    char *name;
    /** Nome da superclasse (string legível). */
    char *super;
    /** Índice da superclasse no constant pool (se aplicável). */
    u2 super_index;
    /** Flags de acesso da classe (u2). */
    u2 access_flags;
    /** Quantidade de entradas no runtime constant pool. */
    u2 constants_count;
    /** Array de constantes em formato runtime, de tamanho constants_count. */
    RuntimeConstant *runtime_cp;
    /** Quantidade de campos declarados na classe. */
    u2 field_count;
    /** Array de campos (Field) com tamanho field_count. */
    Field *fields;
    /** Quantidade de métodos declarados na classe. */
    u2 method_count;
    /** Array de métodos (Method) com tamanho method_count. */
    Method *methods;
    /* TODO: Handle interfaces */
} Class;

/**
 * @struct MethodArea
 * @brief Estrutura que representa a Method Area (conjunto de classes carregadas).
 *
 * Mantém um vetor de classes carregadas, contagem e um array de referências
 * auxiliares usado pelo coletor/resolução de referências (refs) e um contador de referências.
 */
typedef struct
{
    /** Número de classes atualmente carregadas. */
    size_t num_classes;
    /** Array de classes (tamanho num_classes). */
    Class *classes;
    /** Contador de referências (uso interno). */
    u4 ref_count;
    /** Vetor de ponteiros genéricos para referências auxiliares (uso interno). */
    void **refs;
} MethodArea;

#endif /* METHOD_AREA_H */
