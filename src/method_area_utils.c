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

Method *lookup_method(const char *method_name, const char *method_descriptor, const Class *class)
{
    for (u2 i = 0; i < class->method_count; i++)
    {
        if (!strcmp(method_name, class->methods[i].name) &&
            !strcmp(method_descriptor, class->methods[i].descriptor))
            return &class->methods[i];
    }

    return NULL;
}

Field *lookup_field(const char *field_name, const char *field_descriptor, const Class *class)
{
    for (u2 i = 0; i < class->field_count; i++)
    {
        if (!strcmp(field_name, class->fields[i].name) &&
            !strcmp(field_descriptor, class->fields[i].type))
            return &class->fields[i];
    }

    return NULL;
}

static void set_runtimecp(Class *cls, ClassFile *cf)
{
    cls->constants_count = cf->constant_pool_count;
    cls->runtime_cp = (RuntimeConstant *)calloc(cf->constant_pool_count, sizeof(RuntimeConstant));

    for (u2 i = 0; i < cf->constant_pool_count - 1; i++)
    {
        cls->runtime_cp[i].type = cf->constant_pool[i].tag;

        switch (cf->constant_pool[i].tag)
        {
        case CONSTANT_NameAndType:
        case CONSTANT_UTF8:
            break;
        case CONSTANT_Integer:
            cls->runtime_cp[i].value.i = cf->constant_pool[i].info._4Bn.number.i;
            break;
        case CONSTANT_Float:
            cls->runtime_cp[i].value.f = cf->constant_pool[i].info._4Bn.number.f;
            break;
        case CONSTANT_Long:
            cls->runtime_cp[i].value.l = cf->constant_pool[i].info._8Bn.number.l;
            i++;
            break;
        case CONSTANT_Double:
            cls->runtime_cp[i].value.d = cf->constant_pool[i].info._8Bn.number.d;
            i++;
            break;
        default:
            cls->runtime_cp[i].value.strref = get_constant_UTF8_value(i + 1, cf->constant_pool);
            break;
        }
    }
}

static void set_class_fields(Class *cls, ClassFile *cf)
{
    cls->field_count = cf->fields_count;
    cls->fields = (Field *)calloc(cf->fields_count, sizeof(Field));

    for (u2 i = 0; i < cf->fields_count; i++)
    {
        cls->fields[i].name = get_constant_UTF8_value(cf->fields[i].name_index, cf->constant_pool);
        cls->fields[i].type = get_constant_UTF8_value(cf->fields[i].descriptor_index, cf->constant_pool);
        cls->fields[i].access_flags = cf->fields[i].access_flags;
        cls->fields[i].attrs = cf->fields[i].attributes;
    }
}

static u2 count_args(char *params)
{
    u2 nargs = 0;
    char *p = params + 1; // Skip first parenthesis

    while (*p != ')')
    {
        switch (*p)
        {
        case '[':
            break;
        case 'L':
            nargs++;
            while (*p != ';')
                p++;
            break;
        default:
            nargs++;
            break;
        }
        p++;
    }

    return nargs;
}

