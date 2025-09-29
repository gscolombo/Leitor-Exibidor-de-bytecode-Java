#include "reader.h"

u1 read_u1(FILE *fptr)
{
    u1 u;
    fread(&u, sizeof(u1), 1, fptr);

    return u;
}

u2 read_u2(FILE *fptr)
{
    u2 u;
    fread(&u, sizeof(u2), 1, fptr);

    if (LE)
    {
        u = u2swap(u);
    }

    return u;
}

u4 read_u4(FILE *fptr)
{
    u4 u;
    fread(&u, sizeof(u4), 1, fptr);

    if (LE)
    {
        u = u4swap(u);
    }

    return u;
}

FILE *open_classfile(const char *path)
{
    size_t l;

    if ((l = strlen(path)) > 6)
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

ClassFile read_classfile(FILE *fptr)
{
    ClassFile cf;

    if (fseek(fptr, 0, SEEK_END) == 0)
    {
        const size_t fsize = ftell(fptr);

        printf("Size: %lu bytes.\n\n", fsize);
        fseek(fptr, 0, SEEK_SET);

        cf.magic = read_u4(fptr);
        cf.minor_version = read_u2(fptr);
        cf.major_version = read_u2(fptr);
        cf.constant_pool_count = read_u2(fptr);
        cf.constant_pool = parse_constant_pool(fptr, cf.constant_pool_count);
        cf.access_flags = read_u2(fptr);
        cf.this_class = read_u2(fptr);
        cf.super_class = read_u2(fptr);
        cf.interfaces_count = read_u2(fptr);

        if (cf.interfaces_count > 0)
        {
            u2 *interfaces = (u2 *)calloc(cf.interfaces_count, sizeof(u2));
            for (size_t i = 0; i < cf.interfaces_count; i++)
            {
                interfaces[i] = read_u2(fptr);
            }
            cf.interfaces = interfaces;
        }

        cf.fields_count = read_u2(fptr);

        if (cf.fields_count > 0)
        {
            field_info *fields = (field_info *)calloc(cf.fields_count, sizeof(field_info));
            for (size_t i = 0; i < cf.fields_count; i++)
            {
                fields[i].access_flags = read_u2(fptr);
                fields[i].name_index = read_u2(fptr);
                fields[i].descriptor_index = read_u2(fptr);
                fields[i].attributes_count = read_u2(fptr);

                if (fields[i].attributes_count > 0)
                {
                    attribute *attrs = (attribute *)calloc(fields[i].attributes_count, sizeof(attribute));
                    read_attributes(cf.constant_pool, fields[i].attributes_count, fptr, attrs);
                }
            }
            cf.fields = fields;
        }

        fclose(fptr);
    }

    return cf;
}

void read_attributes(cp_info *cp, unsigned int n, FILE *fptr, attribute *attr)
{
    if (attr != NULL)
        for (size_t i = 0; i < n; i++)
        {
            attr[i].attribute_name_index = read_u2(fptr);
            attr[i].attribute_length = read_u4(fptr);

            const wchar_t *attr_name = cp[attr[i].attribute_name_index].info.UTF8.str;
            const attribute_name attr_type = *convert_attr_name(attr_name);

            switch (attr_type)
            {
            case ConstantValue:
                attr[i].info.ConstantValue.constantvalue_index = read_u2(fptr);
                break;
            case Code:
                attr[i].info.Code.max_stack = read_u2(fptr);
                attr[i].info.Code.max_locals = read_u2(fptr);
                attr[i].info.Code.code_length = read_u4(fptr);
                fread(attr[i].info.Code.code, sizeof(u1), attr[i].info.Code.code_length, fptr);
                attr[i].info.Code.exception_table_length = read_u2(fptr);

                for (size_t j = 0; j < attr[i].info.Code.exception_table_length; j++)
                {
                    attr[i].info.Code.exception_table[j].start_pc = read_u2(fptr);
                    attr[i].info.Code.exception_table[j].end_pc = read_u2(fptr);
                    attr[i].info.Code.exception_table[j].handler_pc = read_u2(fptr);
                    attr[i].info.Code.exception_table[j].catch_type = read_u2(fptr);
                }

                attr[i].info.Code.attributes_count = read_u2(fptr);
                read_attributes(cp, attr[i].info.Code.attributes_count, fptr, attr[i].info.Code.attributes);
                break;
            case StackMapTable:
            default:
                break;
            }
        }
}
