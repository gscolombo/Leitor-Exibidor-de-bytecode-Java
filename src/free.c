/**
 * @file free.c
 * @brief Implementações das funções responsáveis pela liberação de memória alocada
 *        para estruturas do ClassFile e seus atributos.
 *
 * Este módulo garante que toda memória dinâmica alocada durante o parsing de um
 * arquivo `.class` seja corretamente liberada, incluindo:
 *  - Constant Pool (UTF8 strings e bytes);
 *  - Campos, métodos e atributos;
 *  - Estruturas internas específicas como Code, Exceptions, InnerClasses;
 *  - Subatributos recursivos.
 *
 * O gerenciamento de memória é uma parte crítica do carregador de classes para evitar
 * *memory leaks*, especialmente devido à natureza recursiva da estrutura de atributos.
 */

#include "free.h"
#include <stdio.h>

/**
 * @brief Libera todos os recursos associados a uma estrutura ClassFile.
 *
 * Esta função:
 *  - libera a lista de interfaces;
 *  - libera os atributos associados a cada método e, em seguida, o array de métodos;
 *  - libera os atributos associados a cada campo e, em seguida, o array de campos;
 *  - libera atributos de nível de classe;
 *  - libera strings da constant pool (UTF8);
 *  - libera o array da constant pool;
 *
 * Importante: A função **não** libera a própria estrutura ClassFile, apenas suas áreas
 * internas, pois a estrutura pode ter sido alocada na stack.
 *
 * @param cf Ponteiro para a estrutura ClassFile previamente carregada.
 */
void free_classfile(ClassFile *cf)
{
    cp_info *cp = cf->constant_pool;

    free(cf->interfaces);

    for (size_t i = 0; i < cf->methods_count; i++)
        free_attributes(cp, cf->methods[i].attributes_count, cf->methods[i].attributes);
    free(cf->methods);

    for (size_t i = 0; i < cf->fields_count; i++)
        free_attributes(cp, cf->fields[i].attributes_count, cf->fields[i].attributes);
    free(cf->fields);

    free_attributes(cp, cf->attributes_count, cf->attributes);

    // Liberação específica da Constant Pool — apenas UTF8 tem memória alocada internamente
    for (size_t i = 1; i < (size_t)cf->constant_pool_count - 1; i++)
    {
        if (cp[i].tag == CONSTANT_UTF8)
        {
            free(cp[i].info.UTF8.bytes);
            free(cp[i].info.UTF8.str);
        }
    }

    free(cf->constant_pool);
}

/**
 * @brief Libera recursivamente os atributos de um campo, método ou da própria classe.
 *
 * A função inspeciona o tipo do atributo (via `convert_attr_name`) e executa a rotina
 * adequada de liberação para cada caso:
 *
 * - **Code**
 *   - libera bytecode (`code`);
 *   - libera tabela de exceções (`exception_table`);
 *   - libera atributos internos do Code.
 *
 * - **Exceptions**
 *   - libera a tabela de índices de exceções.
 *
 * - **InnerClasses**
 *   - libera o array de `classes`.
 *
 * Outros atributos conhecidos, porém sem memória dinâmica específica, são simplesmente ignorados.
 *
 * Após tratar cada atributo individualmente, a função libera o array de atributos.
 *
 * @param cp Ponteiro para a Constant Pool usada para resolver nomes dos atributos.
 * @param count Número de atributos no array.
 * @param attr Array de atributos a serem liberados.
 */
void free_attributes(cp_info *cp, u2 count, attribute *attr)
{
    if (count > 0)
        for (u2 i = 0; i < count; i++)
        {
            const attribute_name *attr_name =
                convert_attr_name(cp[attr[i].attribute_name_index - 1].info.UTF8.str);

            if (attr_name != NULL)
                switch (*attr_name)
                {
                case Code:
                    free(attr[i].info.Code.code);
                    free(attr[i].info.Code.exception_table);
                    free_attributes(
                        cp,
                        attr[i].info.Code.attributes_count,
                        attr[i].info.Code.attributes
                    );
                    break;

                case Exceptions:
                    free(attr[i].info.Exceptions.exception_index_table);
                    break;

                case InnerClasses:
                    free(attr[i].info.InnerClasses.classes);
                    break;

                default:
                    break;
                }
        }

    free(attr);
}
