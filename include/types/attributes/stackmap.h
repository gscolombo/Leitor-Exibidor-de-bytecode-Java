/**
 * @file include/types/attributes/stackmap.h
 * @brief Estrutura placeholder para frames do atributo StackMapTable.
 *
 * O atributo StackMapTable é usado pela JVM para verificação de tipos (bytecode verification).
 * Ele descreve o estado da pilha e das variáveis locais em pontos específicos do método.
 *
 * A especificação define vários tipos de "stack_map_frame":
 *   - same_frame
 *   - same_locals_1_stack_item_frame
 *   - chop_frame
 *   - append_frame
 *   - full_frame
 *
 * Cada um possui formatação e campos distintos.
 *
 * No entanto, o leitor-exibidor deste projeto **não implementa** suporte completo
 * a StackMapTable, portanto este arquivo declara apenas um tipo vazio,
 * servindo como placeholder para permitir:
 *
 *  - compilação do projeto completo,
 *  - representação mínima do atributo StackMapTable,
 *  - futura extensão caso necessário.
 */

#ifndef TYPES_ATTRIBUTES_STACKMAP_H
#define TYPES_ATTRIBUTES_STACKMAP_H

/**
 * @union stack_map_frame
 * @brief Estrutura placeholder para frames da StackMapTable.
 *
 * A estrutura completa exigiria parsing detalhado da JVM Specification,
 * incluindo diferentes variantes de frames com campos específicos.
 *
 * Como o projeto não utiliza esses dados, o union permanece vazio.
 */
typedef union stack_map_frame
{
    /* Estrutura propositalmente vazia */
} stack_map_frame;

#endif /* TYPES_ATTRIBUTES_STACKMAP_H */
