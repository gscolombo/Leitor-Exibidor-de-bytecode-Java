#include "reader.h"
#include "writer.h"
#include "free.h"
#include "bootstrap_loader.h"
#include "interpreter.h"

int main(const int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Número de argumentos inválido.\n");
        printf("Uso: %s <modo> <arquivo.class>\n\n", argv[0]);
        printf("Argumentos necessários:\n");
        printf("  <modo>           --show     Exibe informações do arquivo .class\n");
        printf("                   --execute  Executa o arquivo .class\n");
        printf("  <arquivo.class>  Caminho para o arquivo .class\n\n");
        printf("Exemplo: %s --show programa.class\n", argv[0]);
        return 1;
    }

    if (!strcmp(argv[1], "--execute"))
    {
        /* Interpreter */

        // Initialize method area
        MethodArea method_area;
        method_area.num_classes = 0;
        method_area.ref_count = 0;
        method_area.classes = NULL;
        method_area.refs = NULL;

        // Load and link input class as initial class
        Class *initial_class = bootstrap_loader(argv[2], &method_area, NULL);
        if (initial_class == NULL)
        {
            fprintf(stderr, "Erro: não foi possível carregar a classe '%s'.\n", argv[2]);
            cleanup(method_area);
            return 1;
        }

        // Invoke main method
        Method *main_method = lookup_method("main", "([Ljava/lang/String;)V", initial_class);
        if (!main_method)
        {
            printf("Método \"main\" não encontrado.\n");
            cleanup(method_area);
            return 1;
        }

        dtype *local_vars = (dtype *)calloc(main_method->bytecode.max_locals, sizeof(dtype));
        if (local_vars == NULL)
        {
            fprintf(stderr, "Erro: falha ao alocar memória para variáveis locais.\n");
            cleanup(method_area);
            return 1;
        }
        invoke_method(initial_class, main_method, local_vars, NULL, &method_area);

        // Clean method area
        cleanup(method_area);
    }
    else if (!strcmp(argv[1], "--show"))
    {
        /* Information display */
        FILE *fptr = open_classfile(argv[2]);

        if (fptr != NULL)
        {
            ClassFile cf = read_classfile(fptr, true);
            show_classfile(&cf);
            free_classfile(&cf);
        }
        else
        {
            fprintf(stderr, "Erro: não foi possível abrir o arquivo '%s'.\n", argv[2]);
            return 1;
        }
    }
    else
    {
        printf("Invalid option \"%s\". Choose between --show and --execute.\n", argv[1]);
        return 1;
    }

    return 0;
}