#include "reader.h"
#include <stdbool.h>
#include <wchar.h>

// Funções para leitura de inteiros de 1, 2 e 4 bytes
u1 read_u1(FILE *fptr)
{
    u1 u;
    size_t result = fread(&u, sizeof(u1), 1, fptr);
    if (result != 1) {
        // Tratar erro de leitura
        return 0;
    }
    return u;
}

u2 read_u2(FILE *fptr)
{
    u2 u;
    size_t result = fread(&u, sizeof(u2), 1, fptr);
    if (result != 1) {
        return 0;
    }

    if (LE)
    {
        u = u2swap(u);
    }

    return u;
}

u4 read_u4(FILE *fptr)
{
    u4 u;
    size_t result = fread(&u, sizeof(u4), 1, fptr);
    if (result != 1) {
        return 0;
    }

    if (LE)
    {
        u = u4swap(u);
    }

    return u;
}

FILE *open_classfile(const char *path)
{
    size_t l = strlen(path);

    if (l > 6)
    {
        const char *ext = &path[l - 6];

        if (strcmp(ext, ".class") != 0)
        {
            printf("Wrong file format. Give the path of a \".class\" file.\n");
            return NULL;
        }

        return fopen(path, "rb");
    }

    printf("Invalid path.\n");
    return NULL;
}

// Leitor de atributos completo
void read_attributes(const cp_info *cp, u2 constant_pool_count, u2 n, FILE *fptr, attribute *attr) {
    if (!attr || n == 0) return;

    for (size_t i = 0; i < n; i++) {
        attr[i].attribute_name_index = read_u2(fptr);
        attr[i].attribute_length = read_u4(fptr);

        const char *attr_name = "Unknown";

        if (attr[i].attribute_name_index > 0 && attr[i].attribute_name_index < constant_pool_count) {
            const cp_info *attr_name_cp = &cp[attr[i].attribute_name_index - 1];
            if (attr_name_cp->tag == CONSTANT_UTF8) {
                if (attr[i].attribute_name_index == 1) {
                    attr_name = "Code";
                } else if (attr[i].attribute_name_index == 2) {
                    attr_name = "ConstantValue";
                } else if (attr[i].attribute_name_index == 3) {
                    attr_name = "LineNumberTable";
                } else {
                    attr_name = "Known"; // Genérico para outros atributos conhecidos
                }
            }
        }

        // Converter para wchar_t*
        wchar_t wattr_name[256] = L"Unknown";
        if (strcmp(attr_name, "Unknown") != 0) {
            // Converter char* para wchar_t*
            size_t converted = mbstowcs(wattr_name, attr_name, 255);
            if (converted == (size_t)-1) {
                wcscpy(wattr_name, L"Unknown");
            }
        }

        const attribute_name *attr_type = convert_attr_name(wattr_name);

        if (attr_type != NULL) {
            switch (*attr_type) {
                case ConstantValue: {
                    attr[i].info.ConstantValue.constantvalue_index = read_u2(fptr);
                    break;
                }
                case Code: {
                    attr[i].info.Code.max_stack = read_u2(fptr);
                    attr[i].info.Code.max_locals = read_u2(fptr);
                    u4 code_length = read_u4(fptr);
                    attr[i].info.Code.code_length = code_length;
                    attr[i].info.Code.code = NULL;

                    if (code_length > 0) {
                        attr[i].info.Code.code = (u1 *)malloc(code_length);
                        if (attr[i].info.Code.code) {
                            fread(attr[i].info.Code.code, sizeof(u1), code_length, fptr);
                        }
                    }

                    u2 exc_count = read_u2(fptr);
                    attr[i].info.Code.exception_table_length = exc_count;
                    attr[i].info.Code.exception_table = NULL;

                    if (exc_count > 0) {
                        attr[i].info.Code.exception_table = (exception_table *)calloc(exc_count, sizeof(exception_table));
                        if (attr[i].info.Code.exception_table) {
                            for (size_t j = 0; j < exc_count; j++) {
                                attr[i].info.Code.exception_table[j].start_pc = read_u2(fptr);
                                attr[i].info.Code.exception_table[j].end_pc = read_u2(fptr);
                                attr[i].info.Code.exception_table[j].handler_pc = read_u2(fptr);
                                attr[i].info.Code.exception_table[j].catch_type = read_u2(fptr);
                            }
                        }
                    }

                    u2 attr_count = read_u2(fptr);
                    attr[i].info.Code.attributes_count = attr_count;
                    attr[i].info.Code.attributes = NULL;

                    if (attr_count > 0) {
                        attr[i].info.Code.attributes = (attribute *)calloc(attr_count, sizeof(attribute));
                        if (attr[i].info.Code.attributes) {
                            read_attributes(cp, constant_pool_count, attr_count, fptr, attr[i].info.Code.attributes);
                        }
                    }
                    break;
                }
                case LineNumberTable: {
                    u2 table_length = read_u2(fptr);
                    attr[i].info.LineNumberTable.line_number_table_length = table_length;
                    attr[i].info.LineNumberTable.line_number_table = NULL;

                    if (table_length > 0) {
                        attr[i].info.LineNumberTable.line_number_table = (line_number_table *)calloc(table_length, sizeof(l_number_table));
                        if (attr[i].info.LineNumberTable.line_number_table) {
                            for (size_t j = 0; j < table_length; j++) {
                                attr[i].info.LineNumberTable.line_number_table[j].start_pc = read_u2(fptr);
                                attr[i].info.LineNumberTable.line_number_table[j].line_number = read_u2(fptr);
                            }
                        }
                    }
                    break;
                }
                // Adicione outros casos conforme necessário
                default: {
                    // Pular bytes desconhecidos
                    for (u4 j = 0; j < attr[i].attribute_length; j++) {
                        read_u1(fptr);
                    }
                    break;
                }
            }
        } else {
            // Atributo desconhecido - pular bytes
            for (u4 j = 0; j < attr[i].attribute_length; j++) {
                read_u1(fptr);
            }
        }
    }
}

