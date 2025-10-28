#include "writer.h"
#include <string.h>
#include "types/cp/constants.h"
#include "types/attributes/attribute_info.h"
#include "types/attributes/attributes.h"

/** @file
 * @brief Declaração de função para exibição de informações do arquivo `.class`.
 */

static const FlagMap class_flag_map[8] = {
    {0x0001, "ACC_PUBLIC"},
    {0x0010, "ACC_FINAL"},
    {0x0020, "ACC_SUPER"},
    {0x0200, "ACC_INTERFACE"},
    {0x0400, "ACC_ABSTRACT"},
    {0x1000, "ACC_SYNTHETIC"},
    {0x2000, "ACC_ANNOTATION"},
    {0x4000, "ACC_ENUM"}};

static const FlagMap class_flag_kw_map[6] = {
    {0x0001, "public"},
    {0x0010, "final"},
    {0x0200, "interface"},
    {0x0400, "abstract"},
    {0x2000, "@interface"},
    {0x4000, "enum"}};

static void show_class_attributes(ClassFile *cf)
{
    if (cf->attributes_count == 0)
        return;

    cp_info *cp = cf->constant_pool;

    for (u2 i = 0; i < cf->attributes_count; ++i)
    {
        const attribute *ai = &cf->attributes[i];

        // resolve nome do atributo via CP (Utf8)
        const char *attr_name = cp[ai->attribute_name_index - 1].info.UTF8.str;
        if (!attr_name)
        {
            printf("[<unknown-attribute>: length=%u]\n", ai->attribute_length);
            continue;
        }

        const attribute_name *attr_enum = convert_attr_name(attr_name);

        if (attr_enum)
            switch (*attr_enum)
            {
            case SourceFile:
                u2 sourcefile_index = ai->info.SourceFile.sourcefile_index; // já vem parseado
                if (sourcefile_index > 0 && sourcefile_index <= cf->constant_pool_count)
                {
                    const char *fname = cf->constant_pool[sourcefile_index - 1].info.UTF8.str;
                    if (fname)
                        printf("SourceFile: \"%s\"\n", fname);
                    else
                        printf("SourceFile: <invalid cp index #%u>\n", sourcefile_index);
                }
                else
                {
                    printf("SourceFile: <invalid cp index #%u>\n", sourcefile_index);
                }
                break;
            case InnerClasses:
                u2 n = ai->info.InnerClasses.number_of_classes;
                if (n > 0)
                {
                    printf("InnerClasses:\n");
                    for (u2 i = 0; i < n; i++)
                    {
                        struct classes class = ai->info.InnerClasses.classes[i];
                        const char *inner_class_name = cp[class.inner_name_index - 1].info.UTF8.str;
                        const char *inner_class_full_name = cp[cp[class.inner_class_info_index - 1].info.Class.name_index - 1].info.UTF8.str;
                        const char *outer_class_name = cp[cp[class.outer_class_info_index - 1].info.Class.name_index - 1].info.UTF8.str;

                        printf("  #%u= #%u of #%u;\t\t\t\t  // %s=class %s of class %s\n",
                               class.inner_name_index, class.inner_class_info_index, class.outer_class_info_index,
                               inner_class_name, inner_class_full_name, outer_class_name);
                    }
                }
                break;
            default:
                printf("[%s: length=%u]\n", attr_name, ai->attribute_length);
                break;
            }
    }
}

void show_classfile(ClassFile *cf)
{
    cp_info *cp = cf->constant_pool;

    u2 cls_name_index = cp[cf->this_class - 1].info.Class.name_index;
    u2 super_cls_name_index = cp[cf->super_class - 1].info.Class.name_index;

    char *classname = cp[cls_name_index - 1].info.UTF8.str;
    char *super_classname = cp[super_cls_name_index - 1].info.UTF8.str;
    char *class_access_flags = parse_flags(cf->access_flags, 8, ", ", class_flag_map);
    char *class_kws_flags = parse_flags(cf->access_flags, 6, " ", class_flag_kw_map);

    printf("%s class %s\n", class_kws_flags, classname);
    printf("  Magic: %#X\n", cf->magic);
    printf("  Version: %u.%u\n", cf->major_version, cf->minor_version);
    printf("  Flags: (%#.4x) %s\n", cf->access_flags, class_access_flags);
    printf("  this_class: #%u\t\t\t// %s\n", cf->this_class, classname);
    printf("  super_class: #%u\t\t\t// %s\n", cf->super_class, super_classname);
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
        printf("\n");
    }

    printf("}\n");
    show_class_attributes(cf);

    free(class_access_flags);
    free(class_kws_flags);
}
