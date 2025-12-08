/**
 * @file method_area_utils.h
 * @brief Funções utilitárias relacionadas à Method Area e à resolução de classes, métodos e campos.
 *
 * Este módulo fornece operações fundamentais para o interpretador da JVM simplificada,
 * incluindo busca de classes já carregadas, resolução de métodos e campos declarados,
 * carregamento dinâmico de classes via bootstrap loader e limpeza da Method Area ao final da execução.
 *
 * As funções aqui definidas implementam parte do comportamento descrito na especificação
 * da JVM para a etapa de *linking* e *method resolution*.
 */

#ifndef METHOD_AREA_UTILS_H
#define METHOD_AREA_UTILS_H

#include <string.h>

#include "reader.h"
#include "cp/writer.h"
#include "MethodArea.h"
#include "bytecode/utils.h"
#include "free.h"

/**
 * @brief Busca uma classe já carregada na Method Area.
 *
 * A busca compara o nome interno da classe (por exemplo `"java/lang/String"`)
 * com o nome passado. Caso a classe esteja carregada, seu ponteiro é retornado.
 *
 * @param class_name Nome da classe no formato interno da JVM.
 * @param method_area Estrutura contendo todas as classes carregadas no momento.
 * @return Ponteiro para a classe encontrada, ou NULL caso não esteja na Method Area.
 */
Class *lookup_class(const char *class_name, const MethodArea *method_area);

/**
 * @brief Busca um método específico em uma classe.
 *
 * Realiza comparação por nome e descritor, conforme a convenção da JVM:
 * - `method_name`: nome do método (ex.: `"main"`, `"toString"`)
 * - `descriptor`: assinatura do método (ex.: `"(I)V"`, `"([Ljava/lang/String;)V"`)
 *
 * A busca é feita somente na classe fornecida, não em superclasses.
 *
 * @param method_name Nome do método procurado.
 * @param descriptor Descritor conforme a especificação da JVM.
 * @param cls Ponteiro para a classe onde o método será procurado.
 * @return Ponteiro para a estrutura `Method` encontrada, ou NULL se o método não existir.
 */
Method *lookup_method(const char *method_name, const char *descriptor, const Class *cls);

/**
 * @brief Busca um campo declarado em uma classe.
 *
 * A resolução é feita com base no nome e descritor do campo. Apenas os campos
 * diretamente definidos na classe são verificados (não realiza lookup em superclasses).
 *
 * @param field_name Nome do campo procurado.
 * @param descriptor Tipo descrito no formato da JVM (ex.: `"I"`, `"Ljava/lang/String;"`).
 * @param cls Classe na qual o campo será procurado.
 * @return Ponteiro para estrutura `Field` correspondente ao campo, ou NULL se não encontrado.
 */
Field *lookup_field(const char *field_name, const char *descriptor, const Class *cls);

/**
 * @brief Cria uma nova entrada `Class`, carrega seu arquivo `.class` e retorna a instância carregada.
 *
 * Essa função encapsula o processo de:
 * - Criação da estrutura `Class`.
 * - Leitura do arquivo `.class` correspondente.
 * - Preenchimento de campos internos.
 *
 * É tipicamente usada pelo bootstrap loader ou para carregamento dinâmico de classes
 * durante a execução.
 *
 * @param class_name Caminho/nome interno da classe a ser carregada.
 * @return Ponteiro para a estrutura `Class` recém-criada e carregada.
 */
Class *create_and_load_class(const char *class_name);

/**
 * @brief Libera todas as classes carregadas na Method Area e suas dependências.
 *
 * Realiza a limpeza completa dos dados armazenados, liberando:
 * - Constant pool
 * - Fields
 * - Methods
 * - Atributos
 * - Arrays alocados
 *
 * Usado ao final da execução do interpretador para prevenir vazamentos de memória.
 *
 * @param method_area Estrutura contendo todas as classes carregadas.
 */
void cleanup(MethodArea method_area);

#endif /* METHOD_AREA_UTILS_H */
