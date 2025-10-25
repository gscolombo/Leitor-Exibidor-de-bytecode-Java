#include "fields.h"
#include <inttypes.h>
#include "types/cp/constants.h"
#include "types/attributes/attribute_info.h"
#include "types/attributes/attributes.h"

static const FlagMap flag_map[9] = {
    {0x0001, "ACC_PUBLIC"},
    {0x0002, "ACC_PRIVATE"},
    {0x0004, "ACC_PROTECTED"},
    {0x0008, "ACC_STATIC"},
    {0x0010, "ACC_FINAL"},
    {0x0040, "ACC_VOLATILE"},
    {0x0080, "ACC_TRANSIENT"},
    {0x1000, "ACC_SYNTHETIC"},
    {0x4000, "ACC_ENUM"}};

static const FlagMap flag_kw_map[7] = {
    {0x0001, "public"},
    {0x0002, "private"},
    {0x0004, "protected"},
    {0x0008, "static"},
    {0x0010, "final"},
    {0x0040, "volatile"},
    {0x0080, "transient"}};

// Converte (high, low) -> double sem UB
static double u8_to_double(u4 high, u4 low) {
    uint64_t bits = ((uint64_t)high << 32) | (uint64_t)low;
    double d;
    memcpy(&d, &bits, sizeof(double));
    return d;
}

// Converte (high, low) -> int64_t sem UB (javap imprime long como signed)
static int64_t u8_to_int64(u4 high, u4 low) {
    uint64_t u = ((uint64_t)high << 32) | (uint64_t)low;
    int64_t s;
    memcpy(&s, &u, sizeof(s));
    return s;
}

// Converte u4 -> float sem UB
static float u4_to_float(u4 x) {
    float f;
    memcpy(&f, &x, sizeof(float));
    return f;
}

static void print_constant_value(const ClassFile *cf, u2 constant_index) {
    if (constant_index == 0 || constant_index > cf->constant_pool_count) {
        printf("  [ConstantValue: <invalid cp index #%u>]\n", constant_index);
        return;
    }

    cp_info *cp = cf->constant_pool;
    cp_info *entry = &cp[constant_index - 1];

    switch (entry->tag) {
        case CONSTANT_Integer: {
            // no seu constants.h: info._4Bn.bytes e info._4Bn.number.i
            int32_t v;
            memcpy(&v, &entry->info._4Bn.bytes, sizeof(v));
            printf("  [ConstantValue: %d]\n", v);
            break;
        }
        case CONSTANT_Float: {
            float f = u4_to_float(entry->info._4Bn.bytes);
            printf("  [ConstantValue: %g]\n", f);
            break;
        }
        case CONSTANT_Long: {
            int64_t v = u8_to_int64(entry->info._8Bn.high_bytes, entry->info._8Bn.low_bytes);
            printf("  [ConstantValue: %" PRId64 "]\n", v);
            break;
        }
        case CONSTANT_Double: {
            double d = u8_to_double(entry->info._8Bn.high_bytes, entry->info._8Bn.low_bytes);
            printf("  [ConstantValue: %g]\n", d);
            break;
        }
        case CONSTANT_String: {
            u2 utf8_index = entry->info.String.string_index;
            if (utf8_index >= 1 && utf8_index <= cf->constant_pool_count &&
                cp[utf8_index - 1].tag == CONSTANT_UTF8) {
                const char *s = cp[utf8_index - 1].info.UTF8.str;
                printf("  [ConstantValue: \"%s\"]\n", s ? s : "<invalid utf8>");
            } else {
                printf("  [ConstantValue: <invalid string index #%u>]\n", utf8_index);
            }
            break;
        }
        default:
            // pela especificação, ConstantValue deve apontar só para {Integer, Float, Long, Double, String}
            printf("  [ConstantValue: <unsupported cp tag %u at #%u>]\n", entry->tag, constant_index);
            break;
    }
}

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
            member_info field = cf->fields[i];

            u2 access_flags = field.access_flags;
            char *field_name = cp[field.name_index - 1].info.UTF8.str;
            char *field_desc = cp[field.descriptor_index - 1].info.UTF8.str;

            char *field_desc_str = parse_descriptor(field_desc, NULL);

            char *flags = parse_flags(access_flags, 9, ", ", flag_map);
            char *kws = parse_flags(access_flags, 7, " ", flag_kw_map);

            printf(" %s %s%s;\n    descriptor: %s\n    flags: (0x%04x) %s\n%c",
                   kws, field_desc_str, field_name, field_desc,
                   access_flags, flags, nl);

            if (field.attributes_count > 0 && field.attributes != NULL) {
                attribute *attrs = field.attributes;
                for (u2 a = 0; a < field.attributes_count; ++a) {
                    const attribute *ai = &attrs[a];
                    const char *attr_name = cp[ai->attribute_name_index - 1].info.UTF8.str;

                    if (!attr_name) {
                        printf("  [<unknown-attribute>: length=%u]\n", ai->attribute_length);
                        continue;
                    }

                    if (strcmp(attr_name, "ConstantValue") == 0) {
                        // já parseado no reader: ai->info.ConstantValue.constantvalue_index
                        print_constant_value(cf, ai->info.ConstantValue.constantvalue_index);
                    } else {
                        // por enquanto, só liste nome/length dos demais atributos de field
                        printf("  [%s: length=%u]\n", attr_name, ai->attribute_length);
                    }
                }
            }

            free(flags);
            free(field_desc_str);
            free(kws);
        }
}