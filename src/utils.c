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
    const char *str;
} conversion[] = {
    {ConstantValue, "ConstantValue"},
    {Code, "Code"},
    {StackMapTable, "StackMapTable"},
    {Exceptions, "Exceptions"},
    {InnerClasses, "InnerClasses"},
    {EnclosingMethod, "EnclosingMethod"},
    {Synthetic, "Synthetic"},
    {Signature, "Signature"},
    {SourceFile, "SourceFile"},
    {SourceDebugExtension, "SourceDebugExtension"},
    {LineNumberTable, "LineNumberTable"},
    {LocalVariableTable, "LocalVariableTable"},
    {LocalVariableTypeTable, "LocalVariableTypeTable"},
    {Deprecated, "Deprecated"},
    {RuntimeVisibleAnnotations, "RuntimeVisibleAnnotations"},
    {RuntimeInvisibleAnnotations, "RuntimeInvisibleAnnotations"},
    {RuntimeVisibleParameterAnnotations, "RuntimeVisibleParameterAnnotations"},
    {RuntimeInvisibleParameterAnnotations, "RuntimeInvisibleParameterAnnotations"},
    {AnnotationDefault, "AnnotationDefault"},
    {BootstrapMethods, "BootstrapMethods"},
    {MethodParameters, "MethodParameters"}};

const attribute_name *convert_attr_name(const char *name)
{
    if (name != NULL)
        for (size_t i = 0; i < sizeof(conversion) / sizeof(conversion[0]); i++)
        {
            if (!strcmp(name, conversion[i].str))
                return &conversion[i].attr;
        }

    return NULL;
}

static const struct
{
    char base_type;
    char *type;
} base_type_map[9] = {
    {L'B', "byte"},
    {L'C', "char"},
    {L'D', "double"},
    {L'F', "float"},
    {L'I', "int"},
    {L'J', "long"},
    {L'S', "short"},
    {L'Z', "boolean"},
    {L'V', "void"}};

char *parse_descriptor(const char *descriptor, char *sep)
{
    if (descriptor == NULL)
        return NULL;

    unsigned int dim = 0;
    size_t l = strlen(descriptor);

    char *buf = (char *)calloc(256, sizeof(char));

    if (buf != NULL)
        for (const char *desc = descriptor; desc < descriptor + l; desc++)
        {
            if (*desc == L'[')
                dim++;
            else
            {
                switch (*desc)
                {
                case L'(':
                    strcat(buf, "(");
                    break;
                case L')':
                    strcat(buf, ")");
                    break;
                case L'L':
                    char *buf_it = buf + strlen(buf);
                    while (*(++desc) != L';')
                        *(buf_it++) = *desc == L'/' ? L'.' : *desc;
                    *buf_it = L'\0';
                    break;
                default:
                    for (size_t i = 0; i < sizeof(base_type_map) / sizeof(base_type_map[0]); i++)
                        if (*desc == base_type_map[i].base_type)
                            strcat(buf, base_type_map[i].type);
                    break;
                }

                while (dim > 0)
                {
                    strcat(buf, "[]");
                    dim--;
                }

                // Append separator
                if (sep != NULL &&
                    desc > descriptor &&
                    desc + 1 < descriptor + l &&
                    *(desc + 1) != L')' &&
                    *desc != L'[' &&
                    *desc != L'V')
                    strncat(buf, sep, 1);

                // Append whitespace
                if (desc < descriptor + l &&
                    *desc != L'(' &&
                    *desc != L')' &&
                    *(desc + 1) != L')')
                    strcat(buf, " ");
            }
        }

    return buf;
}