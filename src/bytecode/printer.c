/* --- Início: helpers e show_opcodes corrigidos --- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "bytecode/printer.h"
#include "types/cp/constants.h"

/* Resolve uma entrada da constant pool e retorna string alocada (caller free()). */
static char *resolve_cp_entry(const cp_info *cp, u2 cp_count, u2 index)
{
    if (index < 1 || index > cp_count) {
        char *s = malloc(32);
        snprintf(s, 32, "<invalid cp index #%u>", index);
        return s;
    }

    const cp_info *entry = &cp[index - 1];
    char *s = NULL;

    switch (entry->tag) {
    case CONSTANT_Class: {
        u2 name_index = entry->info.Class.name_index;
        if (name_index >= 1 && name_index <= cp_count &&
            cp[name_index - 1].tag == CONSTANT_UTF8) {
            const char *name = cp[name_index - 1].info.UTF8.str;
            s = malloc(strlen(name) + 16);
            sprintf(s, "Class %s", name);
        } else {
            s = strdup("<invalid Class>");
        }
        break;
    }

    case CONSTANT_Fieldref:
    case CONSTANT_Methodref:
    case CONSTANT_InterfaceMethodref: {
        /* No teu header, campo é 'Ref' que contém class_index e name_and_type_index */
        u2 class_index = entry->info.Ref.class_index;
        u2 name_type_index = entry->info.Ref.name_and_type_index;

        const char *class_name = "<invalid>";
        const char *nt_name = "<invalid>";
        const char *nt_desc = "<invalid>";

        if (class_index >= 1 && class_index <= cp_count &&
            cp[class_index - 1].tag == CONSTANT_Class) {
            u2 ci = cp[class_index - 1].info.Class.name_index;
            if (ci >= 1 && ci <= cp_count && cp[ci - 1].tag == CONSTANT_UTF8)
                class_name = cp[ci - 1].info.UTF8.str;
        }

        if (name_type_index >= 1 && name_type_index <= cp_count &&
            cp[name_type_index - 1].tag == CONSTANT_NameAndType) {
            u2 nidx = cp[name_type_index - 1].info.NameAndType.name_index;
            u2 tidx = cp[name_type_index - 1].info.NameAndType.descriptor_index;
            if (nidx >= 1 && nidx <= cp_count && cp[nidx - 1].tag == CONSTANT_UTF8)
                nt_name = cp[nidx - 1].info.UTF8.str;
            if (tidx >= 1 && tidx <= cp_count && cp[tidx - 1].tag == CONSTANT_UTF8)
                nt_desc = cp[tidx - 1].info.UTF8.str;
        }

        s = malloc(strlen(class_name) + strlen(nt_name) + strlen(nt_desc) + 32);
        if (entry->tag == CONSTANT_Fieldref)
            sprintf(s, "Fieldref %s.%s:%s", class_name, nt_name, nt_desc);
        else if (entry->tag == CONSTANT_Methodref)
            sprintf(s, "Methodref %s.%s:%s", class_name, nt_name, nt_desc);
        else
            sprintf(s, "InterfaceMethodref %s.%s:%s", class_name, nt_name, nt_desc);

        break;
    }

    case CONSTANT_NameAndType: {
        const char *n = "<invalid>", *d = "<invalid>";
        u2 nidx = entry->info.NameAndType.name_index;
        u2 didx = entry->info.NameAndType.descriptor_index;
        if (nidx >= 1 && nidx <= cp_count && cp[nidx - 1].tag == CONSTANT_UTF8)
            n = cp[nidx - 1].info.UTF8.str;
        if (didx >= 1 && didx <= cp_count && cp[didx - 1].tag == CONSTANT_UTF8)
            d = cp[didx - 1].info.UTF8.str;
        s = malloc(strlen(n) + strlen(d) + 16);
        sprintf(s, "NameAndType %s:%s", n, d);
        break;
    }

    case CONSTANT_UTF8: {
        const char *u = entry->info.UTF8.str;
        s = malloc(strlen(u) + 8);
        sprintf(s, "UTF8 %s", u);
        break;
    }

    case CONSTANT_String: {
        u2 str_idx = entry->info.String.string_index;
        const char *val = "<invalid>";
        if (str_idx >= 1 && str_idx <= cp_count && cp[str_idx - 1].tag == CONSTANT_UTF8)
            val = cp[str_idx - 1].info.UTF8.str;
        s = malloc(strlen(val) + 16);
        sprintf(s, "String %s", val);
        break;
    }

    case CONSTANT_Integer: {
        int i = entry->info._4Bn.number.i;
        char tmp[64];
        snprintf(tmp, sizeof(tmp), "Integer %d", i);
        s = strdup(tmp);
        break;
    }

    case CONSTANT_Float: {
        float f = entry->info._4Bn.number.f;
        char tmp[64];
        snprintf(tmp, sizeof(tmp), "Float %g", f);
        s = strdup(tmp);
        break;
    }

    case CONSTANT_Long: {
        long l = entry->info._8Bn.number.l;
        char tmp[64];
        snprintf(tmp, sizeof(tmp), "Long %ld", l);
        s = strdup(tmp);
        break;
    }

    case CONSTANT_Double: {
        double d = entry->info._8Bn.number.d;
        char tmp[64];
        snprintf(tmp, sizeof(tmp), "Double %g", d);
        s = strdup(tmp);
        break;
    }

    default:
        s = malloc(32);
        snprintf(s, 32, "<cp tag %u>", entry->tag);
        break;
    }

    return s;
}

