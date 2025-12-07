/**
 * @file src/cp/parser.c
 * @brief Implementação do parser do constant pool e utilitários de decodificação.
 *
 * Contém:
 *  - parse_constant_pool: interpreta as entradas do constant_pool conforme a JVM Spec
 *  - decode_modified_utf8_str: decodifica strings em Modified UTF-8 (formato JVM)
 *  - decode_float_bytes / decode_long_bytes / decode_double_bytes: reconstróem
 *    valores numéricos (float/long/double) a partir das palavras lidas do .class.
 *
 * Observações importantes:
 * - Funções que alocam memória (por exemplo, para strings) retornam buffers que devem
 *   ser liberados pelo chamador (free).
 * - A função parse_constant_pool aloca um array de cp_info e possíveis buffers
 *   internos (ex.: bytes/strings). A liberação apropriada deve ser feita por
 *   free_classfile() ou função equivalente.
 */

#include "parser.h"
#include "reader.h"

cp_info *parse_constant_pool(FILE *fptr, u2 count)
{
    cp_info *constant_pool = (cp_info *)malloc(sizeof(cp_info) * count);
    cp_info *cp = constant_pool;

    /* Percorre as entradas do constant pool. A especificação diz que a tabela
       tem 'count-1' entradas válidas (índice 1..count-1), por isso o laço
       usa constant_pool + count - 1 como limite. */
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
            /* Inteiros e floats ocupam 4 bytes */
            u4 ibytes = read_u4(fptr);
            cp->info._4Bn.bytes = ibytes;
            if (cp->tag == CONSTANT_Integer)
                cp->info._4Bn.number.i = ibytes;
            else
                cp->info._4Bn.number.f = decode_float_bytes(ibytes);
        }
        break;

        case CONSTANT_Long:
        case CONSTANT_Double:
        {
            /* Long e Double ocupam 8 bytes (duas entradas no constant pool) */
            u4 high_bytes = read_u4(fptr), low_bytes = read_u4(fptr);
            cp->info._8Bn.high_bytes = high_bytes;
            cp->info._8Bn.low_bytes = low_bytes;

            if (cp->tag == CONSTANT_Long)
                cp->info._8Bn.number.l = decode_long_bytes(high_bytes, low_bytes);
            else
                cp->info._8Bn.number.d = decode_double_bytes(high_bytes, low_bytes);

            /* Por especificação, essas constantes ocupam duas entradas na tabela */
            cp++;
        }
        break;

        case CONSTANT_NameAndType:
            cp->info.NameAndType.name_index = read_u2(fptr);
            cp->info.NameAndType.descriptor_index = read_u2(fptr);
            break;

        case CONSTANT_UTF8:
        {
            /* UTF8: primeiro lê-se o comprimento em bytes, depois os bytes */
            u2 l = read_u2(fptr);
            cp->info.UTF8.length = l;

            u1 *bytes = (u1 *)malloc(sizeof(u1) * l);
            u1 *b;

            for (b = bytes; b < bytes + l; b++)
            {
                *b = read_u1(fptr);
            }

            /* Aqui o código original fazia b -= l; e atribuia cp->info.UTF8.bytes = b;
               isso resultaria em atribuir o ponteiro de bytes - l (um bug). Para manter
               compatibilidade com o seu código, preservei a lógica original: */
            b -= l;
            cp->info.UTF8.bytes = b;

            /* Decodifica modified UTF-8 para wchar_t* */
            cp->info.UTF8.str = decode_modified_utf8_str(l, b);
        }
        break;

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
            /* Entrada desconhecida: ignora/avisa (comportamento atual: apenas quebra) */
            break;
        }

        cp++;
    }

    /* Retorna o ponteiro para o início do array (compatível com o original). */
    return cp - (count - 1);
}

/**
 * @brief Decodifica uma string em Modified UTF-8 para wchar_t*.
 *
 * A JVM usa uma variante chamada Modified UTF-8 onde o caractere NUL é codificado
 * como 0xC0 0x80. Esta função analisa a sequência de bytes e converte para um buffer
 * wide (wchar_t*). O resultado é alocado dinamicamente e deve ser liberado com free().
 *
 * @param length Número de bytes no buffer `bytes`.
 * @param bytes Ponteiro para os bytes codificados em Modified UTF-8.
 * @return wchar_t* String wide (terminada em L'\\0') alocada dinamicamente ou NULL em erro.
 */
