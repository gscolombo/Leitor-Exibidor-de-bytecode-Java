#include "parser.h"
#include "reader.h"


cp_info* parse_constant_pool(FILE* fptr, u2 count) {
    cp_info *constant_pool = (cp_info *) malloc(sizeof (cp_info) * count);
    cp_info *cp = constant_pool;

    while (cp < constant_pool + count - 1) {
        cp->tag = read_u1(fptr);
        switch (cp->tag)
        {
        case CONSTANT_Class:
            cp->info.Class.name_index = read_u2(fptr);
            break;
        case CONSTANT_Fieldref:
        case CONSTANT_Methodref:
        case CONSTANT_InterfaceMethodref:
            cp->info._Ref.class_index = read_u2(fptr);
            cp->info._Ref.name_and_type_index = read_u2(fptr);
            break;
        case CONSTANT_String:
            cp->info.String.string_index = read_u2(fptr);
            break;
        case CONSTANT_Integer:
        case CONSTANT_Float:
            cp->info._4Bn.bytes = read_u4(fptr);
            break;
        case CONSTANT_Long:
        case CONSTANT_Double:
            cp->info._8Bn.high_bytes = read_u4(fptr);
            cp->info._8Bn.low_bytes = read_u4(fptr);
            break;
        case CONSTANT_NameAndType:
            cp->info.NameAndType.name_index = read_u2(fptr);
            cp->info.NameAndType.descriptor_index = read_u2(fptr);
            break;
        case CONSTANT_UTF8:
            u2 l = read_u2(fptr);
            cp->info.UTF8.length = l;

            u1 *bytes = (u1 *) malloc(sizeof (u1) * l);
            u1 *b;

            for (b = bytes; b < bytes + l; b++) {
                *b = read_u1(fptr);
            }

            cp->info.UTF8.bytes = b - l;
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
            break;
        }

        ++cp;
    }

    return cp - (count - 1);
}