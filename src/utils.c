#include "utils.h"
#include <wchar.h>

/** @file
 * @brief Definição de funções utilitárias para o restante do projeto.
 */

u2 u2swap(u2 n) {
    return ((n >> 8) & 0xff) |
           ((n << 8) & 0xff00);
}

u4 u4swap(u4 n) {
    return ((n >> 24) & 0xff) |      // Troca o byte 3 com o byte 0
           ((n >> 8) & 0xff00) |     // Troca o byte 1 com o byte 2
           ((n << 8) & 0xff0000) |   // Troca o byte 2 com o byte 1
           ((n << 24) & 0xff000000); // Troca o byte 0 com o byte 3
}

unsigned int num_digits(unsigned int n)
{
    if (n < 10)
        return 1;

    int r = 2;
    while ((n /= 10) > 9)
    {
        ++r;
    }

    return r;
}

char *parse_flags(u2 flags, size_t n, const char *sep, const FlagMap flag_map[])
{
    size_t sep_len = strlen(sep);
    size_t buffer_size = 256;

    char *flag_str = malloc(buffer_size);
    if (flag_str)
    {
        char *ptr = flag_str;

        for (size_t i = 0; i < n; i++)
        {
            if (flags & flag_map[i].flag)
            {
                size_t l = strlen(flag_map[i].name);

                if (buffer_size > l + 2)
                {
                    strcpy(ptr, flag_map[i].name);
                    ptr += l;
                    buffer_size -= l;
                    strcpy(ptr, sep);
                    ptr += sep_len;
                    buffer_size -= sep_len;
                }
            }
        }

        if (ptr > flag_str)
            ptr -= sep_len; // Remove extra separator
        *ptr = '\0';
    }

    char *tmp = realloc(flag_str, buffer_size);
    if (tmp)
        flag_str = tmp;

    return flag_str;
}

static const struct
{
    const attribute_name attr;
    const wchar_t *str;
} conversion[] = {
    {ConstantValue, L"ConstantValue"},
    {Code, L"Code"},
    {StackMapTable, L"StackMapTable"},
    {Exceptions, L"Exceptions"},
    {InnerClasses, L"InnerClasses"},
    {EnclosingMethod, L"EnclosingMethod"},
    {Synthetic, L"Synthetic"},
    {Signature, L"Signature"},
    {SourceFile, L"SourceFile"},
    {SourceDebugExtension, L"SourceDebugExtension"},
    {LineNumberTable, L"LineNumberTable"},
    {LocalVariableTable, L"LocalVariableTable"},
    {LocalVariableTypeTable, L"LocalVariableTypeTable"},
    {Deprecated, L"Deprecated"},
    {RuntimeVisibleAnnotations, L"RuntimeVisibleAnnotations"},
    {RuntimeInvisibleAnnotations, L"RuntimeInvisibleAnnotations"},
    {RuntimeVisibleParameterAnnotations, L"RuntimeVisibleParameterAnnotations"},
    {RuntimeInvisibleParameterAnnotations, L"RuntimeInvisibleParameterAnnotations"},
    {AnnotationDefault, L"AnnotationDefault"},
    {BootstrapMethods, L"BootstrapMethods"},
    {MethodParameters, L"MethodParameters"}};

const attribute_name *convert_attr_name(const wchar_t *name)
{
    if (name != NULL)
        for (size_t i = 0; i < sizeof(conversion) / sizeof(conversion[0]); i++)
        {
            if (!wcscmp(name, conversion[i].str))
                return &conversion[i].attr;
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

wchar_t *parse_descriptor(const wchar_t *descriptor, wchar_t *sep)
{
    if (descriptor == NULL)
        return NULL;

    unsigned int dim = 0;
    size_t l = wcslen(descriptor);

    wchar_t *buf = (wchar_t *)calloc(256, sizeof(wchar_t));

    if (buf != NULL)
        for (const wchar_t *desc = descriptor; desc < descriptor + l; desc++)
        {
            if (*desc == L'[')
                dim++;
            else
            {
                switch (*desc)
                {
                case L'(':
                    wcsncat(buf, L"(", 1);
                    break;
                case L')':
                    wcsncat(buf, L")", 1);
                    break;
                case L'L':
                    wchar_t *buf_it = buf + wcslen(buf);
                    while (*(++desc) != L';')
                        *(buf_it++) = *desc == L'/' ? L'.' : *desc;
                    *buf_it = L'\0';
                    break;
                default:
                    for (size_t i = 0; i < sizeof(base_type_map) / sizeof(base_type_map[0]); i++)
                        if (*desc == base_type_map[i].base_type)
                            wcscat(buf, base_type_map[i].type);
                    break;
                }

                while (dim > 0)
                {
                    wcscat(buf, L"[]");
                    dim--;
                }

                // Append separator
                if (sep != NULL &&
                    desc > descriptor &&
                    desc + 1 < descriptor + l &&
                    *(desc + 1) != L')' &&
                    *desc != L'[' &&
                    *desc != L'V')
                    wcsncat(buf, sep, 1);

                // Append whitespace
                if (desc < descriptor + l &&
                    *desc != L'(' &&
                    *desc != L')' &&
                    *(desc + 1) != L')')
                    wcsncat(buf, L" ", 1);
            }
        }

    return buf;
}
