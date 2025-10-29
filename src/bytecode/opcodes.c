#include "bytecode/opcodes.h"

static const OpcodeInfo opcode_table[] = {
    {0x00, "nop", 1},
    {0x01, "aconst_null", 1},
    {0x02, "iconst_m1", 1},
    {0x03, "iconst_0", 1},
    {0x04, "iconst_1", 1},
    {0x05, "iconst_2", 1},
    {0x06, "iconst_3", 1},
    {0x07, "iconst_4", 1},
    {0x08, "iconst_5", 1},
    {0x09, "lconst_0", 2},
    {0x0A, "lconst_1", 2},
    {0x0B, "fconst_0", 1},
    {0x0C, "fconst_1", 1},
    {0x0D, "fconst_2", 1},
    {0x0E, "dconst_0", 2},
    {0x0F, "dconst_1", 2},

    {0x10, "bipush", 1},
    {0x11, "sipush", 2},
    {0x12, "ldc", 1},
    {0x13, "ldc_w", 2},
    {0x14, "ldc2_w", 2},

    {0x15, "iload", 1},
    {0x16, "lload", 2},
    {0x17, "fload", 1},
    {0x18, "dload", 2},
    {0x19, "aload", 1},
    {0x1A, "iload_0", 1},
    {0x1B, "iload_1", 1},
    {0x1C, "iload_2", 1},
    {0x1D, "iload_3", 1},
    {0x1E, "lload_0", 2},
    {0x1F, "lload_1", 2},
    {0x20, "lload_2", 2},
    {0x21, "lload_3", 2},
    {0x22, "fload_0", 1},
    {0x23, "fload_1", 1},
    {0x24, "fload_2", 1},
    {0x25, "fload_3", 1},
    {0x26, "dload_0", 2},
    {0x27, "dload_1", 2},
    {0x28, "dload_2", 2},
    {0x29, "dload_3", 2},
    {0x2A, "aload_0", 1},
    {0x2B, "aload_1", 1},
    {0x2C, "aload_2", 1},
    {0x2D, "aload_3", 1},

    {0x30, "faload", 1},
    {0x31, "daload", 1},
    {0x32, "aaload", 1},
    {0x33, "baload", 1},
    {0x34, "caload", 1},
    {0x35, "saload", 1},

    {0x36, "istore", 1},
    {0x37, "lstore", 2},
    {0x38, "fstore", 1},
    {0x39, "dstore", 2},
    {0x3A, "astore", 1},
    {0x3B, "istore_0", 1},
    {0x3C, "istore_1", 1},
    {0x3D, "istore_2", 1},
    {0x3E, "istore_3", 1},
    {0x3F, "lstore_0", 2},
    {0x40, "lstore_1", 2},
    {0x41, "lstore_2", 2},
    {0x42, "lstore_3", 2},
    {0x43, "fstore_0", 1},
    {0x44, "fstore_1", 1},
    {0x45, "fstore_2", 1},
    {0x46, "fstore_3", 1},
    {0x47, "dstore_0", 2},
    {0x48, "dstore_1", 2},
    {0x49, "dstore_2", 2},
    {0x4A, "dstore_3", 2},
    {0x4B, "astore_0", 1},
    {0x4C, "astore_1", 1},
    {0x4D, "astore_2", 1},
    {0x4E, "astore_3", 1},

    {0x50, "lastore", 1},
    {0x51, "fastore", 1},
    {0x52, "dastore", 1},
    {0x53, "aastore", 1},
    {0x54, "bastore", 1},
    {0x57, "pop", 1},
    {0x58, "pop2", 2},
    {0x59, "dup", 1},
    {0x5A, "dup_x1", 1},
    {0x5B, "dup_x2", 1},
    {0x5C, "dup2", 2},
    {0x5D, "dup2_x1", 2},
    {0x5E, "dup2_x2", 2},
    {0x5F, "swap", 1},

    {0x60, "iadd", 1},
    {0x61, "ladd", 2},
    {0x62, "fadd", 1},
    {0x63, "dadd", 2},
    {0x64, "isub", 1},
    {0x65, "lsub", 2},
    {0x66, "fsub", 1},
    {0x67, "dsub", 2},
    {0x68, "imul", 1},
    {0x69, "lmul", 2},
    {0x6A, "fmul", 1},
    {0x6B, "dmul", 2},
    {0x6C, "idiv", 1},
    {0x6D, "ldiv", 2},
    {0x6E, "fdiv", 1},
    {0x6F, "ddiv", 2},
    {0x70, "irem", 1},
    {0x71, "lrem", 2},
    {0x72, "frem", 1},
    {0x73, "drem", 2},
    {0x74, "ineg", 1},
    {0x75, "lneg", 2},
    {0x76, "fneg", 1},
    {0x77, "dneg", 2},

    {0x84, "iinc", 2},

    {0x85, "i2l", 1},
    {0x86, "i2f", 1},
    {0x87, "i2d", 1},
    {0x88, "l2i", 1},
    {0x89, "l2f", 1},
    {0x8A, "l2d", 1},
    {0x8B, "f2i", 1},
    {0x8C, "f2l", 1},
    {0x8D, "f2d", 1},
    {0x8E, "d2i", 1},
    {0x8F, "d2l", 1},
    {0x90, "d2f", 1},
    {0x91, "i2b", 1},
    {0x92, "i2c", 1},
    {0x93, "i2s", 1},

    {0x94, "lcmp", 1},
    {0x95, "fcmpl", 1},
    {0x96, "fcmpg", 1},
    {0x97, "dcmpl", 1},
    {0x98, "dcmpg", 1},

    {0x99, "ifeq", 2},
    {0x9A, "ifne", 2},
    {0x9B, "iflt", 2},
    {0x9C, "ifge", 2},
    {0x9D, "ifgt", 2},
    {0x9E, "ifle", 2},
    {0x9F, "if_icmpeq", 2},
    {0xA0, "if_icmpne", 2},
    {0xA1, "if_icmplt", 2},
    {0xA2, "if_icmpge", 2},
    {0xA3, "if_icmpgt", 2},
    {0xA4, "if_icmple", 2},
    {0xA5, "if_acmpeq", 2},
    {0xA6, "if_acmpne", 2},
    {0xA7, "goto", 2},

    {0xAA, "tableswitch", 1},
    {0xAC, "ireturn", 1},
    {0xAD, "lreturn", 1},
    {0xAE, "freturn", 1},
    {0xAF, "dreturn", 1},
    {0xB0, "areturn", 1},
    {0xB1, "return", 1},

    {0xB2, "getstatic", 2},
    {0xB3, "putstatic", 2},
    {0xB4, "getfield", 2},
    {0xB5, "putfield", 2},
    {0xB6, "invokevirtual", 2},
    {0xB7, "invokespecial", 2},
    {0xB8, "invokestatic", 2},
    {0xBB, "new", 2},
    {0xBC, "newarray", 1},
    {0xBD, "anewarray", 2},
    {0xBF, "athrow", 1},

    {0xC6, "ifnull", 2},
    {0xC7, "ifnonnull", 2},
};

