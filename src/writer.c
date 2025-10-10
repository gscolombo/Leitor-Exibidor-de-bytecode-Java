#include "writer.h"

static const FlagMap class_flag_map[8] = {
    {0x0001, "ACC_PUBLIC"},
    {0x0010, "ACC_FINAL"},
    {0x0020, "ACC_SUPER"},
    {0x0200, "ACC_INTERFACE"},
    {0x0400, "ACC_ABSTRACT"},
    {0x1000, "ACC_SYNTHETIC"},
    {0x2000, "ACC_ANNOTATION"},
    {0x4000, "ACC_ENUM"}};

void show_classfile(ClassFile *cf)
{
    cp_info *cp = cf->constant_pool;

    u2 cls_name_index = cp[cf->this_class - 1].info.Class.name_index;
    u2 super_cls_name_index = cp[cf->super_class - 1].info.Class.name_index;

    wchar_t *classname = cp[cls_name_index - 1].info.UTF8.str;
    wchar_t *super_classname = cp[super_cls_name_index - 1].info.UTF8.str;
    char *class_access_flags = get_access_flags(cf->access_flags, 8, class_flag_map);

    printf("class %ls\n", classname);
    printf("  Magic: %#X\n", cf->magic);
    printf("  Version: %u.%u\n", cf->major_version, cf->minor_version);
    printf("  Flags: (%#.4x) %s\n", cf->access_flags, class_access_flags);
    printf("  this_class: #%u\t\t\t// %ls\n", cf->this_class, classname);
    printf("  super_class: #%u\t\t\t// %ls\n", cf->super_class, super_classname);
    printf("  interfaces: %u, fields: %u, methods: %u, attributes: %u\n",
           cf->interfaces_count, cf->fields_count, cf->methods_count, cf->attributes_count);

    printf("Constant Pool (count = %u):\n", cf->constant_pool_count);

    u2 count = cf->constant_pool_count;
    show_constants(count, cp);

    if (count > 0)
        printf("{\n");

    show_fields(cf);
    if (cf->fields_count > 0)
        printf("\n");
    show_methods(cf);

    printf("}\n");

    free(class_access_flags);
}


static const FlagMap flag_map[9] = {
    {0x0001, "ACC_PUBLIC"},
    {0x0002, "ACC_PRIVATE"},
    {0x0004, "ACC_PROTECTED"},
    {0x0008, "ACC_STATIC"},
    {0x0010, "ACC_FINAL"},
    {0x0040, "ACC_VOLATILE"},
    {0x0080, "ACC_TRANSIENT"},
    {0x1000, "ACC_SYNTHETIC"},
    {0x4000, "ACC_ENUM"}};

void show_fields(const ClassFile *cf)
{
    if (cf->constant_pool == NULL)
        return;

    cp_info *cp = cf->constant_pool;

    u2 count = cf->fields_count;
    char nl;
    if (count > 0 && cf->fields != NULL)
        for (size_t i = 0; i < count; i++)
        {
            nl = i == (size_t)(count - 1) ? ' ' : '\n';
            member_info field = cf->fields[i];

            u2 access_flags = field.access_flags;
            wchar_t *field_name = cp[field.name_index - 1].info.UTF8.str;
            wchar_t *field_desc = cp[field.descriptor_index - 1].info.UTF8.str;

            wchar_t *field_desc_str = parse_descriptor(field_desc, NULL);

            char *flags = get_access_flags(access_flags, 9, flag_map);

            printf(" %ls%ls;\n    descriptor: %ls\n    flags: (0x%04x) %s\n%c",
                   field_desc_str, field_name, field_desc,
                   access_flags, flags, nl);

            free(flags);
            free(field_desc_str);
        }
}
