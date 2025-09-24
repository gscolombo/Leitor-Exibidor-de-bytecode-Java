#include "writer.h"

void show_classfile(ClassFile* cf) {
    printf("Magic: %#X\n", cf->magic);
    printf("Version: %u.%u\n", cf->major_version, cf->minor_version);
    printf("Constant Pool (count = %u):\n", cf->constant_pool_count);

    cp_info *cp = cf->constant_pool;
    u2 count = cf->constant_pool_count;

    for (u2 i = 1; i < count; i++, cp++) {
        show_constant(i, count, cp);
    }
}

void show_constant(u2 i, u2 count, cp_info* cp) {
    // Align list indexes
    u1 ws = num_digits(count) - num_digits(i);
    char *pad = malloc(ws + 1);
    memset(pad, ' ', ws);
    pad[ws] = '\0';

    switch (cp->tag)
    {
    case CONSTANT_Class:
        printf("%s#%u = Class\t\t\t#%u\n", pad, i, cp->info.Class.name_index);
        break;
    case CONSTANT_Fieldref:
        printf("%s#%u = Fieldref\t\t\t#%u.#%u\n", pad, i, cp->info._Ref.class_index, cp->info._Ref.name_and_type_index);
        break;
    case CONSTANT_Methodref: 
        printf("%s#%u = Methodref\t\t\t#%u.#%u\n", pad, i, cp->info._Ref.class_index, cp->info._Ref.name_and_type_index);
        break;
    case CONSTANT_InterfaceMethodref:
        printf("%s#%u = InterfaceMethodref\t\t\t#%u.#%u\n", pad, i, cp->info._Ref.class_index, cp->info._Ref.name_and_type_index);
        break;
    case CONSTANT_String:
        printf("%s#%u = String\n", pad, i);
        break;
    case CONSTANT_Integer:
        printf("%s#%u = Integer\n", pad, i);
        break;
    case CONSTANT_Float:
        printf("%s#%u = Float\n", pad, i);
        break;
    case CONSTANT_Long:
        printf("%s#%u = Long\n", pad, i);
        break;
    case CONSTANT_Double:
        printf("%s#%u = Double\n", pad, i);
        break;
    case CONSTANT_NameAndType:
        printf("%s#%u = NameAndType\n", pad, i);
        break;
    case CONSTANT_UTF8:
        wchar_t* s = decode_modified_utf8_str(cp->info.UTF8.length, cp->info.UTF8.bytes);
        printf("%s#%u = UTF-8\t\t\t%ls\n", pad, i, s);
        break;
    case CONSTANT_MethodHandle:
        printf("%s#%u = MethodHandle\n", pad, i);
        break;
    case CONSTANT_MethodType:
        printf("%s#%u = MethodType\n", pad, i);
        break;
    case CONSTANT_InvokeDynamic:
        printf("%s#%u = InvokeDynamic\n", pad, i);
        break;
    default:
        break;
    }

    free(pad);
}

wchar_t* decode_modified_utf8_str(u2 length, const u1* bytes) {
    if (bytes == NULL) {
        return NULL;
    }

    size_t buffer_size = 0, pos = 0;

    // Define buffer size
    while (pos < length) {
        u1 x = bytes[pos];

        if (x < 0x80) {
            buffer_size++;
            pos++;

        // 2 byte code point
        } else if ((x & 0xE0) == 0xC0) { // Check if high byte starts with 110 
            if (pos + 1 >= length) return NULL; // Early return if truncated sequence
            buffer_size++;
            pos += 2;

        // 3 byte code point
        } else if ((x & 0xF0) == 0xE0) { // Check if high byte starts with 1110
            if (pos + 2 >= length) return NULL; // Same as above
            buffer_size++;
            pos += 3;
        } else {
            return NULL;
        }
    }    

    wchar_t* str = (wchar_t*) malloc((buffer_size + 1) * sizeof(wchar_t));
    if (str == NULL) return NULL;

    size_t i = pos = 0;

    // Decode bytes
    while (pos < length) {
        u1 x = bytes[pos];

        if (x < 0x80) {
            str[i++] = (wchar_t) x;
            pos++;
        } else if ((x & 0xE0) == 0xC0) {
            u1 y = bytes[pos + 1];
            u2 code_point = ((x & 0x1F) << 6) | (y & 0x3F);
            str[i++] = code_point == 0 ? L'0' : (wchar_t) code_point; // The null character is represent by two bytes (0xC0,0x80);
            pos += 2;
        } else if ((x & 0xF0) == 0xE0) {
            u1 y = bytes[pos + 1];
            u1 z = bytes[pos + 2];
            u2 code_point = ((x & 0xF) << 12) | ((y & 0x3F) << 6) | (z & 0x3F);
            str[i++] = (wchar_t) code_point;
            pos += 3;
        }
    }

    str[i] = L'\0';
    return str;
}