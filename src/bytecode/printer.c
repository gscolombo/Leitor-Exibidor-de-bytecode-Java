#include "bytecode/printer.h"

void show_opcodes(const u1 *code, u4 length, const cp_info *cp)
{
    u4 i = 0;
    while (i < length)
    {
        // Ignore reserved opcodes
        if (code[i] > 0xC9)
        {
            i++;
            continue;
        }

        const OpcodeInfo info = opcode_table[code[i]];
        printf("      %4u: %s", i, info.name);

        // operandos imediatos ou índices na constant pool
        switch (code[i])
        {
        case 0x10: // bipush, 1 byte imediato
            if (i + 1 < length)
                printf(" %d", (u1)code[i + 1]);
            i += 2;
            break;

        case 0x11: // sipush, 2 bytes imediato
            if (i + 2 < length)
            {
                u2 val = (code[i + 1] << 8) | code[i + 2];
                printf(" %d", val);
            }
            i += 3;
            break;

        case 0x12: // ldc, 1 byte índice
            if (i + 1 < length)
            {
                char *constant_value = get_constant_UTF8_value(code[i + 1], cp);
                printf(" #%u <%s>", code[i + 1], constant_value);
                free(constant_value);
            }
            i += 2;
            break;

        case 0x13: // ldc_w, 2 bytes índice
        case 0x14: // ldc2_w, 2 bytes índice
        case 0xB2: // getstatic
        case 0xB3: // putstatic
        case 0xB4: // getfield
        case 0xB5: // putfield
        case 0xB6: // invokevirtual
        case 0xB7: // invokespecial
        case 0xB8: // invokestatic
        case 0xBB: // new
        case 0xBD: // anewarray
            if (i + 2 < length)
            {
                u2 idx = (code[i + 1] << 8) | code[i + 2];
                char *constant_value = get_constant_UTF8_value(idx, cp);
                printf(" #%u <%s>", idx, constant_value);
                free(constant_value);
            }
            i += 3;
            break;

        case 0xC5: // multianewarray
            if (i + 3 < length)
            {
                u2 idx = (code[i + 1] << 8) | code[i + 2];
                char *constant_value = get_constant_UTF8_value(idx, cp);
                u1 dim = code[i + 3];
                printf(" #%u <%s> dim %u", idx, constant_value, dim);
                free(constant_value);
            }
            i += 4;
            break;

        case 0x84: // iinc, 1 byte imediato não-negativo, 1 byte imediato
            if (i + 2 < length)
            {
                printf(" %u by %i", (u1)code[i + 1], (uint8_t)code[i + 2]);
            }
            i += 3;
            break;

        case 0x99: // ifeq
        case 0x9A: // ifne
        case 0x9B: // iflt
        case 0x9C: // ifle
        case 0x9D: // ifgt
        case 0x9E: // ifge
        case 0x9F: // if_icmpeq
        case 0xA0: // if_icmpne
        case 0xA1: // if_icmplt
        case 0xA2: // if_icmpge
        case 0xA3: // if_icmpgt
        case 0xA4: // if_icmple
        case 0xA5: // if_acmpeq
        case 0xA6: // if_acmpne
        case 0xA7: // goto
        case 0xC6: // ifnull
        case 0xC7: // ifnonnull
            if (i + 2 < length)
            {
                int16_t offset = (code[i + 1] << 8) | code[i + 2];
                printf(" %u (%c%i)", offset + i, offset > 0 ? '+' : '\0', offset); // branch address
                i += 3;
            }
            break;

        case 0xBC: // newarray, 1 byte imediato
            if (i + 1 < length)
            {
                printf(" %u", code[i + 1]);
                switch (code[i + 1])
                {
                case 4:
                    printf(" (boolean)");
                    break;
                case 5:
                    printf(" (char)");
                    break;
                case 6:
                    printf(" (float)");
                    break;
                case 7:
                    printf(" (double)");
                    break;
                case 8:
                    printf(" (byte)");
                    break;
                case 9:
                    printf(" (short)");
                    break;
                case 10:
                    printf(" (int)");
                    break;
                case 11:
                    printf(" (long)");
                    break;
                default:
                    printf(" unknown type");
                    break;
                }
            }
            i += 2;
            break;

        default:
            i += 1;
            break;
        }

        printf("\n");
    }
}
