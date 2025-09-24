#include "writer.h"

void show_classfile(ClassFile* cf) {
    printf("Magic: %#X\n", cf->magic);
    printf("Version: %u.%u\n", cf->major_version, cf->minor_version);
    printf("Constant Pool (count = %u):\n", cf->constant_pool_count);

    cp_info *cp = cf->constant_pool;
    u2 count = cf->constant_pool_count;

    for (u2 i = 1; i < count; i++, cp++) {
        show_constant(i, count, cp);
    }
}

void show_constant(u2 i, u2 count, cp_info* cp) {
    // Align list indexes
    u1 ws = num_digits(count) - num_digits(i);
    char *pad = malloc(ws + 1);
    memset(pad, ' ', ws);
    pad[ws] = '\0';

    switch (cp->tag)
    {
    case CONSTANT_Class:
        u2 name_index = cp->info.Class.name_index;
        printf("%s#%u = Class\t\t\t#%u\t\t// %ls\n", 
                pad, i, name_index, cp[name_index - i].info.UTF8.str);
        break;
    case CONSTANT_Fieldref:
    case CONSTANT_Methodref:
    case CONSTANT_InterfaceMethodref:
        char* ref = cp->tag == CONSTANT_Fieldref ? "Fieldref" : (cp->tag == CONSTANT_Methodref ? "Methodref": "InterfaceMethodref");
        u2 class_index = cp->info._Ref.class_index, name_and_type_index = cp->info._Ref.name_and_type_index;
        wchar_t *cls = cp[cp[class_index-i].info.Class.name_index - i].info.UTF8.str; 
        wchar_t *name = cp[cp[name_and_type_index-i].info.NameAndType.name_index - i].info.UTF8.str;
        wchar_t *type = cp[cp[name_and_type_index-i].info.NameAndType.descriptor_index - i].info.UTF8.str;
        name = !wcscmp(name, (wchar_t*) L"<init>") ? L"\"<init>\"" : name;
        
        printf("%s#%u = %s\t\t\t#%u.#%u\t\t// %ls.%ls:%ls\n", pad, i, ref, class_index, name_and_type_index, cls, name, type);
        break;
    case CONSTANT_String:
        printf("%s#%u = String\n", pad, i);
        break;
    case CONSTANT_Integer:
        printf("%s#%u = Integer\n", pad, i);
        break;
    case CONSTANT_Float:
        printf("%s#%u = Float\n", pad, i);
        break;
    case CONSTANT_Long:
        printf("%s#%u = Long\n", pad, i);
        break;
    case CONSTANT_Double:
        printf("%s#%u = Double\n", pad, i);
        break;
    case CONSTANT_NameAndType:
        printf("%s#%u = NameAndType\n", pad, i);
        break;
    case CONSTANT_UTF8:
        printf("%s#%u = UTF-8\t\t\t%ls\n", pad, i, cp->info.UTF8.str);
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