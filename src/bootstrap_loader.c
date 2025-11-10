#include "bootstrap_loader.h"
#include "writer.h"

static char *ROOT_FOLDER;

ClassFile *bootstrap_loader(char *path, MethodArea *method_area, const char *class_name)
{
    // Check if method area has classes
    if (method_area->classes != NULL)
    {
        // Search for class in method area
        for (size_t i = 0; i < method_area->num_classes; i++)
        {
            char *loaded_cls_name = get_constant_UTF8_value(method_area->classes[i].this_class,
                                                            method_area->classes[i].constant_pool);
            if (!strcmp(loaded_cls_name, class_name))
            {
                free(loaded_cls_name);
                return &method_area->classes[i];
            }
            free(loaded_cls_name);
        }

        path = strcat(ROOT_FOLDER, "/");
        path = strcat(path, class_name);
        path = strcat(path, ".class");
        FILE *fptr = open_classfile(path);
        ClassFile class = read_classfile(fptr, false);

        method_area->classes = (ClassFile *)realloc(method_area->classes, sizeof(method_area->classes) + sizeof(class));

        if (method_area->classes != NULL)
        {
            method_area->num_classes++;
            method_area->classes[method_area->num_classes - 1] = class;
            return method_area->classes;
        }
    }
    else // Load initial class
    {
        FILE *fptr = open_classfile(path);
        ClassFile class = read_classfile(fptr, false);

        ROOT_FOLDER = strtok(path, "/");

        method_area->classes = (ClassFile *)malloc(sizeof(ClassFile));
        if (method_area->classes != NULL)
        {
            method_area->num_classes = 1;
            method_area->classes[0] = class;
            return method_area->classes;
        }
    }

    return NULL;
}