/* map newarray numeric code to human name */
static const char *newarray_type_name(u1 code)
{
    switch (code) {
    case 4: return "boolean";
    case 5: return "char";
    case 6: return "float";
    case 7: return "double";
    case 8: return "byte";
    case 9: return "short";
    case 10: return "int";
    case 11: return "long";
    default: return "unknown";
    }
}

/* Helper to read signed 16-bit from two bytes */
static int16_t read_i16_from_bytes(const u1 *code, u4 hi_index, u4 lo_index) {
    return (int16_t)((code[hi_index] << 8) | code[lo_index]);
}

/* NOTE: ALTEREI A ASSINATURA para receber cp_count (mais seguro) */
void show_opcodes(const u1 *code, u4 length, const cp_info *cp, u2 cp_count)
{
    if (!code) return;

    u4 i = 0;
    while (i < length)
    {
        /* Preserve comportamento anterior: ignorar opcodes reservados */
        if (code[i] > 0xC9)
        {
            i++;
            continue;
        }

        const OpcodeInfo opcode = opcode_table[code[i]];
        printf("      %4u: %-15s", i, opcode.mnemonic);

        switch (code[i])
        {
        case 0x10: { // bipush (signed byte)
            if (i + 1 < length) {
                int8_t val = (int8_t)code[i + 1];
                printf("%d (0x%02X)", val, code[i+1]);
            }
            i += 2;
            break;
        }

        case 0x11: { // sipush (signed 2 bytes)
            if (i + 2 < length) {
                int16_t val = read_i16_from_bytes(code, i + 1, i + 2);
                printf("%d (0x%02X 0x%02X)", val, code[i+1], code[i+2]);
            }
            i += 3;
            break;
        }

        case 0x12: { // ldc (u1 index)
            if (i + 1 < length) {
                u1 idx = code[i + 1];
                char *resolved = resolve_cp_entry(cp, cp_count, idx);
                printf("#%u (%s) [bytes: 0x%02X]", idx, resolved, code[i+1]);
                free(resolved);
            }
            i += 2;
            break;
        }

        case 0x13: case 0x14: /* ldc_w / ldc2_w */
        case 0xB2: case 0xB3: case 0xB4: case 0xB5:
        case 0xB6: case 0xB7: case 0xB8: case 0xBB:
        case 0xBD:
        {
            if (i + 2 < length) {
                u2 idx = (u2)((code[i+1] << 8) | code[i+2]);
                char *resolved = resolve_cp_entry(cp, cp_count, idx);
                printf("#%u (%s) [bytes: 0x%02X 0x%02X]", idx, resolved, code[i+1], code[i+2]);
                free(resolved);
            }
            i += 3;
            break;
        }

        case 0xC5: { /* multianewarray */
            if (i + 3 < length) {
                u2 idx = (u2)((code[i+1] << 8) | code[i+2]);
                u1 dim = code[i+3];
                char *resolved = resolve_cp_entry(cp, cp_count, idx);
                printf("#%u (%s) dim %u [bytes: 0x%02X 0x%02X 0x%02X]", idx, resolved, dim, code[i+1], code[i+2], code[i+3]);
                free(resolved);
            }
            i += 4;
            break;
        }

        case 0xB9: { /* invokeinterface: indexbyte1,indexbyte2,count,0 */
            if (i + 4 < length) {
                u2 idx = (u2)((code[i+1] << 8) | code[i+2]);
                u1 count = code[i+3];
                u1 zero = code[i+4];
                char *resolved = resolve_cp_entry(cp, cp_count, idx);
                printf("#%u (%s) count=%u reserved=0x%02X [bytes: 0x%02X 0x%02X 0x%02X 0x%02X]",
                       idx, resolved, count, zero, code[i+1], code[i+2], code[i+3], code[i+4]);
                free(resolved);
            }
            i += 5;
            break;
        }

        case 0x84: { /* iinc: index (u1), const (signed byte) */
            if (i + 2 < length) {
                u1 idx = code[i+1];
                int8_t cons = (int8_t)code[i+2];
                printf("%u by %d [bytes: 0x%02X 0x%02X]", idx, cons, code[i+1], code[i+2]);
            }
            i += 3;
            break;
        }

        case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: case 0x9E:
        case 0x9F: case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4:
        case 0xA5: case 0xA6: case 0xA7: case 0xC6: case 0xC7:
        {
            if (i + 2 < length) {
                int16_t offset = read_i16_from_bytes(code, i + 1, i + 2);
                int target = (int)i + offset;
                printf("target=%d (offset=%+d) [bytes: 0x%02X 0x%02X]", target, offset, code[i+1], code[i+2]);
            }
            i += 3;
            break;
        }
        
        case 0xBC: { /* newarray */
            if (i + 1 < length) {
                u1 t = code[i+1];
                const char *name = newarray_type_name(t);
                printf("%u (%s) [byte: 0x%02X]", t, name, code[i+1]);
            }
            i += 2;
            break;
        }

        case 0xAA: { /* tableswitch */
            /* Reaplicar implementação já existente (preservar pad & leitura) */
            u4 start = i;
            while ((++i) % 4 != 0) continue;

            int32_t _default = get_switch_32B_values(i, code);
            i += 4;
            int32_t low = get_switch_32B_values(i, code);
            i += 4;
            int32_t high = get_switch_32B_values(i, code);
            i += 4;

            if (low <= high)
            {
                printf(" %i to %i\n", low, high);

                for (int32_t j = 0; j < high - low + 1; j++)
                {
                    u4 jump_offset = get_switch_32B_values(i, code);
                    i += 4;
                    int target = start + jump_offset;
                    printf("                 %4d: %d (offset=%+d)\n", j + low, target, jump_offset);
                }
                int default_target = start + _default;
                printf("                 default: %d (offset=%+d)", default_target, _default);
            }

            break;
        }

        case 0xAB: { /* lookupswitch */
            u4 start = i; /* posição do opcode */
            /* consumir padding: avançar para a próxima posição 4-byte alinhada após o opcode */
            while ((++i) % 4 != 0)
                printf(" [pad 0x%02X]", code[i]); /* opcional: mostrar bytes de padding */
            /* agora i está no começo dos 4-byte do default */
            int32_t _default = get_switch_32B_values(i, code);
            i += 4;
            int32_t npairs = get_switch_32B_values(i, code);
            i += 4;

            printf(" npairs=%d\n", npairs);

            for (int32_t p = 0; p < npairs; ++p) {
                int32_t match = get_switch_32B_values(i, code); i += 4;
                int32_t jump_offset = get_switch_32B_values(i, code); i += 4;
                int target = (int)start + jump_offset;
                printf("                 match %d -> target=%d (offset=%+d)\n", match, target, jump_offset);
            }

            int default_target = (int)start + _default;
            printf("                 default -> target=%d (offset=%+d)", default_target, _default);
            break;
        }



        default:
            /* sem operandos extras */
            i += 1;
            break;
        }

        printf("\n");
    }
}

/* --- Fim: helpers e show_opcodes corrigidos --- */