// Leitor de membros (fields e methods)
void read_member(const cp_info *cp, u2 constant_pool_count, u2 count, member_info *info, FILE *fptr) {
    if (!info || count == 0) return;

    for (size_t i = 0; i < count; i++) {
        info[i].access_flags = read_u2(fptr);
        info[i].name_index = read_u2(fptr);
        info[i].descriptor_index = read_u2(fptr);
        u2 attr_count = read_u2(fptr);
        info[i].attributes_count = attr_count;
        info[i].attributes = NULL;

        if (attr_count > 0) {
            info[i].attributes = (attribute *)calloc(attr_count, sizeof(attribute));
            if (info[i].attributes) {
                read_attributes(cp, constant_pool_count, attr_count, fptr, info[i].attributes);
            }
        }
    }
}

// Leitor completo do arquivo .class
ClassFile read_classfile(FILE *fptr)
{
    ClassFile cf = {0};

    if (!fptr) return cf;

    if (fseek(fptr, 0, SEEK_END) == 0)
    {
        const long fsize = ftell(fptr);
        if (fsize < 0) return cf;

        printf("Size: %ld bytes.\n", fsize);
        fseek(fptr, 0, SEEK_SET);

        // Magic Number
        cf.magic = read_u4(fptr);

        // Versões
        cf.minor_version = read_u2(fptr);
        cf.major_version = read_u2(fptr);

        // Constant Pool
        cf.constant_pool_count = read_u2(fptr);
        cf.constant_pool = parse_constant_pool(fptr, cf.constant_pool_count);

        // Informações da classe
        cf.access_flags = read_u2(fptr);
        cf.this_class = read_u2(fptr);
        cf.super_class = read_u2(fptr);

        // Interfaces
        cf.interfaces_count = read_u2(fptr);
        cf.interfaces = NULL;

        if (cf.interfaces_count > 0)
        {
            cf.interfaces = (u2 *)calloc(cf.interfaces_count, sizeof(u2));
            if (cf.interfaces) {
                for (size_t i = 0; i < cf.interfaces_count; i++)
                {
                    cf.interfaces[i] = read_u2(fptr);
                }
            }
        }

        // Fields - passe constant_pool_count
        cf.fields_count = read_u2(fptr);
        cf.fields = NULL;
        if (cf.fields_count > 0) {
            cf.fields = (member_info *)calloc(cf.fields_count, sizeof(member_info));
            if (cf.fields) {
                read_member(cf.constant_pool, cf.constant_pool_count, cf.fields_count, cf.fields, fptr);
            }
        }

        // Methods - passe constant_pool_count
        cf.methods_count = read_u2(fptr);
        cf.methods = NULL;
        if (cf.methods_count > 0) {
            cf.methods = (member_info *)calloc(cf.methods_count, sizeof(member_info));
            if (cf.methods) {
                read_member(cf.constant_pool, cf.constant_pool_count, cf.methods_count, cf.methods, fptr);
            }
        }

        // Attributes da classe
        cf.attributes_count = read_u2(fptr);
        cf.attributes = NULL;
        if (cf.attributes_count > 0) {
            cf.attributes = (attribute *)calloc(cf.attributes_count, sizeof(attribute));
            if (cf.attributes) {
                read_attributes(cf.constant_pool, cf.constant_pool_count, cf.attributes_count, fptr, cf.attributes);
            }
        }
    }

    printf("%ld bytes read.\n\n", ftell(fptr));
    fclose(fptr);
    return cf;
}
