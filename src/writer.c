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

static u2 be_u2_from_bytes(const u1 *p) {
    return (u2)((p[0] << 8) | p[1]); // big-endian
}

static void show_class_attributes(ClassFile *cf)
{
    if (cf->attributes_count == 0) return;

    cp_info *cp = cf->constant_pool;

    for (u2 i = 0; i < cf->attributes_count; ++i) {
        const attribute *ai = &cf->attributes[i];

        // resolve nome do atributo via CP (Utf8)
        const char *attr_name = cp[ai->attribute_name_index - 1].info.UTF8.str;
        if (!attr_name) {
            printf("[<unknown-attribute>: length=%u]\n", ai->attribute_length);
            continue;
        }

        if (strcmp(attr_name, "SourceFile") == 0) {
            u2 sourcefile_index = ai->info.SourceFile.sourcefile_index; // já vem parseado
            if (sourcefile_index > 0 && sourcefile_index <= cf->constant_pool_count) {
                const char *fname = cf->constant_pool[sourcefile_index - 1].info.UTF8.str;
                if (fname) printf(" [SourceFile: %s]\n", fname);
                else       printf("[SourceFile: <invalid cp index #%u>]\n", sourcefile_index);
            } else {
                printf("[SourceFile: <invalid cp index #%u>]\n", sourcefile_index);
            }
        } else {
            // fallback para atributos de classe ainda não implementados
            printf("[%s: length=%u]\n", attr_name, ai->attribute_length);
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

    // imprime atributos de classe (ex.: SourceFile) antes da chave final
    show_class_attributes(cf);

    printf("}\n");

    free(class_access_flags);
    free(class_kws_flags);
}
