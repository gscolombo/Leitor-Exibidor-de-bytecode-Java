/**
 * @file cp/parser.c
 * @brief Parser da Constant Pool e utilitários para decodificação de tipos primitivos e UTF-8 modificado.
 *
 * Este módulo lê e converte as entradas da Constant Pool a partir de um arquivo
 * `.class` (fluxo FILE*) para um array de `cp_info`. Também fornece funções
 * auxiliares para decodificar a Modified UTF-8 usada na constant pool e para
 * decodificar representações em bytes de `float`, `long` e `double` conforme
 * a especificação IEEE 754 e a especificação de class files da JVM.
 *
 * Observações importantes:
 *  - A função `parse_constant_pool` aloca um array de `cp_info` cujo ponteiro é
 *    retornado; o chamador é responsável por liberar essa memória (e conteúdos
 *    internos) posteriormente (ver `free_classfile` / `free_attributes`).
 *  - Entradas `CONSTANT_Long` e `CONSTANT_Double` ocupam duas posições na constant
 *    pool (a segunda é considerada invalida/reservada). O parser avança um slot
 *    extra para refletir essa reserva.
 *  - `decode_modified_utf8_str` retorna uma nova string C (`malloc`) que deve ser
 *    liberada pelo chamador. Em caso de sequência UTF truncada ou inválida, retorna NULL.
 */

#include "parser.h"

cp_info *parse_constant_pool(FILE *fptr, u2 count)
{
    cp_info *constant_pool = (cp_info *)malloc(sizeof(cp_info) * count);
    cp_info *cp = constant_pool;

    while (cp < constant_pool + count - 1)
    {
        cp->tag = read_u1(fptr);
        switch (cp->tag)
        {
        case CONSTANT_Class:
            cp->info.Class.name_index = read_u2(fptr);
            break;

        case CONSTANT_Fieldref:
        case CONSTANT_Methodref:
        case CONSTANT_InterfaceMethodref:
            cp->info.Ref.class_index = read_u2(fptr);
            cp->info.Ref.name_and_type_index = read_u2(fptr);
            break;

        case CONSTANT_String:
            cp->info.String.string_index = read_u2(fptr);
            break;

        case CONSTANT_Integer:
        case CONSTANT_Float:
        {
            u4 ibytes = read_u4(fptr);
            cp->info._4Bn.bytes = ibytes;
            if (cp->tag == CONSTANT_Integer)
                cp->info._4Bn.number.i = ibytes;
            else
                cp->info._4Bn.number.f = decode_float_bytes(ibytes);
            break;
        }

        case CONSTANT_Long:
        case CONSTANT_Double:
        {
            u4 high_bytes = read_u4(fptr);
            u4 low_bytes = read_u4(fptr);

            cp->info._8Bn.high_bytes = high_bytes;
            cp->info._8Bn.low_bytes = low_bytes;

            if (cp->tag == CONSTANT_Long)
                cp->info._8Bn.number.l = decode_long_bytes(high_bytes, low_bytes);
            else
                cp->info._8Bn.number.d = decode_double_bytes(high_bytes, low_bytes);

            cp++; // Skip extra slot for 8-byte constants
            break;
        }

        case CONSTANT_NameAndType:
            cp->info.NameAndType.name_index = read_u2(fptr);
            cp->info.NameAndType.descriptor_index = read_u2(fptr);
            break;

        case CONSTANT_UTF8:
        {
            u2 l = read_u2(fptr);
            cp->info.UTF8.length = l;

            u1 *bytes = (u1 *)malloc(sizeof(u1) * l);
            u1 *b;

            for (b = bytes; b < bytes + l; b++)
                *b = read_u1(fptr);

            b -= l;
            cp->info.UTF8.bytes = b;
            cp->info.UTF8.str = decode_modified_utf8_str(l, b);
            break;
        }

        case CONSTANT_MethodHandle:
            cp->info.MethodHandle.reference_kind = read_u1(fptr);
            cp->info.MethodHandle.reference_index = read_u2(fptr);
            break;

        case CONSTANT_MethodType:
            cp->info.MethodType.descriptor_index = read_u2(fptr);
            break;

        case CONSTANT_InvokeDynamic:
            cp->info.InvokeDynamic.bootstrap_method_attr_index = read_u2(fptr);
            cp->info.InvokeDynamic.name_and_type_index = read_u2(fptr);
            break;

        default:
            break;
        }

        cp++;
    }

    return constant_pool;
}

/**
 * @brief Decodifica uma string em Modified UTF-8 (formato usado nas constant pools) para uma string C.
 *
 * A Modified UTF-8 usada nos class files difere do UTF-8 padrão principalmente na forma
 * de codificar o caractere nulo (U+0000) e em sequences válidas. Esta função:
 *  - verifica o comprimento e a validade das sequências multibyte antes de decodificar;
 *  - aloca e retorna uma nova string C (terminada em '\0') com o conteúdo decodificado;
 *  - retorna NULL em caso de `bytes == NULL` ou se encontrar uma sequência truncada/inválida.
 *
 * @param length Comprimento em bytes do array `bytes`.
 * @param bytes Ponteiro para os bytes codificados em Modified UTF-8.
 * @return Ponteiro para string alocada (malloc) contendo o texto decodificado, ou NULL em caso de erro.
 */
