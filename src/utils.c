/**
 * @file utils.c
 * @brief Implementações de funções utilitárias usadas pelo projeto.
 *
 * Este módulo fornece utilitários gerais de baixo nível usados por várias
 * partes do interpretador/loader de classes, incluindo:
 *  - operações de troca de ordem de bytes (endianness) para tipos u2/u4;
 *  - contagem de dígitos decimais;
 *  - conversão de flags para string legível;
 *  - mapeamentos e conversões de nomes de atributos;
 *  - análise (parsing) de descritores JVM para uma forma legível.
 *
 * As funções aqui são projetadas para serem simples, seguras e reutilizáveis.
 */

#include "utils.h"

/**
 * @brief Troca a ordem dos dois bytes de um valor de 16 bits armazenado em um unsigned int.
 *
 * Usado para corrigir endianness em leituras de campos u2 quando necessário.
 *
 * @param n Valor original (assume-se que os 16 bits menos significativos contêm o valor u2).
 * @return Valor com os dois bytes trocados.
 */
unsigned int u2swap(const unsigned int n)
{
    return ((n << 8)) | ((n >> 8));
}

/**
 * @brief Troca a ordem dos quatro bytes de um valor de 32 bits armazenado em um unsigned int.
 *
 * Usado para corrigir endianness em leituras de campos u4.
 *
 * @param n Valor original de 32 bits.
 * @return Valor com a ordem dos bytes invertida (big-endian <-> little-endian).
 */
unsigned int u4swap(const unsigned int n)
{
    return ((n >> 24)) |           // Troca o byte 3 com o byte 0
           ((n >> 8) & 0xff00) |   // Troca o byte 1 com o byte 2
           ((n << 8) & 0xff0000) | // Troca o byte 2 com o byte 1
           ((n << 24));            // Troca o byte 0 com o byte 3
}

/**
 * @brief Calcula quantos dígitos decimais tem o número fornecido.
 *
 * Ex.: num_digits(9) -> 1, num_digits(10) -> 2, num_digits(123) -> 3.
 *
 * @param n Número não-negativo.
 * @return Quantidade de dígitos decimais de `n`.
 */
unsigned int num_digits(unsigned int n)
{
    if (n < 10)
        return 1;

    int r = 2;
    while ((n /= 10) > 9)
        ++r;

    return r;
}

/**
 * @brief Converte um conjunto de flags (bitmask) em uma string contendo os nomes das flags setadas.
 *
 * A função varre o array `flag_map` que contém pares (flag, nome) e concatena os nomes das
 * flags presentes em `flags`, separados por `sep`. A string retornada é alocada dinamicamente
 * e deve ser liberada pelo chamador (free).
 *
 * @param flags Máscara de flags (u2).
 * @param n Número de entradas em `flag_map`.
 * @param sep Separador usado entre nomes (ex.: ", ").
 * @param flag_map Array de `FlagMap` que mapeia cada bit a um nome textual.
 * @return Ponteiro para string alocada dinamicamente com os nomes das flags ou NULL em caso de falha.
 */
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

/**
 * @brief Mapeamento interno entre nomes de atributos (string) e o enum attribute_name.
 *
 * Esta tabela é usada por convert_attr_name() para traduzir o nome textual de um atributo
 * (por exemplo, "Code") para o identificador `attribute_name` correspondente.
 */
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

/**
 * @brief Converte o nome textual de um atributo para o enum `attribute_name`.
 *
 * Procura na tabela `conversion` e retorna um ponteiro para o valor enum correspondente.
 *
 * @param name Nome textual do atributo (ex.: "Code"). Pode ser NULL.
 * @return Ponteiro para o `attribute_name` correspondente caso encontrado, ou NULL caso contrário.
 */
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

/**
 * @brief Mapeamento entre código base de tipo (char) e string legível (ex.: 'I' -> "int").
 *
 * Usado pela função parse_descriptor para traduzir tipos primitivos do descritor JVM.
 */
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

/**
 * @brief Converte um descritor JVM em uma representação legível.
 *
 * Exemplos:
 *  - "(I)V"   -> "(int) void"
 *  - "([I)I"  -> "([int]) int" (ou forma similar)
 *  - "(Ljava/lang/String;)V" -> "(java.lang.String) void"
 *
 * A string retornada é alocada com calloc e deve ser liberada pelo chamador.
 *
 * @param descriptor String com o descritor no formato JVM (ex.: "(ILjava/lang/String;)[I").
 * @param sep  Separador a ser usado entre parâmetros (ex.: ","). Se NULL, usa um espaço simples.
 * @return Ponteiro para string alocada contendo a forma legível do descritor, ou NULL se `descriptor` for NULL.
 */
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
                {
                    char *buf_it = buf + strlen(buf);
                    while (*(++desc) != L';')
                        *(buf_it++) = *desc == L'/' ? L'.' : *desc;
                    *buf_it = L'\0';
                    break;
                }
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
