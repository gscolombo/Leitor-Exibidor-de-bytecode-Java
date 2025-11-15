#include "bootstrap_loader.h"

static char *ROOT_FOLDER;

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
        path = strcat(ROOT_FOLDER, "/");
        path = strcat(path, class_name);
        path = strcat(path, ".class");
        cls = create_and_load_class(path);

        // ...then add to method area
        method_area->classes = (Class *)realloc(method_area->classes, sizeof(method_area->classes) + sizeof(Class));

        if (method_area->classes != NULL)
        {
            method_area->num_classes++;
            size_t i = method_area->num_classes - 1;

            method_area->classes[i] = *cls; free(cls);
            return &method_area->classes[i];
        }
    }
    else // Load initial class
    {
        cls = create_and_load_class(path);

        ROOT_FOLDER = strtok(path, "/");

        if (method_area->classes != NULL)
        {
            method_area->num_classes = 1;
            method_area->classes[0] = *cls; free(cls);
            return &method_area->classes[0];
        }
    }

    return NULL;
}