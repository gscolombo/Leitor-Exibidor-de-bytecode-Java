#include "utils.h"
#include "reader.h"
#include "writer.h"
#include "free.h"
#include "bootstrap_loader.h"
#include "MethodArea.h"
#include "Thread.h"
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
        MethodArea *method_area = (MethodArea *)malloc(sizeof(MethodArea));
        method_area->num_classes = 0;
        method_area->classes = NULL;

        // Load and link input class as initial class
        ClassFile *initial_class = bootstrap_loader(argv[2], method_area, NULL);
        if (initial_class != NULL)
        {
            // Find <init> method of initial class
            member_info *init_method = find_method(initial_class, "<init>");
            if (init_method != NULL)
            {
                // Start main thread
                Thread *main_thread = initialize_thread(method_area);

                java_type *local_variables = (java_type *)calloc(init_method->attributes->info.Code.max_locals, sizeof(java_type));
                local_variables[0].ref.object_ref = initial_class;

                // Initialize initial class
                invoke_method(main_thread, initial_class, init_method, local_variables, NULL);

                member_info *main_method = find_method(initial_class, "main");
                if (main_method != NULL)
                    invoke_method(main_thread, initial_class, main_method, NULL, NULL);

                // Cleanup
                free_thread(main_thread);
            }
        }
        for (size_t i = 0; i < method_area->num_classes; i++)
            free_classfile(&method_area->classes[i]);

        free(method_area->classes);
        free(method_area);
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