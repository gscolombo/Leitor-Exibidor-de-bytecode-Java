/**
 * @file include/types/attributes/annotation.h
 * @brief Estruturas relacionadas a anotações presentes em atributos da JVM.
 *
 * A especificação da JVM define estruturas detalhadas para representar
 * anotações (annotations) e valores de elementos (element_value), utilizadas por
 * atributos como:
 *
 *  - RuntimeVisibleAnnotations
 *  - RuntimeInvisibleAnnotations
 *  - RuntimeVisibleParameterAnnotations
 *  - RuntimeInvisibleParameterAnnotations
 *  - AnnotationDefault
 *
 * No entanto, neste projeto, essas estruturas são deixadas vazias porque:
 *  - As anotações **não são usadas pelo leitor-exibidor** atual,
 *  - Sua implementação completa é extensa e não necessária para os objetivos do trabalho,
 *  - O parser não lida com esses atributos em profundidade.
 *
 * Este arquivo existe para manter consistência com a estrutura formal da JVM e
 * permitir futura extensão do parser caso seja necessário suportar anotações.
 */

#ifndef TYPES_ATTRIBUTES_ANNOTATION_H
#define TYPES_ATTRIBUTES_ANNOTATION_H

/**
 * @struct annotation
 * @brief Estrutura placeholder para representar uma anotação da JVM.
 *
 * A forma completa incluiria:
 *  - type_index (u2)
 *  - num_element_value_pairs
 *  - lista de element_value_pairs
 *
 * Atualmente vazio porque o projeto não implementa parsing de anotações.
 */
typedef struct annotation
{
    /* Estrutura propositalmente vazia — não utilizada neste projeto. */
} annotation;

/**
 * @struct element_value
 * @brief Estrutura placeholder para representar valores de elementos dentro de anotações.
 *
 * Na JVM real, cada element_value possui:
 *  - um tag (letra indicando o tipo)
 *  - conteúdos variáveis conforme o tipo (const_value_index, enum_const_name, annotation_value, array_value)
 *
 * Essa complexidade é omitida aqui por não ser necessária ao leitor/exibidor atual.
 */
typedef struct element_value
{
    /* Estrutura propositalmente vazia — não utilizada neste projeto. */
} element_value;

#endif /* TYPES_ATTRIBUTES_ANNOTATION_H */
