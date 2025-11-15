#include "utils.h"
#include "reader.h"
#include "writer.h"
#include "free.h"
#include "bootstrap_loader.h"
#include "MethodArea.h"
#include "Frame.h"
#include "interpreter.h"

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

        // Initialize method area
        MethodArea method_area;
        method_area.num_classes = 0;
        method_area.classes = (Class *)malloc(sizeof(Class));
        if (method_area.classes == NULL)
            exit(1);

        // Load and link input class as initial class
        Class *initial_class = bootstrap_loader(argv[2], &method_area, NULL);
        if (initial_class != NULL)
        {
            // Invoke main method
            Method *main_method = lookup_method("main", initial_class);
            if (!main_method) {
                printf("Método \"main\" não encontrado.\n");
                exit(1);
            }

            invoke_method(initial_class, main_method, NULL, NULL, &method_area);
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