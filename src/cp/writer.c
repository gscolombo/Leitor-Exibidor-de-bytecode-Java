#include "cp/writer.h"

void show_constants(u2 count, cp_info *_cp)
{
    u2 i = 1;
    for (cp_info *cp = _cp; cp < _cp + count; cp++, i++)
    {
        u1 ws = num_digits(count) - num_digits(i);
        char *pad = malloc(ws + 1);
        memset(pad, ' ', ws);
        pad[ws] = '\0';

        switch (cp->tag)
        {
        case CONSTANT_Class:
        {
            u2 cls_name_index = cp->info.Class.name_index;
            printf("%s#%u = Class\t\t\t#%u\t\t// %s\n",
                   pad, i, cls_name_index, _cp[cls_name_index - 1].info.UTF8.str);
            break;
        }
        case CONSTANT_Fieldref:
        case CONSTANT_Methodref:
        case CONSTANT_InterfaceMethodref:
        {
            char *ref;
            if (cp->tag == CONSTANT_Fieldref)
                ref = "Fieldref";
            else if (cp->tag == CONSTANT_Methodref)
                ref = "Methodref";
            else
                ref = "InterfaceMethodref";

            u2 cls_index = cp->info.Ref.class_index;
            u2 name_and_type_index = cp->info.Ref.name_and_type_index;

            char *cls = _cp[_cp[cls_index - 1].info.Class.name_index - 1].info.UTF8.str;
            char *ref_name = _cp[_cp[name_and_type_index - 1].info.NameAndType.name_index - 1].info.UTF8.str;
            char *ref_type = _cp[_cp[name_and_type_index - 1].info.NameAndType.descriptor_index - 1].info.UTF8.str;

            ref_name = !strcmp(ref_name, "<init>") ? "\"<init>\"" : ref_name;

            printf("%s#%u = %s\t\t\t#%u.#%u\t\t// %s.%s:%s\n",
                   pad, i, ref, cls_index, name_and_type_index, cls, ref_name, ref_type);
            break;
        }
        case CONSTANT_String:
        {
            u2 str_index = cp->info.String.string_index;
            char *str = _cp[str_index - 1].info.UTF8.str;

            if (str[0] == '\n')
                str[0] = '\0';

            size_t l;
            if ((l = strlen(str)) > 0 && str[l - 1] == '\n')
                str[l - 1] = 0;

            printf("%s#%u = String\t\t\t#%u\t\t// %s\n", pad, i, str_index, str);
            break;
        }
        case CONSTANT_Integer:
            printf("%s#%u = Integer\t\t\t%i\n", pad, i, cp->info._4Bn.number.i);
            break;
        case CONSTANT_Float:
            printf("%s#%u = Float\t\t\t%.1ff\n", pad, i, cp->info._4Bn.number.f);
            break;
        case CONSTANT_Long:
            printf("%s#%u = Long\t\t\t%lil\n", pad, i, cp->info._8Bn.number.l);
            break;
        case CONSTANT_Double:
            printf("%s#%u = Double\t\t\t%.1lfd\n", pad, i, cp->info._8Bn.number.d);
            break;
        case CONSTANT_NameAndType:
        {
            u2 name_index = cp->info.NameAndType.name_index;
            u2 desc_index = cp->info.NameAndType.descriptor_index;
            char *name = _cp[name_index - 1].info.UTF8.str;
            char *desc = _cp[desc_index - 1].info.UTF8.str;
            name = !strcmp(name, "<init>") ? "\"<init>\"" : name;
            printf("%s#%u = NameAndType\t\t#%u.#%u\t\t// %s:%s\n", pad, i, name_index, desc_index, name, desc);
            break;
        }
        case CONSTANT_UTF8:
            char *str = cp->info.UTF8.str;
            if (str[0] == '\n')
                str[0] = '\0';

            size_t l;
            if ((l = strlen(str)) > 0 && str[l - 1] == '\n')
                str[l - 1] = 0;

            printf("%s#%u = UTF-8\t\t\t%s\n", pad, i, str);
            break;
        case CONSTANT_MethodHandle:
            printf("%s#%u = MethodHandle\n", pad, i);
            break;
        case CONSTANT_MethodType:
            printf("%s#%u = MethodType\n", pad, i);
            break;
        case CONSTANT_InvokeDynamic:
            printf("%s#%u = InvokeDynamic\n", pad, i);
            break;
        default:
            break;
        }

        free(pad);
    }
}

char *get_constant_UTF8_value(u2 i, const cp_info *cp)
{
    char *s = NULL;
    i--;

    switch (cp[i].tag)
    {
    case CONSTANT_Class:
        return get_constant_UTF8_value(cp[i].info.Class.name_index, cp);
    case CONSTANT_Fieldref:
    case CONSTANT_Methodref:
    case CONSTANT_InterfaceMethodref:
    {
        u2 cls_index = cp[i].info.Ref.class_index;
        u2 name_and_type_index = cp[i].info.Ref.name_and_type_index;

        char *cls_name = get_constant_UTF8_value(cls_index, cp);
        char *name_and_type = get_constant_UTF8_value(name_and_type_index, cp);

        if (cls_name && name_and_type)
        {
            s = (char *)malloc((strlen(cls_name) + strlen(name_and_type) + 2) * sizeof(char));
            if (s)
                sprintf(s, "%s.%s", cls_name, name_and_type);
            free(cls_name);
            free(name_and_type);
        }
        break;
    }
    case CONSTANT_NameAndType:
    {
        u2 name_index = cp[i].info.NameAndType.name_index;
        u2 desc_index = cp[i].info.NameAndType.descriptor_index;

        char *ref_name = get_constant_UTF8_value(name_index, cp);
        char *ref_type = get_constant_UTF8_value(desc_index, cp);

        if (ref_name != NULL && ref_type != NULL)
        {
            s = (char *)malloc((strlen(ref_name) + strlen(ref_type) + 2) * sizeof(char));
            if (s)
                sprintf(s, "%s:%s", ref_name, ref_type);
            free(ref_name);
            free(ref_type);
        }
        break;
    }
    case CONSTANT_String:
        char *str = get_constant_UTF8_value(cp[i].info.String.string_index, cp);
        s = (char *)malloc((strlen(str) + 1) * sizeof(char));
        strcpy(s, str);
        free(str);
        break;
    case CONSTANT_Integer:
    case CONSTANT_Float:
    case CONSTANT_Long:
    case CONSTANT_Double:
        s = (char *)malloc(15 * sizeof(char));
        if (s)
        {
            switch (cp[i].tag)
            {
            case CONSTANT_Integer:
                sprintf(s, "%i", cp[i].info._4Bn.number.i);
                break;
            case CONSTANT_Float:
                sprintf(s, "%gf", cp[i].info._4Bn.number.f);
                break;
            case CONSTANT_Long:
                sprintf(s, "%lil", cp[i].info._8Bn.number.l);
                break;
            case CONSTANT_Double:
                sprintf(s, "%gd", cp[i].info._8Bn.number.d);
                break;
            default:
                break;
            }
            size_t l = strlen(s);
            s = (char *)realloc(s, l + 1);
        }
        break;
    case CONSTANT_UTF8:
        s = (char *)malloc((strlen(cp[i].info.UTF8.str) + 1) * sizeof(char));
        strcpy(s, cp[i].info.UTF8.str);
        break;
    default:
        break;
    }

    if (s[0] == '\n')
        s[0] = '\0';

    size_t l = strlen(s);
    if (l > 0)
        if (s[l - 1] == '\n')
            s[l - 1] = 0;

    return s;
}
