#include "cp/writer.h"

void show_constants(u2 count, cp_info *_cp)
{
    u2 i = 1;
    for (cp_info *cp = _cp; cp < _cp + count; cp++, i++)
    {
        // Alinhamento à direita dos índices da lista
        u1 ws = num_digits(count) - num_digits(i);
        char *pad = malloc(ws + 1);
        memset(pad, ' ', ws);
        pad[ws] = '\0';

        switch (cp->tag)
        {
        case CONSTANT_Class:
            u2 cls_name_index = cp->info.Class.name_index;
            printf("%s#%u = Class\t\t\t#%u\t\t// %s\n",
                   pad, i, cls_name_index, _cp[cls_name_index - 1].info.UTF8.str);
            break;
        case CONSTANT_Fieldref:
        case CONSTANT_Methodref:
        case CONSTANT_InterfaceMethodref:
            char *ref;
            if (cp->tag == CONSTANT_Fieldref)
                ref = "Fieldref";
            else if (cp->tag == CONSTANT_Methodref)
                ref = "Methodref";
            else
                ref = "InterfaceMethodref";

            // Índices das estruturas de referência
            u2 cls_index = cp->info.Ref.class_index;
            u2 name_and_type_index = cp->info.Ref.name_and_type_index;

            // Recuperação as strings para exibição
            char *cls = _cp[_cp[cls_index - 1].info.Class.name_index - 1].info.UTF8.str;
            char *ref_name = _cp[_cp[name_and_type_index - 1].info.NameAndType.name_index - 1].info.UTF8.str;
            char *ref_type = _cp[_cp[name_and_type_index - 1].info.NameAndType.descriptor_index - 1].info.UTF8.str;

            // Inclusão de aspas na função <init> para conformidade com saída do javap
            ref_name = !strcmp(ref_name, (char *)"<init>") ? "\"<init>\"" : ref_name;

            printf("%s#%u = %s\t\t\t#%u.#%u\t\t// %s.%s:%s\n",
                   pad, i, ref, cls_index, name_and_type_index, cls, ref_name, ref_type);
            break;
        case CONSTANT_String:
            u2 str_index = cp->info.String.string_index;
            printf("%s#%u = String\t\t\t#%u\t\t// %s\n", pad, i, str_index, _cp[str_index - 1].info.UTF8.str);
            break;
        case CONSTANT_Integer:
            printf("%s#%u = Integer\t\t\t%i\n", pad, i, cp->info._4Bn.number.i);
            break;
        case CONSTANT_Float:
            printf("%s#%u = Float\t\t\t%f\n", pad, i, cp->info._4Bn.number.f);
            break;
        case CONSTANT_Long:
            printf("%s#%u = Long\t\t\t%li\n", pad, i, cp->info._8Bn.number.l);
            break;
        case CONSTANT_Double:
            printf("%s#%u = Double\t\t\t%f\n", pad, i, cp->info._8Bn.number.d);
            break;
        case CONSTANT_NameAndType:
            // Índices das strings de nome e descritor
            u2 name_index = cp->info.NameAndType.name_index;
            u2 desc_index = cp->info.NameAndType.descriptor_index;

            // Recuperação das strings
            char *name = _cp[name_index - 1].info.UTF8.str;
            char *desc = _cp[desc_index - 1].info.UTF8.str;

            // Inclusão de aspas no método <init> para conformidade com saída do javap
            name = !strcmp(name, (char *)"<init>") ? "\"<init>\"" : name;

            printf("%s#%u = NameAndType\t\t#%u.#%u\t\t// %s:%s\n", pad, i, name_index, desc_index, name, desc);
            break;
        case CONSTANT_UTF8:
            printf("%s#%u = UTF-8\t\t\t%s\n", pad, i, cp->info.UTF8.str);
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
