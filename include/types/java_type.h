/**
 * @file java_type.h
 * @brief Tipos usados para representar valores em tempo de execução na JVM simplificada.
 *
 * Este módulo define estruturas que representam:
 * - tipos primitivos da JVM,
 * - referências (objeto, string, array),
 * - categorias de tipos (CAT1, CAT2),
 * - valores de execução (dtype), que são usados pelo interpretador e pela pilha de operandos.
 */

#ifndef JAVA_TYPE_H
#define JAVA_TYPE_H

#include "uinteger.h"
#include "MethodArea.h"

/**
 * @enum cat
 * @brief Categoria do tipo segundo a JVM.
 *
 * A JVM divide valores em duas categorias:
 * - CAT1: ocupa 1 slot na pilha (int, float, reference, returnAddress etc.)
 * - CAT2: ocupa 2 slots (long, double)
 */
typedef enum cat
{
    CAT1, /**< Tipos que ocupam 1 slot na pilha. */
    CAT2  /**< Tipos que ocupam 2 slots na pilha (long, double). */
} cat;

/**
 * @enum type_enum
 * @brief Enumeração de tipos primitivos e especiais suportados pela JVM.
 *
 * Esse enum é usado especialmente na inicialização de variáveis e conversões.
 */
typedef enum type_enum
{
    BYTE,           /**< Representa um valor byte (8 bits). */
    SHORT,          /**< Representa um short (16 bits). */
    INT,            /**< Representa um int (32 bits). */
    LONG,           /**< Representa um long (64 bits). */
    CHAR,           /**< Representa um char UTF-16 (16 bits). */
    FLOAT,          /**< Representa um float (32 bits). */
    DOUBLE,         /**< Representa um double (64 bits). */
    BOOLEAN,        /**< Representa um boolean (true/false). */
    RETURN_ADDRESS, /**< Endereço de retorno usado por instruções como JSR/RET. */
    REFERENCE       /**< Referência para objetos, strings ou arrays. */
} type_enum;

/**
 * @union primitive_type
 * @brief Representa qualquer tipo primitivo suportado pela JVM.
 *
 * Usado para armazenar valores *não referenciais* dentro de java_type.
 */
typedef union
{
    int8_t byte;       /**< Valor do tipo byte. */
    int16_t _short;    /**< Valor do tipo short. */
    int32_t _int;      /**< Valor do tipo int. */
    int64_t _long;     /**< Valor do tipo long. */
    u2 _char;          /**< Valor do tipo char (UTF-16). */
    float _float;      /**< Valor do tipo float. */
    double _double;    /**< Valor do tipo double. */
    bool boolean;      /**< Valor boolean. */
    u4 returnAddress;  /**< Usado para instruções de fluxo como jsr/ret. */
} primitive_type;

/**
 * @struct ArrayRef
 * @brief Representa uma referência para arrays em tempo de execução.
 *
 * Campos:
 * - t: tipo base do array (ex.: INT, FLOAT, REFERENCE)
 * - class_name: nome da classe de referência (para arrays de objetos)
 * - arraylength: tamanho total do array
 * - dims: número de dimensões
 * - values: ponteiro para área de dados (ou ponteiro para ponteiros se multidimensional)
 */
typedef struct ArrayRef
{
    u1 t;                /**< Tipo base do array (type_enum para arrays primitivos). */
    const char *class_name; /**< Nome da classe para arrays de objetos. */
    u4 arraylength;      /**< Tamanho da dimensão atual. */
    u1 dims;             /**< Número de dimensões do array. */
    void *values;        /**< Área de dados. Para dim > 1, armazena referências. */
} ArrayRef;

/**
 * @struct reference
 * @brief Representa uma referência JVM (objeto, string ou array).
 *
 * A JVM opera sobre referências, que podem apontar para:
 * - objetos instanciados,
 * - strings constantes ou criadas em runtime,
 * - arrays multidimensionais ou primitivos.
 */
typedef struct reference
{
    /**
     * @brief Tipo da referência.
     *
     * REF_NULL       → referência nula  
     * REF_STRING     → referência para string  
     * REF_OBJECT     → referência para objeto instanciado  
     * REF_ARRAY      → referência para array (primitivo ou não)
     */
    enum
    {
        REF_NULL,
        REF_STRING,
        REF_OBJECT,
        REF_ARRAY
    } type;

    /**
     * @union value
     * @brief Valor efetivo da referência (string, objeto ou array).
     */
    union
    {
        struct ClassImpl *object_ref; /**< Referência para instância de objeto. */

        /**
         * @union array_ref
         * @brief Subreferência para strings ou arrays.
         */
        union
        {
            char *string; /**< Ponteiro para string UTF-8. */
            ArrayRef array; /**< Representação interna de um array. */
        } array_ref;

    } value;

} reference;

/**
 * @union java_type
 * @brief Representa tanto valores primitivos quanto referências.
 *
 * É o tipo genérico que trafega pela pilha de operandos, variáveis locais,
 * campos e valores de retorno do interpretador.
 */
typedef union java_type
{
    reference *ref;   /**< Quando o tipo é REFERENCE. */
    primitive_type t; /**< Quando o valor é primitivo. */
} java_type;

/**
 * @struct dtype
 * @brief Tipo completo armazenado na pilha da JVM (valor + categoria).
 *
 * A categoria (CAT1 ou CAT2) determina quantos slots da pilha esse valor ocupa.
 */
typedef struct dtype
{
    cat cat;         /**< Categoria do tipo (CAT1 ou CAT2). */
    java_type value; /**< Valor primitivo ou referência. */
} dtype;

#endif /* JAVA_TYPE_H */
