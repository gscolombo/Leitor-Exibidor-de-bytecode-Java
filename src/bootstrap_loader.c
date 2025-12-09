#include "bootstrap_loader.h"
#include <string.h>

static char ROOT_FOLDER[2048];

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