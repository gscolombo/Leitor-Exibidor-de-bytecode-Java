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
    printf("{\n");

    show_fields(cf);
    if (cf->fields_count > 0)
        printf("\n");
    show_methods(cf);

    printf("}\n");
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

    l += (flags_str[1] != NULL ? (strlen(flags_str[1]) + (l > 0 ? 2 : 0)) : 0); // Extra size for ", "

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
