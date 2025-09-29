#include "writer.h"

void show_classfile(ClassFile *cf)
{
    cp_info *cp = cf->constant_pool;

    u2 cls_name_index = cp[cf->this_class - 1].info.Class.name_index;
    u2 super_cls_name_index = cp[cf->super_class - 1].info.Class.name_index;

    wchar_t *classname = cp[cls_name_index - 1].info.UTF8.str;
    wchar_t *super_classname = cp[super_cls_name_index - 1].info.UTF8.str;

    printf("class %ls\n", classname);
    printf("  Magic: %#X\n", cf->magic);
    printf("  Version: %u.%u\n", cf->major_version, cf->minor_version);
    show_class_access_flags(cf);
    printf("  this_class: #%u\t\t\t// %ls\n", cf->this_class, classname);
    printf("  super_class: #%u\t\t\t// %ls\n", cf->super_class, super_classname);
    printf("Constant Pool (count = %u):\n", cf->constant_pool_count);

    u2 count = cf->constant_pool_count;
    for (u2 i = 1; i < count; i++, cp++)
    {
        show_constant(i, count, cp);
    }
}

void show_constant(u2 i, u2 count, cp_info *cp)
{
    // Align list indexes
    u1 ws = num_digits(count) - num_digits(i);
    char *pad = malloc(ws + 1);
    memset(pad, ' ', ws);
    pad[ws] = '\0';

    switch (cp->tag)
    {
    case CONSTANT_Class:
        u2 cls_name_index = cp->info.Class.name_index;
        printf("%s#%u = Class\t\t\t#%u\t\t// %ls\n",
               pad, i, cls_name_index, cp[cls_name_index - i].info.UTF8.str);
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

        struct RefInfo r = get_ref_info(cp, i);
        printf("%s#%u = %s\t\t\t#%u.#%u\t\t// %ls.%ls:%ls\n",
               pad, i, ref, r.cls_index, r.name_and_type_index, r.cls, r.cls_name, r.cls_type);
        break;
    case CONSTANT_String:
        u2 str_index = cp->info.String.string_index;
        printf("%s#%u = String\t\t\t#%u\t\t// %ls\n", pad, i, str_index, cp[str_index - i].info.UTF8.str);
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
        u2 name_index = cp->info.NameAndType.name_index;
        u2 desc_index = cp->info.NameAndType.descriptor_index;
        wchar_t *name = cp[name_index - i].info.UTF8.str;
        wchar_t *desc = cp[desc_index - i].info.UTF8.str;
        name = !wcscmp(name, (wchar_t *)L"<init>") ? L"\"<init>\"" : name;

        printf("%s#%u = NameAndType\t\t#%u.#%u\t\t// %ls:%ls\n", pad, i, name_index, desc_index, name, desc);
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

struct RefInfo get_ref_info(const cp_info *cp, u2 i)
{
    u2 cls_index = cp->info.Ref.class_index;
    u2 name_and_type_index = cp->info.Ref.name_and_type_index;
    wchar_t *cls = cp[cp[cls_index - i].info.Class.name_index - i].info.UTF8.str;
    wchar_t *cls_name = cp[cp[name_and_type_index - i].info.NameAndType.name_index - i].info.UTF8.str;
    wchar_t *cls_type = cp[cp[name_and_type_index - i].info.NameAndType.descriptor_index - i].info.UTF8.str;
    cls_name = !wcscmp(cls_name, (wchar_t *)L"<init>") ? L"\"<init>\"" : cls_name;

    struct RefInfo ref_info = {
        .cls = cls,
        .cls_name = cls_name,
        .cls_type = cls_type,
        .cls_index = cls_index,
        .name_and_type_index = name_and_type_index};

    return ref_info;
}

void show_class_access_flags(const ClassFile *cf)
{
    u2 flags = cf->access_flags, n = 0;
    char *flags_str[4] = {NULL};
    size_t l = 0;

    if ((flags & 0x0001) == 0x0001)
    {
        flags_str[0] = "ACC_PUBLIC";
        l += strlen(flags_str[0]);
        n++;
    }

    n++;
    if ((flags & 0x0010) == 0x0010)
        flags_str[1] = "ACC_FINAL";
    else if ((flags & 0x0020) == 0x0020)
        flags_str[1] = "ACC_SUPER";
    else
        n--;

    l += (flags_str[1] != NULL ? (strlen(flags_str[1]) + + (l > 0 ? 2 : 0)) : 0); // Extra size for ", "

    n++;
    if ((flags & 0x0200) == 0x0200)
        flags_str[2] = "ACC_INTERFACE";
    else if ((flags & 0x0400) == 0x0400)
        flags_str[2] = "ACC_ABSTRACT";
    else
        n--;

    l += (flags_str[2] != NULL ? (strlen(flags_str[2]) + (l > 0 ? 2 : 0)) : 0);

    n++;
    if ((flags & 0x1000) == 0x1000)
        flags_str[3] = "ACC_SYNTHETIC";
    else if ((flags & 0x2000) == 0x2000)
        flags_str[3] = "ACC_ANNOTATION";
    else if ((flags & 0x4000) == 0x4000)
        flags_str[3] = "ACC_ENUM";
    else
        n--;

    l += flags_str[3] != NULL ? strlen(flags_str[3]) : 0;

    char *str = (char *)calloc((l + 1), sizeof(char));
    for (int i = 0; i < n; i++)
    {
        if (i)
            strncat(str, ", ", 2 + 1);
        strncat(str, flags_str[i], strlen(flags_str[i]) + 1);
    }

    printf("  Flags: (%#.4x) %s\n", flags, str);
    free(str);
}