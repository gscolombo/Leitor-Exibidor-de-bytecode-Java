/**
 * @file utils.c
 * @brief Implementação de funções utilitárias para manipulação de endianness,
 *        conversão de flags, análise de nomes de atributos e interpretação
 *        de descritores da JVM.
 *
 * Este módulo contém funções de suporte utilizadas por todo o leitor/exibidor:
 *  - u2swap e u4swap: conversão entre little e big endian
 *  - num_digits: contagem de dígitos de inteiros
 *  - parse_flags: conversão de flags da JVM em string
 *  - convert_attr_name: mapeamento de nomes de atributos para enums
 *  - parse_descriptor: conversão do descritor JVM para forma legível
 */

#include "utils.h"

/**
 * @brief Inverte os dois bytes de um valor de 16 bits.
 *
 * Útil quando valores lidos do arquivo .class (big-endian) precisam ser tratados
 * em arquitetura little-endian.
 *
 * @param n Valor de 16 bits.
 * @return Valor com bytes trocados.
 */
unsigned int u2swap(const unsigned int n)
{
    return ((n >> 8) & 0xff) |      /* Byte alto vai para baixo   */
           ((n << 8) & 0xff00);     /* Byte baixo vai para cima   */
}

/**
 * @brief Inverte a ordem dos quatro bytes de um valor de 32 bits.
 *
 * Exemplo:
 * - Entrada: 0x12345678  
 * - Saída:   0x78563412
 *
 * @param n Valor de 32 bits.
 * @return Valor com bytes invertidos.
 */
unsigned int u4swap(const unsigned int n)
{
    return ((n >> 24) & 0xff)        | /* Byte 3 → Byte 0 */
           ((n >> 8)  & 0xff00)      | /* Byte 2 → Byte 1 */
           ((n << 8)  & 0xff0000)    | /* Byte 1 → Byte 2 */
           ((n << 24) & 0xff000000);   /* Byte 0 → Byte 3 */
}

/**
 * @brief Retorna o número de dígitos de um inteiro positivo.
 *
 * Exemplo:
 * - n = 5   → 1
 * - n = 42  → 2
 * - n = 999 → 3
 *
 * @param n Inteiro não negativo.
 * @return Quantidade de dígitos de `n`.
 */
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

/**
 * @brief Converte flags da JVM (u2) em uma string legível.
 *
 * Dado um conjunto de bits representando flags (ex.: ACC_PUBLIC, ACC_STATIC),
 * esta função combina os nomes das flags reconhecidas em um texto,
 * separados pelo `sep` fornecido.
 *
 * A string retornada é alocada dinamicamente e deve ser liberada com `free()`.
 *
 * @param flags Valor de flags (u2).
 * @param n Quantidade de itens no mapa de flags.
 * @param sep Separador entre nomes das flags (ex.: " " ou ", ").
 * @param flag_map Array de FlagMap com {bit,name}.
 * @return char* String formatada contendo as flags ativas.
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

        /* Remove separador extra ao final */
        if (ptr > flag_str)
            ptr -= sep_len;

        *ptr = '\0';
    }

    /* Ajusta o buffer para o tamanho real usado */
    char *tmp = realloc(flag_str, buffer_size);
    if (tmp)
        flag_str = tmp;

    return flag_str;
}

/**
 * @brief Tabela de conversão entre wide-string de nomes de atributos e enum attribute_name.
 *
 * Usada por convert_attr_name().
 */
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
    {MethodParameters, L"MethodParameters"}
};

/**
 * @brief Converte um nome de atributo (wide-string) para o enum correspondente.
 *
 * Exemplo:
 * - "Code" → attribute_name::Code  
 * - "LineNumberTable" → attribute_name::LineNumberTable  
 *
 * @param name Nome do atributo em wchar_t (UTF-16/UTF-32 dependendo da plataforma).
 * @return Ponteiro para o enum correspondente ou NULL se desconhecido.
 */
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

/**
 * @brief Tabela de correspondência entre códigos de descritor da JVM e nomes legíveis.
 */
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
    {L'V', L"void"}
};

/**
 * @brief Converte descritores da JVM para representações legíveis.
 *
 * Exemplos:
 * - "I"      → "int"
 * - "[I"     → "int[]"
 * - "Ljava/lang/String;" → "java.lang.String"
 * - "(IF)V"  → "( int float ) void"
 *
 * O parâmetro `sep` é inserido entre múltiplos tipos (por exemplo, em parâmetros
 * de métodos).
 *
 * @param descriptor Descritor JVM em wchar_t.
 * @param sep Wide-char separador (normalmente L"," ou L";").
 * @return wchar_t* String alocada dinamicamente contendo o texto interpretado.
 *         Deve ser liberada com free().
 */
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
            {
                dim++;
            }
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

                case L'L': /* Objeto: Lfully/qualified/ClassName; */
                {
                    wchar_t *buf_it = buf + wcslen(buf);
                    while (*(++desc) != L';')
                        *(buf_it++) = (*desc == L'/') ? L'.' : *desc;

                    *buf_it = L'\0';
                }
                break;

                default: /* Tipo primitivo */
                    for (size_t i = 0; i < sizeof(base_type_map) / sizeof(base_type_map[0]); i++)
                        if (*desc == base_type_map[i].base_type)
                            wcscat(buf, base_type_map[i].type);
                }

                /* Arrays */
                while (dim > 0)
                {
                    wcscat(buf, L"[]");
                    dim--;
                }

                /* Insere separador, se apropriado */
                if (sep != NULL &&
                    desc > descriptor &&
                    desc + 1 < descriptor + l &&
                    *(desc + 1) != L')' &&
                    *desc != L'[' &&
                    *desc != L'V')
                {
                    wcsncat(buf, sep, 1);
                }

                /* Insere espaço */
                if (desc < descriptor + l &&
                    *desc != L'(' &&
                    *desc != L')' &&
                    *(desc + 1) != L')')
                {
                    wcsncat(buf, L" ", 1);
                }
            }
        }

    return buf;
}