static void set_class_methods(Class *cls, ClassFile *cf)
{
    cls->method_count = cf->methods_count;
    cls->methods = (Method *)calloc(cf->methods_count, sizeof(Method));

    for (u2 i = 0; i < cf->methods_count; i++)
    {
        cls->methods[i].name = get_constant_UTF8_value(cf->methods[i].name_index, cf->constant_pool);
        cls->methods[i].descriptor = get_constant_UTF8_value(cf->methods[i].descriptor_index, cf->constant_pool);

        size_t lparams = 1;
        char *desc = cls->methods[i].descriptor;
        while (*desc++ != ')')
            lparams++;

        size_t lret = strlen(cls->methods[i].descriptor) - lparams;

        cls->methods[i].params = (char *)calloc((lparams + 1), sizeof(char));
        cls->methods[i].rettype = (char *)calloc((lret + 1), sizeof(char));

        if (cls->methods[i].params && cls->methods[i].rettype)
        {
            strncpy(cls->methods[i].params, cls->methods[i].descriptor, lparams);
            strncpy(cls->methods[i].rettype, cls->methods[i].descriptor + lparams, lret);
        }

        cls->methods[i].access_flags = cf->methods[i].access_flags;
        cls->methods[i].bytecode.nargs = count_args(cls->methods[i].params);

        if (!(cf->methods[i].access_flags & 0x0400))
        {
            cls->methods[i].bytecode.max_locals = cf->methods[i].attributes->info.Code.max_locals;
            cls->methods[i].bytecode.max_stack = cf->methods[i].attributes->info.Code.max_stack;
            cls->methods[i].bytecode.code_length = cf->methods[i].attributes->info.Code.code_length;

            cls->methods[i].bytecode.code = (u1 *)malloc(cf->methods[i].attributes->info.Code.code_length);
            memcpy(cls->methods[i].bytecode.code, cf->methods[i].attributes->info.Code.code, cf->methods[i].attributes->info.Code.code_length);
        }
    }
}

Class *create_and_load_class(const char *path)
{
    // Open and parse classfile
    FILE *fptr = open_classfile(path);
    if (fptr == NULL)
    {
        // open_classfile already prints error message
        return NULL;
    }
    ClassFile cf = read_classfile(fptr, false);

    // Create class
    Class *cls = (Class *)malloc(sizeof(Class));

    cls->name = get_constant_UTF8_value(cf.this_class, cf.constant_pool);
    cls->super = get_constant_UTF8_value(cf.super_class, cf.constant_pool);
    cls->super_index = cf.super_class;

    set_runtimecp(cls, &cf);
    set_class_fields(cls, &cf);
    set_class_methods(cls, &cf);

    free_classfile(&cf);

    return cls;
}

void cleanup(MethodArea method_area)
{
    // Clean up references
    if (method_area.refs != NULL)
    {
        for (size_t i = 0; i < method_area.ref_count; i++)
        {
            if (method_area.refs[i] != NULL)
                free(method_area.refs[i]);
        }
        free(method_area.refs);
    }

    // Clean up classes
    if (method_area.classes != NULL)
    {
        for (size_t i = 0; i < method_area.num_classes; i++)
        {
            Class cls = method_area.classes[i];
            
            if (cls.name != NULL)
                free(cls.name);
            if (cls.super != NULL)
                free(cls.super);

            if (cls.runtime_cp != NULL)
            {
                for (size_t j = 0; j < cls.constants_count; j++)
                {
                    if ((cls.runtime_cp[j].type >= 7 && cls.runtime_cp[j].type <= 11) ||
                        cls.runtime_cp[j].type >= 15)
                    {
                        if (cls.runtime_cp[j].value.strref != NULL)
                            free(cls.runtime_cp[j].value.strref);
                    }
                }
                free(cls.runtime_cp);
            }

            if (cls.fields != NULL)
            {
                for (u2 j = 0; j < cls.field_count; j++)
                {
                    if (cls.fields[j].name != NULL)
                        free(cls.fields[j].name);
                    if (cls.fields[j].type != NULL)
                        free(cls.fields[j].type);
                }
                free(cls.fields);
            }

            if (cls.methods != NULL)
            {
                for (u2 j = 0; j < cls.method_count; j++)
                {
                    if (cls.methods[j].name != NULL)
                        free(cls.methods[j].name);
                    if (cls.methods[j].descriptor != NULL)
                        free(cls.methods[j].descriptor);
                    if (cls.methods[j].params != NULL)
                        free(cls.methods[j].params);
                    if (cls.methods[j].rettype != NULL)
                        free(cls.methods[j].rettype);
                    if (cls.methods[j].bytecode.code != NULL)
                        free(cls.methods[j].bytecode.code);
                }
                free(cls.methods);
            }
        }
        free(method_area.classes);
    }
}