#include "utils.h"

unsigned int u2swap(const unsigned int n)
{
    return ((n >> 8) & 0xff) |  // Troca o byte 0 com o byte 1
           ((n << 8) & 0xff00); // Troca o byte 1 com o byte 0
}

unsigned int u4swap(const unsigned int n)
{
    /*
        Na expressão abaixo, os operadores "|" concatenam
        os segmentos de bytes da palavra (32 bits), enquanto o operador "&"
        usado com "0x...ff..." escolhe o byte a ser movido.

        Exemplo:
        n = 0x12345678
        a <= ((n>>24)&0xff) = 0x00000012 & 0x000000ff = (...0 0001 0010) & (...0 1111 1111) = (...0 0001 0010) = 0x00000012
        b <= ((n<<8)&0xff0000) = 0x34567800 & 0x00ff0000 = (0011 0100 0101 0110 0111 1000 0...) & (0000 0000 1111 1111 0...) = (0000 0000 0101 0110 0...) = 0x00560000
        a | b <= 0x00000012 | 0x00560000 = 0x00560012
        O mesmo para os bytes 2 e 3
    */
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

char *get_access_flags(u2 flags, size_t length, const FlagMap flag_map[])
{
    const char *names[length];

    size_t l = 0;

    for (size_t i = 0; i < length; i++)
    {
        if (flags & flag_map[i].flag)
        {
            names[l] = flag_map[i].name;
            l++;
        }
    }

    char *flag_str = (char *)calloc(l + 1, sizeof(char *));
    char *tmp = (char *)malloc(20);

    if (flag_str != NULL && tmp != NULL)
    {
        for (size_t i = 0; i < l; i++)
        {
            if (i > 0)
            {
                snprintf(tmp, strlen(names[i]) + 3, ", %s", names[i]);
                strcat(flag_str, tmp);
                continue;
            }

            snprintf(flag_str, strlen(names[i]) + 1, "%s", names[i]);
        }

        free(tmp);
        return flag_str;
    }

    return NULL;
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