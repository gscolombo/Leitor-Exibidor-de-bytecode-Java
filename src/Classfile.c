/**
 * @file Classfile.c
 * @brief Implementação das funções de liberação de memória associadas a ClassFile.
 *
 * Este módulo implementa a lógica necessária para desalocar todas as estruturas
 * associadas a um arquivo .class carregado em memória, incluindo:
 * - constant pool (UTF-8 strings)
 * - interfaces, fields, methods
 * - atributos de métodos e atributos internos do atributo Code
 *
 * As funções aqui são fundamentais para evitar vazamentos de memória após o uso
 * de `read_classfile()`.
 */

#include "Classfile.h"

/**
 * @brief Libera toda a memória alocada em um `ClassFile`.
 *
 * Esta função percorre e libera:
 * - interfaces
 * - fields
 * - methods e seus atributos (via free_attributes)
 * - entradas do constant pool do tipo UTF8 (bytes + string convertida)
 * - o array de constant_pool
 *
 * Observações:
 * - O constant pool é percorrido a partir do índice 1 porque a JVM define o índice 0
 *   como inválido/unused.
 * - A função **não** libera a própria estrutura `ClassFile` (pois ela pode estar
 *   alocada na stack dependendo de como read_classfile foi chamado).
 *
 * @param cf Ponteiro para o ClassFile a ser desalocado.
 */
void free_classfile(ClassFile *cf)
{
    cp_info *cp = cf->constant_pool;

    /* Libera arrays simples */
    free(cf->interfaces);
    free(cf->fields);

    /* Libera cada método e seus atributos */
    for (size_t i = 0; i < cf->methods_count; i++)
        free_attributes(cp, cf->methods[i].attributes_count, cf->methods[i].attributes);

    free(cf->methods);

    /**
     * Liberação das entradas do constant pool.
     * Apenas CONSTANT_UTF8 contém buffers alocados com malloc().
     *
     * A varredura começa em 1 porque:
     * - Por convenção da JVM, o índice 0 não é usado.
     */
    for (size_t i = 1; i < (size_t)cf->constant_pool_count - 1; i++)
    {
        if (cp[i].tag == CONSTANT_UTF8)
        {
            free(cp[i].info.UTF8.bytes);
            free(cp[i].info.UTF8.str);
        }
    }

    /* Libera o constant pool em si */
    free(cf->constant_pool);
}

/**
 * @brief Libera um array de atributos associado a fields, methods ou ao attribute Code.
 *
 * Esta função analisa o tipo de cada atributo usando `convert_attr_name()` e libera
 * corretamente suas subestruturas:
 *
 * - **Code**:
 *     - code (byte array)
 *     - exception_table
 *     - atributos internos (chamada recursiva de free_attributes)
 *
 * - **LineNumberTable**:
 *     - line_number_table
 *
 * Outros atributos que não possuem estruturas alocadas são ignorados.
 *
 * A função finaliza liberando o próprio array `attr`.
 *
 * @param cp Constant pool associado (necessário para obter o nome de cada atributo).
 * @param count Número de atributos no array.
 * @param attr Ponteiro para o array de atributos a ser liberado.
 */
void free_attributes(cp_info *cp, u2 count, attribute *attr)
{
    if (count > 0)
        for (size_t i = 0; i < count; i++)
        {
            const attribute_name *attr_name =
                convert_attr_name(cp[attr[i].attribute_name_index - 1].info.UTF8.str);

            if (attr_name != NULL)
                switch (*attr_name)
                {
                case Code:
                    /* Libera bloco de bytecode */
                    free(attr[i].info.Code.code);

                    /* Libera tabela de exceções */
                    free(attr[i].info.Code.exception_table);

                    /* Libera atributos internos do Code */
                    free_attributes(
                        cp,
                        attr[i].info.Code.attributes_count,
                        attr[i].info.Code.attributes
                    );
                    break;

                case LineNumberTable:
                    free(attr[i].info.LineNumberTable.line_number_table);
                    break;

                default:
                    /* Outros atributos não possuem estruturas alocadas */
                    break;
                }
        }

    /* Libera o array de atributos */
    free(attr);
}
