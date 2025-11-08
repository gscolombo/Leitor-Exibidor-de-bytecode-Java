#include "bootstrap_loader.h"
#include "writer.h"

ClassFile *bootstrap_loader(const char *path, MethodArea *method_area)
{
    // Load class from classfile
    FILE *fptr = open_classfile(path);
    ClassFile class = read_classfile(fptr, false);

    // Check if method area has classes
    if (method_area->classes != NULL)
    {
        const char *class_name = get_constant_UTF8_value(class.this_class, class.constant_pool);
        for (size_t i = 0; i < method_area->num_classes; i++)
        {
            const char *loaded_cls_name = get_constant_UTF8_value(method_area->classes[i].this_class, method_area->classes[i].constant_pool);
            if (!strcmp(loaded_cls_name, class_name))
                return &method_area->classes[i];
        }

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