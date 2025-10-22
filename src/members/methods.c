#include "methods.h"

static const FlagMap flag_map[12] = {
    {0x0001, "ACC_PUBLIC"},
    {0x0002, "ACC_PRIVATE"},
    {0x0004, "ACC_PROTECTED"},
    {0x0008, "ACC_STATIC"},
    {0x0010, "ACC_FINAL"},
    {0x0020, "ACC_SYNCHRONIZED"},
    {0x0040, "ACC_BRIDGE"},
    {0x0080, "ACC_VARARGS"},
    {0x0100, "ACC_NATIVE"},
    {0x0400, "ACC_ABSTRACT"},
    {0x0800, "ACC_STRICT"},
    {0x1000, "ACC_SYNTHETIC"}};

static const FlagMap flag_kw_map[9] = {
    {0x0001, "public"},
    {0x0002, "private"},
    {0x0004, "protected"},
    {0x0008, "static"},
    {0x0010, "final"},
    {0x0020, "synchronized"},
    {0x0100, "native"},
    {0x0400, "abstract"},
    {0x0800, "strictfp"}};

void show_methods(const ClassFile *cf)
{
    if (cf->constant_pool == NULL)
        return;

    cp_info *cp = cf->constant_pool;

    u2 count = cf->methods_count;
    char nl;
    if (count > 0 && cf->methods != NULL)
        for (size_t i = 0; i < count; i++)
        {
            nl = i == (size_t)(count - 1) ? ' ' : '\n';
            member_info method = cf->methods[i];

            u2 access_flags = method.access_flags;
            char *method_name = cp[method.name_index - 1].info.UTF8.str;
            char *method_desc = cp[method.descriptor_index - 1].info.UTF8.str;

            // Handle <init> method
            int is_init = !strcmp(method_name, "<init>");
            if (is_init)
            {
                char *classname = cp[cp[cf->this_class - 1].info.Class.name_index - 1].info.UTF8.str;

                for (char *wcptr = classname; wcptr < classname + strlen(classname) - 1; wcptr++)
                    if (*wcptr == L'/')
                    {
                        *wcptr = L'.';
                        break;
                    }

                method_name = classname;
            }

            // Get parameters descriptor length
            char *params_end = strchr(method_desc, L')') + 1;

            size_t params_desc_len = params_end - method_desc;
            size_t ret_desc_len = strlen(params_end);

            // Split descriptor
            char *params_desc = (char *)calloc(params_desc_len + 1, sizeof(char));
            if (params_desc)
            {
                strncpy(params_desc, method_desc, params_desc_len);
                params_desc[params_desc_len] = L'\0';
            }

            char *ret_desc = (char *)calloc(ret_desc_len + 1, sizeof(char));
            if (ret_desc)
            {
                strncpy(ret_desc, params_end, ret_desc_len);
                ret_desc[ret_desc_len] = L'\0';
            }

            // Parse descriptors
            char *params_str = NULL, *ret_str = NULL;

            if (!is_init)
                ret_str = parse_descriptor(ret_desc, NULL);

            params_str = parse_descriptor(params_desc, ",");

            // Parse flags
            char *flags = parse_flags(access_flags, 12, ", ", flag_map);
            char *kws = parse_flags(access_flags, 9, " ", flag_kw_map);

            printf(" %s %s%s%s;\n    descriptor: %s\n    flags: (0x%04x) %s\n%c",
                   kws, ret_str ? ret_str : "", method_name, params_str, method_desc, access_flags, flags, nl);

            free(flags), free(kws);
            free(params_desc), free(ret_desc);
            free(params_str), free(ret_str);
        }
}