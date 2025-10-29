#include "parser.h"
#include "reader.h"

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
        case CONSTANT_Float: {
            u4 ibytes = read_u4(fptr);
            cp->info._4Bn.bytes = ibytes;
            if (cp->tag == CONSTANT_Integer)
                cp->info._4Bn.number.i = ibytes;
            else
                cp->info._4Bn.number.f = decode_float_bytes(ibytes);
            break;
        }

        case CONSTANT_Long:
        case CONSTANT_Double: {
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

        case CONSTANT_UTF8: {
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
            str[i++] = code_point == 0 ? L'\0' : (char)code_point; // The null character is represented by two bytes (0xC0,0x80);
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

    str[i] = L'\0';
    return str;
}

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
        int s, e, m;
        s = ((b >> 31) == 0) ? 1 : -1;
        e = ((b >> 23) & 0xFF);
        m = (e == 0) ? (b & 0x7FFFFF) << 1 : (b & 0x7FFFFF) | 0x800000;

        return s * m * pow(2, e - 150);
    }
}

long decode_long_bytes(u4 hb, u4 lb)
{
    return ((long)hb << 32) | lb;
}

double decode_double_bytes(u4 hb, u4 lb)
{
    long b = decode_long_bytes(hb, lb);

    if (b == 0x7ff0000000000000L)
        return INFINITY;
    else if (b == (long)0xfff0000000000000L)
        return -INFINITY;
    else if (((0x7ff0000000000001L <= b) && (b <= 0x7fffffffffffffffL)) ||
             (((long)0xfff0000000000001L <= b) && (b <= (long)0xffffffffffffffffL)))
        return NAN;
    else
    {
        int s, e;
        long m;
        s = ((b >> 63) == 0) ? 1 : -1;
        e = (int)((b >> 52) & 0x7ffL);
        m = (e == 0) ? (b & 0xfffffffffffffL) << 1 : (b & 0xfffffffffffffL) | 0x10000000000000L;

        return s * m * pow(2, e - 1075);
    }
}