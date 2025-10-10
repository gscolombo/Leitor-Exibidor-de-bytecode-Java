#include "fields.h"

static const struct
{
    u2 flag;
    char *name;
    wchar_t *kw;
} flag_name_map[] = {
    {0x0001, "ACC_PUBLIC", L"public "},
    {0x0002, "ACC_PRIVATE", L"private "},
    {0x0004, "ACC_PROTECTED", L"protected "},
    {0x0008, "ACC_STATIC", L"static "},
    {0x0010, "ACC_FINAL", L"final "},
    {0x0040, "ACC_VOLATILE", L"volatile "},
    {0x0080, "ACC_TRANSIENT", L"transient "},
    {0x1000, "ACC_SYNTHETIC", L"synthetic "},
    {0x4000, "ACC_ENUM", L"enum "}};

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
            field_info field = cf->fields[i];

            u2 access_flags = field.access_flags;
            wchar_t *field_name = cp[field.name_index - 1].info.UTF8.str;
            wchar_t *field_desc = cp[field.descriptor_index - 1].info.UTF8.str;

            wchar_t *full_field_name = get_field_full_name(field_name, field_desc, access_flags, cf);

            if (full_field_name != NULL)
                printf("  %ls;\n    descriptor: %ls\n    flags: (0x%04x) \n%c", full_field_name, field_desc, access_flags, nl);
            
            free(full_field_name);
        }
}

wchar_t *get_field_full_name(const wchar_t *field_name, const wchar_t *field_desc, u2 flags, const ClassFile *cf)
{
    if (field_name == NULL || field_desc == NULL || cf == NULL)
        return NULL;

    cp_info *cp = cf->constant_pool;

    // Include modifiers
    wchar_t *full_field_name = (wchar_t *)calloc(256, sizeof(wchar_t *));
    if (full_field_name != NULL)
    {
        for (size_t i = 0; i < sizeof(flag_name_map) / sizeof(flag_name_map[0]); i++)
            if (flags & flag_name_map[i].flag)
                wcscat(full_field_name, flag_name_map[i].kw);

        // Include descriptor
        unsigned int dim = 0;
        wchar_t field_desc_cp[wcslen(field_desc) + 1];
        wcscpy(field_desc_cp, field_desc);
        for (wchar_t *l = field_desc_cp; l < field_desc_cp + wcslen(field_desc_cp); l++)
        {
            switch (*l)
            {
            case L'[':
                dim++;
                break;
            case L'L':
                while (*(++l) != L';')
                {
                    if (*l == L'/')
                        *l = L'.';
                    *(++full_field_name) = *l;
                }
                break;
            case L'B':
                wcscat(full_field_name, L"byte ");
                break;
            case L'C':
                wcscat(full_field_name, L"char ");
                break;
            case L'D':
                wcscat(full_field_name, L"double ");
                break;
            case L'F':
                wcscat(full_field_name, L"float ");
                break;
            case L'I':
                wcscat(full_field_name, L"int ");
                break;
            case L'J':
                wcscat(full_field_name, L"long ");
                break;
            case L'S':
                wcscat(full_field_name, L"short ");
                break;
            case L'Z':
                wcscat(full_field_name, L"boolean ");
                break;
            default:
                break;
            }

            if (*l != L'[' && dim > 0)
                while (dim > 0)
                    wcscat(full_field_name, L"[]");
        }

        return wcscat(full_field_name, field_name);
    }

    return NULL;
}
