#include "method_area_utils.h"

Class *lookup_class(const char *class_name, const MethodArea *method_area)
{
    for (u2 i = 0; i < method_area->num_classes; i++)
    {
        if (!strcmp(class_name, method_area->classes[i].name))
            return &method_area->classes[i];
    }

    return NULL;
}

Method *lookup_method(const char *method_name, const Class *class)
{
    for (u2 i = 0; i < class->method_count; i++)
    {
        if (!strcmp(method_name, class->methods[i].name))
            return &class->methods[i];
    }

    return NULL;
}

Class *create_and_load_class(const char *path)
{
    // Open and parse classfile
    FILE *fptr = open_classfile(path);
    ClassFile cf = read_classfile(fptr, false);

    // Create class
    Class *cls = (Class *)malloc(sizeof(Class));

    cls->name = get_constant_UTF8_value(cf.this_class, cf.constant_pool);
    cls->super = get_constant_UTF8_value(cf.super_class, cf.constant_pool);
    cls->super_index = cf.super_class;
    cls->constants_count = cf.constant_pool_count;

    cls->runtime_cp = (RuntimeConstant *)calloc(cf.constant_pool_count, sizeof(RuntimeConstant));
    for (u2 i = 0; i < cf.constant_pool_count - 1; i++)
    {
        cls->runtime_cp[i].type = cf.constant_pool[i].tag;

        switch (cf.constant_pool[i].tag)
        {
        case CONSTANT_NameAndType:
        case CONSTANT_UTF8:
            break;
        case CONSTANT_Integer:
            cls->runtime_cp[i].value.i = cf.constant_pool[i].info._4Bn.number.i;
            break;
        case CONSTANT_Float:
            cls->runtime_cp[i].value.f = cf.constant_pool[i].info._4Bn.number.f;
            break;
        case CONSTANT_Long:
            cls->runtime_cp[i].value.l = cf.constant_pool[i].info._8Bn.number.l;
            break;
        case CONSTANT_Double:
            cls->runtime_cp[i].value.d = cf.constant_pool[i].info._8Bn.number.d;
            break;
        default:
            cls->runtime_cp[i].value.strref = get_constant_UTF8_value(i + 1, cf.constant_pool);
            break;
        }
    }

    cls->field_count = cf.fields_count;
    cls->fields = (Field *)calloc(cf.fields_count, sizeof(Field));

    for (u2 i = 0; i < cf.fields_count; i++)
    {
        cls->fields[i].name = get_constant_UTF8_value(cf.fields[i].name_index, cf.constant_pool);
        cls->fields[i].type = get_constant_UTF8_value(cf.fields[i].descriptor_index, cf.constant_pool);
        cls->fields[i].access_flags = cf.fields[i].access_flags;
    }

    cls->method_count = cf.methods_count;
    cls->methods = (Method *)calloc(cf.methods_count, sizeof(Method));

    for (u2 i = 0; i < cf.methods_count; i++)
    {
        cls->methods[i].name = get_constant_UTF8_value(cf.methods[i].name_index, cf.constant_pool);

        char *descriptor = get_constant_UTF8_value(cf.methods[i].descriptor_index, cf.constant_pool);
        cls->methods[i].params = strtok(descriptor, ":");
        cls->methods[i].rettype = strtok(NULL, ":");

        cls->methods[i].access_flags = cf.methods[i].access_flags;
        cls->methods[i].bytecode.max_locals = cf.methods[i].attributes->info.Code.max_locals;
        cls->methods[i].bytecode.max_stack = cf.methods[i].attributes->info.Code.max_stack;
        cls->methods[i].bytecode.code_length = cf.methods[i].attributes->info.Code.code_length;

        cls->methods[i].bytecode.code = (u1 *)malloc(cf.methods[i].attributes->info.Code.code_length);
        memcpy(cls->methods[i].bytecode.code, cf.methods[i].attributes->info.Code.code, cf.methods[i].attributes->info.Code.code_length);
    }

    free_classfile(&cf);

    return cls;
}

void cleanup(MethodArea method_area)
{
    for (size_t i = 0; i < method_area.num_classes; i++)
    {
        free(method_area.classes[i].name);
        free(method_area.classes[i].super);

        for (size_t j = 0; j < method_area.classes[i].constants_count; j++)
            free(method_area.classes[i].runtime_cp[j].value.strref);
        free(method_area.classes[i].runtime_cp);

        for (u2 j = 0; j < method_area.classes[i].field_count; j++)
        {
            free(method_area.classes[i].fields[j].name);
            free(method_area.classes[i].fields[j].type);
        }
        free(method_area.classes[i].fields);

        for (u2 j = 0; j < method_area.classes[i].method_count; j++)
        {
            free(method_area.classes[i].methods[j].name);
            free(method_area.classes[i].methods[j].params);
            free(method_area.classes[i].methods[j].rettype);
            free(method_area.classes[i].methods[j].bytecode.code);
        }
        free(method_area.classes[i].methods);
    }
    free(method_area.classes);
}