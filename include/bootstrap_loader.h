#ifndef BOOTSTRAP_LOADER_H
#define BOOTSTRAP_LOADER_H

#include <stdio.h>

#include "Classfile.h"
#include "reader.h"
#include "MethodArea.h"
#include "method_area_utils.h"

/**
 * @file bootstrap_loader.h
 * @brief Declaração do carregador bootstrap e utilitários relacionados ao carregamento de classes.
 *
 * O bootstrap loader é responsável por carregar uma classe inicial (geralmente a fornecida
 * como entrada) e suas dependências básicas, realizando etapas de carregamento e linking
 * mínimas necessárias para que o interpretador invoque métodos dessa classe.
 */

/**
 * @brief Carrega uma classe pelo caminho/nome fornecido e a insere na MethodArea.
 *
 * A função realiza:
 * - localização do arquivo .class correspondente ao nome fornecido;
 * - leitura do ClassFile via read_classfile();
 * - criação/instanciação da estrutura `Class` interna a partir do `ClassFile`;
 * - registro da classe na `MethodArea` passada como argumento;
 * - resolução mínima (por exemplo, vinculação de constant pool) necessária antes da execução.
 *
 * @param class_path Caminho ou nome interno da classe a ser carregada (por exemplo "MyClass" ou "com/example/MyClass").
 *                   O formato aceito depende da implementação do projeto (pode esperar ".class" no final ou realizar mapeamento).
 * @param method_area Ponteiro para a `MethodArea` onde a classe será registrada.
 * @param parent Nome da classe pai/carregador solicitado ou contexto adicional (pode ser NULL).
 * @return Ponteiro para a estrutura `Class` carregada e registrada na `MethodArea`, ou NULL em caso de falha.
 *
 * @note A função pode encerrar o processo em falhas críticas (por exemplo, arquivo .class inválido), conforme a política do projeto.
 */
Class *bootstrap_loader(char *class_path, MethodArea *method_area, const char *parent);

#endif /* BOOTSTRAP_LOADER_H */
