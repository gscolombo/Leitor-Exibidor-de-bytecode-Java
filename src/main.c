/**
 * @file main.c
 * @brief Programa principal do leitor/executor de arquivos .class Java.
 *
 * Este arquivo contém a rotina \c main que aceita dois modos de operação:
 * - \c --show: abre um arquivo .class, lê sua estrutura e exibe informações.
 * - \c --execute: carrega a classe inicial via bootstrap loader e tenta
 *   invocar o método \c main da classe, executando-o através do interpretador.
 *
 * Uso:
 * @code
 * ./program --show Caminho/Para/Arquivo.class
 * ./program --execute Caminho/Para/Arquivo.class
 * @endcode
 *
 * Observações:
 * - O programa depende de diversos módulos: reader, writer, free, bootstrap_loader e interpreter.
 * - Mensagens de erro simples são impressas em stdout/stderr conforme a falha.
 *
 * @author
 * @date
 */

#include "reader.h"
#include "writer.h"
#include "free.h"
#include "bootstrap_loader.h"
#include "interpreter.h"

/**
 * @brief Ponto de entrada do programa.
 *
 * Dependendo do primeiro argumento, executa o interpretador ou exibe informações
 * do arquivo .class passado no segundo argumento.
 *
 * Modos de operação:
 * - "--execute": inicializa a MethodArea, carrega a classe inicial via
 *   bootstrap_loader(), busca o método "main" com assinatura "([Ljava/lang/String;)V"
 *   e o invoca com invoke_method(). Ao final, realiza cleanup da MethodArea.
 * - "--show": abre o arquivo .class com open_classfile(), o lê com read_classfile()
 *   e exibe suas informações com show_classfile(); por fim libera a estrutura com free_classfile().
 *
 * @param argc Número de argumentos da linha de comando.
 * @param argv Vetor de strings com os argumentos. argv[1] deve ser o modo e argv[2] o caminho para o .class.
 * @return 0 em sucesso; 1 em caso de argumentos inválidos ou opção desconhecida.
 */
int main(const int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Invalid number of arguments."
               "Pass the mode of operation (--show or --execute) along with the path for a .class file.\n");
        return 1;
    }

    if (!strcmp(argv[1], "--execute"))
    {
        /* Interpreter */

        /* Inicializa a MethodArea usada pelo interpretador.
         *
         * @note A estrutura MethodArea deve ser definida em algum cabeçalho incluído
         *       (provavelmente bootstrap_loader.h ou interpreter.h). Aqui apenas
         *       inicializamos seus campos básicos antes de passar para o carregador.
         */
        MethodArea method_area;
        method_area.num_classes = 0;
        method_area.ref_count = 0;
        method_area.classes = (Class *)malloc(sizeof(Class));
        if (method_area.classes == NULL)
            exit(1);

        /* Carrega e faz o link da classe de entrada como classe inicial.
         * bootstrap_loader retorna um ponteiro para a estrutura Class carregada.
         */
        Class *initial_class = bootstrap_loader(argv[2], &method_area, NULL);
        if (initial_class != NULL)
        {
            /* Procura pelo método main na classe inicial.
             *
             * A assinatura usada é "([Ljava/lang/String;)V" conforme padrão Java para public static void main(String[]).
             * Se o método não for encontrado, imprime mensagem e encerra com erro.
             */
            Method *main_method = lookup_method("main", "([Ljava/lang/String;)V", initial_class);
            if (!main_method) {
                printf("Método \"main\" não encontrado.\n");
                exit(1);
            }

            /* Aloca vetor de variáveis locais conforme o número máximo de locais exigido pelo bytecode do método.
             * dtype é o tipo usado para representar valores locais e operandos no interpretador (definido em outro módulo).
             */
            dtype *local_vars = (dtype *)calloc(main_method->bytecode.max_locals, sizeof(dtype));

            /* Invoca o método main da classe inicial.
             *
             * Parâmetros passados:
             * - initial_class: classe que contém o método a ser invocado.
             * - main_method: ponteiro para a estrutura Method representando main.
             * - local_vars: espaço para variáveis locais (inicialmente zerado).
             * - NULL: (provavelmente) argumentos adicionais (por exemplo args) — aqui passado NULL.
             * - &method_area: referência à MethodArea para resolução de classes, métodos e campos durante a execução.
             */
            invoke_method(initial_class, main_method, local_vars, NULL, &method_area);
        }

        /* Limpeza da MethodArea (liberação de memória, referências, etc.). */
        cleanup(method_area);
    }
    else if (!strcmp(argv[1], "--show"))
    {
        /* Information display */

        /* Abre o arquivo .class para leitura. open_classfile deve retornar um FILE* */
        FILE *fptr = open_classfile(argv[2]);

        if (fptr != NULL)
        {
            /* Lê a estrutura do ClassFile do disco para memória.
             * O segundo argumento 'true' indica (provavelmente) que a leitura deve
             * carregar também atributos e informações detalhadas necessárias para exibição.
             */
            ClassFile cf = read_classfile(fptr, true);

            /* Exibe informações do .class lido em um formato legível */
            show_classfile(&cf);

            /* Libera memória alocada para a estrutura ClassFile */
            free_classfile(&cf);
        }
    }
    else
    {
        printf("Invalid option \"%s\". Choose between --show and --execute.\n", argv[1]);
        return 1;
    }

    return 0;
}