const OpcodeInfo *get_opcode_info(u1 opcode)
{
    for (size_t i = 0; i < sizeof(opcode_table) / sizeof(OpcodeInfo); i++)
    {
        if (opcode_table[i].opcode == opcode)
            return &opcode_table[i];
    }
    return NULL;
}

void show_opcodes(const u1 *code, u4 length)
{
    u4 i = 0;
    while (i < length)
    {
        const OpcodeInfo *info = get_opcode_info(code[i]);
        if (!info)
        {
            printf("      %04u: 0x%02X <unknown>\n", i, code[i]);
            i++;
            continue;
        }

        printf("      %04u: %s", i, info->name);

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
                printf(" #%u", code[i + 1]);
            i += 2;
            break;

        case 0x13: // ldc_w, 2 bytes índice
        case 0x14: // ldc2_w, 2 bytes índice
            if (i + 2 < length)
            {
                u2 idx = (code[i + 1] << 8) | code[i + 2];
                printf(" #%u", idx);
            }
            i += 3;
            break;

        case 0x84: // iinc, 1 byte imediato não-negativo, 1 byte imediato
            if (i + 2 < length)
            {
                printf(" %u, %i", (u1)code[i + 1], (uint8_t)code[i + 2]);
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
                printf(" %i", (int16_t)((code[i + 1] << 8) | code[i + 2]) + i); // branch address
            i += 3;
            break;

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
                printf(" #%u", idx);
            }
            i += 3;
            break;

        case 0xBC: // newarray, 1 byte imediato
            if (i + 1 < length)
            {
                switch (code[i + 1])
                {
                case 4:
                    printf(" boolean");
                    break;
                case 5:
                    printf(" char");
                    break;
                case 6:
                    printf(" float");
                    break;
                case 7:
                    printf(" byte");
                    break;
                case 8:
                    printf(" short");
                    break;
                case 9:
                    printf(" int");
                    break;
                case 10:
                    printf(" long");
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
