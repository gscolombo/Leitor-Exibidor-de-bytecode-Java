#include "fields.h"

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

static const FlagMap flag_kw_map[7] = {
    {0x0001, "public"},
    {0x0002, "private"},
    {0x0004, "protected"},
    {0x0008, "static"},
    {0x0010, "final"},
    {0x0040, "volatile"},
    {0x0080, "transient"}};

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

            size_t buffer_size = 256;
            char *kws = malloc(buffer_size);
            if (kws != NULL)
            {
                char *ptr = kws;

                for (size_t i = 0; i < 7; i++)
                {
                    if (access_flags & flag_kw_map[i].flag)
                    {
                        size_t l = strlen(flag_kw_map[i].name);

                        if (buffer_size > l + 2)
                        {
                            strcpy(ptr, flag_kw_map[i].name);
                            ptr += l;
                            buffer_size -= l;
                            *ptr++ = ' ';
                            buffer_size--;
                        }
                    }
                }

                if (ptr > kws)
                    ptr--; // Remove trailing space
                *ptr = '\0';

                char *tmp = realloc(kws, buffer_size);
                if (tmp)
                    kws = tmp;
            }

            printf(" %s %ls%ls;\n    descriptor: %ls\n    flags: (0x%04x) %s\n%c",
                   kws, field_desc_str, field_name, field_desc,
                   access_flags, flags, nl);

            free(flags);
            free(field_desc_str);
            free(kws);
        }
}