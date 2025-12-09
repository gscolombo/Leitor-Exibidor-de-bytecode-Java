/**
 * @file bootstrap_loader.c
 * @brief Implementação do carregador bootstrap (carregamento inicial de classes).
 *
 * Este módulo implementa um carregador simples que busca classes na Method Area
 * e, caso não estejam presentes, carrega o arquivo `.class` do disco, converte
 * para a representação interna (`Class`) e registra a classe na Method Area.
 *
 * Comportamento resumido:
 *  - Se a classe já estiver presente em `method_area`, retorna um ponteiro para ela.
 *  - Caso contrário, monta o caminho para o arquivo `.class` dentro do diretório
 *    raiz (ROOT_FOLDER) e utiliza `create_and_load_class()` para ler e construir
 *    a estrutura `Class`.
 *  - Adiciona a classe carregada ao array `method_area->classes` (usando `realloc`)
 *    e atualiza `method_area->num_classes`.
 *
 * Observações importantes:
 *  - `ROOT_FOLDER` é uma variável estática que é inicializada apenas na primeira
 *    chamada que carrega a *classe inicial* (através de `strtok(path, "/")`).
 *    Ela passa a ser usada para construir caminhos relativos de classes subsequentes.
 *  - A função realiza alocação/rerealocação do array `method_area->classes`.
 *    Em caso de sucesso o ponteiro retornado aponta para o elemento dentro deste array.
 *  - Há um ponto a observar: o código define um `char path[255]` local e também recebe
 *    um parâmetro chamado `path` (char *path). Isso causa *shadowing* do identificador
 *    `path` e pode levar a comportamentos inesperados ou confusos. Recomenda-se renomear
 *    o buffer local para evitar colisões (por exemplo, `buf` ou `class_path`).
 */

#include "bootstrap_loader.h"
#include <string.h>

static char ROOT_FOLDER[2048];

/**
 * @brief Carrega (ou recupera) uma classe pelo nome usando a Method Area.
 *
 * Se a classe já estiver presente em `method_area`, retorna um ponteiro para ela.
 * Caso contrário, monta o caminho para o arquivo `.class` em ROOT_FOLDER,
 * chama `create_and_load_class()` para ler/parsear o arquivo, registra a classe
 * na `method_area` (realloc) e retorna um ponteiro para a entrada recém-inserida.
 *
 * Na primeira chamada (quando `method_area->num_classes` é zero) a função carrega
 * a classe passada em `path` (parâmetro), inicializa `ROOT_FOLDER` usando `strtok`
 * sobre o caminho recebido (obtendo o diretório raiz) e registra a primeira classe.
 *
 * @param path Caminho para o arquivo `.class` a ser carregado na invocação inicial
 *             (ex.: "/algum/dir/com/package/Foo.class") ou caminho/identificador usado
 *             pelo chamador conforme a convenção do projeto.
 * @param method_area Ponteiro para a estrutura MethodArea onde as classes carregadas
 *                    serão registradas. O array `method_area->classes` pode ser realocado.
 * @param class_name Nome da classe no formato esperado pelo carregador (ex.: "mypkg/Foo").
 * @return Ponteiro para a `Class` encontrada ou carregada e armazenada na Method Area,
 *         ou NULL em caso de falha (por exemplo, falha de alocação ou arquivo inexistente).
 *
 * @note O ponteiro retornado aponta para a memória gerenciada por `method_area->classes`.
 *       O chamador não deve liberar esse ponteiro individualmente; em vez disso deve
 *       usar `cleanup()`/rotinas apropriadas que liberem toda a Method Area.
 *
 * @warning O uso de `strtok(path, "/")` modifica a string `path` passada como parâmetro.
 *          Logo, o chamador deve fornecer um buffer mutável quando invocar a função
 *          para a primeira carga (ou aceitar que `path` seja alterado).
 * @warning Existe shadowing do identificador `path`: a função declara um buffer local
 *          `char path[255];` e também possui um parâmetro `char *path`. Isso pode
 *          provocar comportamento incorreto ou confusão — recomenda-se renomear o
 *          buffer local para evitar colisões de identificador.
 */
Class *bootstrap_loader(char *path, MethodArea *method_area, const char *class_name)
{
    Class *cls;

    // Check if method area has classes
    if (method_area->num_classes > 0)
    {
        // Search for class in method area
        if ((cls = lookup_class(class_name, method_area)) != NULL)
            return cls;

        // Else, load and parse classfile...
        if (ROOT_FOLDER[0] == '\0')
        {
            fprintf(stderr, "Erro: ROOT_FOLDER não foi inicializado.\n");
            return NULL;
        }

        char class_path[2048];
        int ret = snprintf(class_path, sizeof(class_path), "%s/%s.class", ROOT_FOLDER, class_name);
        
        // Check for truncation
        if (ret < 0 || (size_t)ret >= sizeof(class_path))
        {
            fprintf(stderr, "Erro: caminho da classe muito longo.\n");
            return NULL;
        }

        cls = create_and_load_class(class_path);
        if (cls == NULL)
            return NULL;

        // ...then add to method area
        method_area->classes = (Class *)realloc(method_area->classes, sizeof(Class) * (1 + method_area->num_classes));

        if (method_area->classes != NULL)
        {
            method_area->num_classes++;
            size_t i = method_area->num_classes - 1;

            method_area->classes[i] = *cls;
            free(cls);
            return &method_area->classes[i];
        }
        else
        {
            free(cls);
            return NULL;
        }
    }
    else // Load initial class
    {
        cls = create_and_load_class(path);
        if (cls == NULL)
            return NULL;

        // Extract root folder from path (copy before strtok modifies it)
        char path_copy[2048];
        strncpy(path_copy, path, sizeof(path_copy) - 1);
        path_copy[sizeof(path_copy) - 1] = '\0';
        
        char *last_slash = strrchr(path_copy, '/');
        if (last_slash != NULL)
        {
            *last_slash = '\0';
            strncpy(ROOT_FOLDER, path_copy, sizeof(ROOT_FOLDER) - 1);
            ROOT_FOLDER[sizeof(ROOT_FOLDER) - 1] = '\0';
        }
        else
        {
            // No slash found, use current directory
            ROOT_FOLDER[0] = '.';
            ROOT_FOLDER[1] = '\0';
        }

        // Allocate classes array if not already allocated
        if (method_area->classes == NULL)
        {
            method_area->classes = (Class *)malloc(sizeof(Class));
            if (method_area->classes == NULL)
            {
                free(cls);
                return NULL;
            }
        }

        method_area->num_classes = 1;
        method_area->classes[0] = *cls;
        free(cls);
        return &method_area->classes[0];
    }
}
