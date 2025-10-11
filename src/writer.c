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

    if (cf->constant_pool_count > 0)
    {
        show_constants(cf->constant_pool_count, cp);
        printf("{\n");
    }

    if (cf->fields_count > 0)
    {
        show_fields(cf);
        printf("\n");
    }

    if (cf->methods_count > 0)
    {
        show_methods(cf);
        printf("}\n");
    }

    free(class_access_flags);
}
