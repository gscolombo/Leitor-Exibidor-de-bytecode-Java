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
        method_area.classes = (Class *)malloc(sizeof(Class));
        if (method_area.classes == NULL)
            exit(1);

        // Load and link input class as initial class
        Class *initial_class = bootstrap_loader(argv[2], &method_area, NULL);
        if (initial_class != NULL)
        {
            // Invoke main method
            Method *main_method = lookup_method("main", "([Ljava/lang/String;)V", initial_class);
            if (!main_method)
            {
                printf("Método \"main\" não encontrado.\n");
                exit(1);
            }

            dtype *local_vars = (dtype *)calloc(main_method->bytecode.max_locals, sizeof(dtype));
            invoke_method(initial_class, main_method, local_vars, NULL, &method_area);
        }

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
    }
    else
    {
        printf("Invalid option \"%s\". Choose between --show and --execute.\n", argv[1]);
        return 1;
    }

    return 0;
}