wchar_t *decode_modified_utf8_str(u2 length, const u1 *bytes)
{
    if (bytes == NULL)
    {
        return NULL;
    }

    size_t buffer_size = 0, pos = 0;

    /* Primeiro passo: determinar quantos wchar_t serão necessários para decodificar. */
    while (pos < length)
    {
        u1 x = bytes[pos];

        if (x < 0x80)
        {
            /* 1-byte ASCII */
            buffer_size++;
            pos++;
        }
        else if ((x & 0xE0) == 0xC0)
        {
            /* 2-byte sequence (including modified UTF-8 NUL encoding) */
            if (pos + 1 >= length)
                return NULL; /* sequência truncada */
            buffer_size++;
            pos += 2;
        }
        else if ((x & 0xF0) == 0xE0)
        {
            /* 3-byte sequence */
            if (pos + 2 >= length)
                return NULL; /* truncado */
            buffer_size++;
            pos += 3;
        }
        else
        {
            /* sequência inválida */
            return NULL;
        }
    }

    wchar_t *str = (wchar_t *)malloc((buffer_size + 1) * sizeof(wchar_t));
    if (str == NULL)
        return NULL;

    size_t i = pos = 0;

    /* Decodificação efetiva */
    while (pos < length)
    {
        u1 x = bytes[pos];

        if (x < 0x80)
        {
            str[i++] = (wchar_t)x;
            pos++;
        }
        else if ((x & 0xE0) == 0xC0)
        {
            u1 y = bytes[pos + 1];
            u2 code_point = ((x & 0x1F) << 6) | (y & 0x3F);

            /* No Modified UTF-8, o NUL é codificado como 0xC0 0x80; restauramos NUL
               mapeando code_point == 0 para L'\0' explicitamente. */
            str[i++] = code_point == 0 ? L'\0' : (wchar_t)code_point;
            pos += 2;
        }
        else if ((x & 0xF0) == 0xE0)
        {
            u1 y = bytes[pos + 1];
            u1 z = bytes[pos + 2];
            u2 code_point = ((x & 0xF) << 12) | ((y & 0x3F) << 6) | (z & 0x3F);
            str[i++] = (wchar_t)code_point;
            pos += 3;
        }
    }

    str[i] = L'\0';
    return str;
}

/**
 * @brief Reconstrói um float (IEEE 754) a partir de 4 bytes (u4).
 *
 * Trata casos especiais (Infinitos e NaN) e calcula o valor normal caso contrário.
 *
 * @param b Palavra de 32 bits contendo a representação do float.
 * @return float Valor reconstruído.
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
        u4 s, e, m;
        s = ((b >> 31) == 0) ? 1 : -1;
        e = ((b >> 23) & 0xFF);
        m = (e == 0) ? (b & 0x7FFFFF) << 1 : (b & 0x7FFFFF) | 0x800000;

        /* 2^(e-127) multiplicado pelo mantissa normalizada; a expressão (1 << (e-150))
           é equivalente a 2^(e-150) * 2^23 usado em combinação com m */
        return s * m * (1 << (e - 150));
    }
}

/**
 * @brief Combina duas palavras (alta/baixa) em um valor long (64 bits).
 *
 * @param hb Parte alta (u4).
 * @param lb Parte baixa (u4).
 * @return long Valor de 64 bits resultante.
 */
long decode_long_bytes(u4 hb, u4 lb)
{
    return ((long)hb << 32) | lb;
}

/**
 * @brief Reconstrói um double (IEEE 754, 64 bits) a partir de duas palavras u4.
 *
 * Trata casos especiais (Infinitos e NaN) e calcula o valor normal caso contrário.
 *
 * @param hb Parte alta (u4).
 * @param lb Parte baixa (u4).
 * @return double Valor double reconstruído.
 */
double decode_double_bytes(u4 hb, u4 lb)
{
    long b = decode_long_bytes(hb, lb);

    if (b == 0x7FF0000000000000L)
        return INFINITY;
    else if (b == (long)0xFFF0000000000000L)
        return -INFINITY;
    else if (((0x7FF0000000000001L <= b) && (b <= 0x7FFFFFFFFFFFFFFFL)) ||
             (((long)0xFFF0000000000001L <= b) && (b <= (long)0xFFFFFFFFFFFFFFFFL)))
        return NAN;
    else
    {
        u4 s, e, m;
        s = ((b >> 63) == 0) ? 1 : -1;
        e = (int)((b >> 52) & 0x7FFL);
        m = (e == 0) ? (b & 0xFFFFFFFFFFFFFL) << 1 : (b & 0xFFFFFFFFFFFFFL) | 0x10000000000000L;

        /* similar ao float, ajustando expoente/mantissa para 64 bits */
        return s * m * (1 << (e - 1075));
    }
}
