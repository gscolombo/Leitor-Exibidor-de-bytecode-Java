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
            method_info method = cf->methods[i];

            u2 access_flags = method.access_flags;
            wchar_t *method_name = cp[method.name_index - 1].info.UTF8.str;
            wchar_t *method_desc = cp[method.descriptor_index - 1].info.UTF8.str;

            wchar_t *full_method_name = get_full_method_name(method_name, method_desc, access_flags, cf);

            char *flags = get_access_flags(access_flags, 12, flag_map);

            if (full_method_name != NULL && flags != NULL)
                printf("  %ls;\n    descriptor: %ls\n    flags: (0x%04x) %s\n%c", full_method_name, method_desc, access_flags, flags, nl);

            free(flags);
            free(full_method_name);
        }
}

wchar_t *get_full_method_name(wchar_t *method_name, const wchar_t *method_desc, u2 flags, const ClassFile *cf)
{
    if (cf == NULL)
        return NULL;

    cp_info *cp = cf->constant_pool;

    // Handle <init> method
    int is_init = !wcscmp(method_name, L"<init>");
    if (is_init)
    {
        wchar_t *classname = cp[cp[cf->this_class - 1].info.Class.name_index - 1].info.UTF8.str;

        for (wchar_t *wcptr = classname; wcptr < classname + wcslen(classname) - 1; wcptr++)
            if (*wcptr == L'/')
            {
                *wcptr = L'.';
                break;
            }

        method_name = classname;
    }

    // Include modifiers
    size_t buffer_size = 256;
    wchar_t *full_method_name = (wchar_t *)calloc(buffer_size, sizeof(wchar_t));
    if (full_method_name != NULL)
    {
        if (flags & 0x0001)
            wcscat(full_method_name, L"public ");
        if (flags & 0x0002)
            wcscat(full_method_name, L"private ");
        if (flags & 0x0004)
            wcscat(full_method_name, L"protected ");

        if (flags & 0x0008)
            wcscat(full_method_name, L"static ");
        if (flags & 0x0010)
            wcscat(full_method_name, L"final ");
        if (flags & 0x0020)
            wcscat(full_method_name, L"synchronized ");
        if (flags & 0x0400)
            wcscat(full_method_name, L"abstract ");

        // Parse descriptor
        size_t dim = 0;
        const wchar_t *buf = method_desc;

        //// Parse parameters descriptor
        wchar_t *params = (wchar_t *)calloc(buffer_size, sizeof(wchar_t));
        parse_method_descriptors(params, &buf, &dim);
        while (*(++buf) != L')')
        {
            parse_method_descriptors(params, &buf, &dim);
            if (buf > method_desc && *(buf + 1) != L')' && *buf != L'[')
                wcsncat(params, L", ", 2);
        }

        wcsncat(params, buf, 1);

        //// Parse return descriptor
        if (!is_init)
        {
            wchar_t *ret = (wchar_t *)calloc(buffer_size, sizeof(wchar_t));
            while (buf++ < method_desc + wcslen(method_desc) - 1)
                parse_method_descriptors(ret, &buf, &dim);
            wcscat(ret, L" ");

            wcscat(full_method_name, ret);
            free(ret);
        }

        wcscat(full_method_name, method_name);
        wcscat(full_method_name, params);

        free(params);

        return full_method_name;
    }

    return NULL;
}

static const struct
{
    wchar_t base_type;
    wchar_t *type;
} base_type_map[9] = {
    {L'B', L"byte"},
    {L'C', L"char"},
    {L'D', L"double"},
    {L'F', L"float"},
    {L'I', L"int"},
    {L'J', L"long"},
    {L'S', L"short"},
    {L'Z', L"boolean"},
    {L'V', L"void"}};

void parse_method_descriptors(wchar_t *dest_str, const wchar_t **buf, size_t *dim)
{
    if (**buf == L'[')
        (*dim)++;
    else
    {
        switch (**buf)
        {
        case L'(':
            wcsncat(dest_str, L"(", 1);
            break;
        case L'L':
            wchar_t *cls = (wchar_t *)calloc(256, sizeof(wchar_t));
            if (cls != NULL)
            {
                wchar_t *cls_ptr = cls;
                while (*(++(*buf)) != L';')
                {
                    *cls_ptr = **buf == L'/' ? L'.' : **buf;
                    cls_ptr++;
                }
                *cls_ptr = L'\0';
                wcscat(dest_str, cls);
            }
            free(cls);
            break;
        default:
            for (size_t i = 0; i < sizeof(base_type_map) / sizeof(base_type_map[0]); i++)
                if (**buf == base_type_map[i].base_type)
                    wcscat(dest_str, base_type_map[i].type);
            break;
        }
        if (*dim > 0)
            while (*dim > 0)
            {
                wcscat(dest_str, L"[]");
                (*dim)--;
            }
    }
}