char *decode_modified_utf8_str(u2 length, const u1 *bytes)
{
    if (bytes == NULL)
    {
        return NULL;
    }

    size_t buffer_size = 0, pos = 0;

    // Define buffer size
    while (pos < length)
    {
        u1 x = bytes[pos];

        if (x < 0x80) // 1 byte code point
        {
            buffer_size++;
            pos++;
        }
        else if ((x & 0xE0) == 0xC0) // 2 bytes code point
        {                            // Check if high byte starts with 110
            if (pos + 1 >= length)
                return NULL; // Early return if truncated sequence
            buffer_size++;
            pos += 2;
        }
        else if ((x & 0xF0) == 0xE0) // 3 bytes code point
        {                            // Check if high byte starts with 1110
            if (pos + 2 >= length)
                return NULL; // Same as above
            buffer_size++;
            pos += 3;
        }
        else
        {
            return NULL;
        }
    }

    char *str = (char *)malloc((buffer_size + 1) * sizeof(char));
    if (str == NULL)
        return NULL;

    size_t i = pos = 0;

    // Decode bytes
    while (pos < length)
    {
        u1 x = bytes[pos];

        if (x < 0x80)
        {
            str[i++] = (char)x;
            pos++;
        }
        else if ((x & 0xE0) == 0xC0)
        {
            u1 y = bytes[pos + 1];
            u2 code_point = ((x & 0x1F) << 6) | (y & 0x3F);
            str[i++] = code_point == 0 ? '\0' : (char)code_point; // The null character is represented by two bytes (0xC0,0x80);
            pos += 2;
        }
        else if ((x & 0xF0) == 0xE0)
        {
            u1 y = bytes[pos + 1];
            u1 z = bytes[pos + 2];
            u2 code_point = ((x & 0xF) << 12) | ((y & 0x3F) << 6) | (z & 0x3F);
            str[i++] = (char)code_point;
            pos += 3;
        }
    }

    str[i] = '\0';
    return str;
}

/**
 * @brief Decodifica um valor `float` a partir de sua representação em 4 bytes (u4).
 *
 * Interpreta `b` como a representação IEEE 754 de precisão simples. Trata
 * padrões especiais (Infinity, -Infinity, NaN) explicitamente; para valores
 * normais reconstroi o sinal, expoente e mantissa e calcula o valor real.
 *
 * @param b Valor u4 contendo os bytes do float conforme class file.
 * @return Valor `float` correspondente (pode ser +Inf, -Inf, NaN ou número finito).
 */
float decode_float_bytes(u4 b)
{
    if (b == 0x7F800000)
        return INFINITY;
    else if (b == 0xFF800000)
        return -INFINITY;
    else if (((0x7F800001 <= b) && (b <= 0x7FFFFFFF)) || ((0xFF800001 <= b) && (b <= 0xFFFFFFFF)))
        return NAN;
    else
    {
        int32_t s, e, m;
        s = ((b >> 31) == 0) ? 1 : -1;
        e = ((b >> 23) & 0xFF);
        m = (e == 0) ? (b & 0x7FFFFF) << 1 : (b & 0x7FFFFF) | 0x800000;

        return s * m * pow(2, e - 150);
    }
}

/**
 * @brief Decodifica um valor `long` a partir de dois campos u4 (high_bytes, low_bytes).
 *
 * Concatena os bytes altos e baixos em um `int64_t` com sinal, retornando o valor.
 *
 * @param hb High bytes (u4).
 * @param lb Low bytes (u4).
 * @return Valor `int64_t` reconstruído.
 */
int64_t decode_long_bytes(u4 hb, u4 lb)
{
    return ((int64_t)hb << 32) | lb;
}

/**
 * @brief Decodifica um valor `double` a partir de dois campos u4 (high_bytes, low_bytes).
 *
 * Reconstrói o word de 64 bits, trata casos especiais (Infinity, -Infinity, NaN)
 * e, para valores normais, reconstroi sinal, expoente e mantissa para calcular
 * o `double` conforme IEEE 754.
 *
 * @param hb High bytes (u4).
 * @param lb Low bytes (u4).
 * @return Valor `double` correspondente (pode ser +Inf, -Inf, NaN ou número finito).
 */
double decode_double_bytes(u4 hb, u4 lb)
{
    int64_t b = decode_long_bytes(hb, lb);

    if (b == 0x7ff0000000000000L)
        return INFINITY;
    else if (b == (long)0xfff0000000000000L)
        return -INFINITY;
    else if (((0x7ff0000000000001L <= b) && (b <= 0x7fffffffffffffffL)) ||
             (((int64_t)0xfff0000000000001L <= b) && (b <= (int64_t)0xffffffffffffffffL)))
        return NAN;
    else
    {
        int32_t s, e;
        int64_t m;
        s = ((b >> 63) == 0) ? 1 : -1;
        e = (int32_t)((b >> 52) & 0x7ffL);
        m = (e == 0) ? (b & 0xfffffffffffffL) << 1 : (b & 0xfffffffffffffL) | 0x10000000000000L;

        return s * m * pow(2, e - 1075);
    }
}
