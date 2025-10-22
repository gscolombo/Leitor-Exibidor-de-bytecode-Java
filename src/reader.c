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

        printf("Size: %lu bytes.\n", fsize);
        fseek(fptr, 0, SEEK_SET);

        cf.magic = read_u4(fptr);
        cf.minor_version = read_u2(fptr);
        cf.major_version = read_u2(fptr);
        cf.constant_pool_count = read_u2(fptr);
        cf.constant_pool = parse_constant_pool(fptr, cf.constant_pool_count);
        cf.access_flags = read_u2(fptr);
        cf.this_class = read_u2(fptr);
        cf.super_class = read_u2(fptr);

        // Interfaces
        cf.interfaces_count = read_u2(fptr);
        cf.interfaces = NULL;

        if (cf.interfaces_count > 0)
        {
            cf.interfaces = (u2 *)calloc(cf.interfaces_count, sizeof(u2));
            for (size_t i = 0; i < cf.interfaces_count; i++)
            {
                cf.interfaces[i] = read_u2(fptr);
            }
        }

        // Fields
        cf.fields_count = read_u2(fptr);
        cf.fields = (member_info *)calloc(cf.fields_count, sizeof(member_info));
        if (cf.fields != NULL)
            read_member(cf.constant_pool, cf.fields_count, cf.fields, fptr);

        // Methods
        cf.methods_count = read_u2(fptr);
        cf.methods = (member_info *)calloc(cf.methods_count, sizeof(member_info));
        if (cf.methods != NULL)
            read_member(cf.constant_pool, cf.methods_count, cf.methods, fptr);

        cf.attributes_count = read_u2(fptr);
    }

    printf("%lu bytes readed.\n\n", ftell(fptr));
    fclose(fptr);
    return cf;
}

void read_member(const cp_info *cp, u2 count, member_info *info, FILE *fptr)
{
    u2 attr_count;

    for (size_t i = 0; i < count; i++)
    {
        info[i].access_flags = read_u2(fptr);
        info[i].name_index = read_u2(fptr);
        info[i].descriptor_index = read_u2(fptr);
        info[i].attributes_count = attr_count = read_u2(fptr);

        if (attr_count > 0)
        {
            info[i].attributes = (attribute *)calloc(attr_count, sizeof(attribute));
            if (info[i].attributes != NULL)
                read_attributes(cp, attr_count, fptr, info[i].attributes);
        }
    }
}

void read_attributes(const cp_info *cp, u2 n, FILE *fptr, attribute *attr)
{
    u2 c;

    if (attr != NULL)
        for (size_t i = 0; i < n; i++)
        {
            attr[i].attribute_name_index = read_u2(fptr);
            attr[i].attribute_length = read_u4(fptr);

            const char *attr_name = cp[attr[i].attribute_name_index - 1].info.UTF8.str;

            const attribute_name *attr_type = convert_attr_name(attr_name);

            if (attr_type != NULL)
                switch (*attr_type)
                {
                case ConstantValue:
                    attr[i].info.ConstantValue.constantvalue_index = read_u2(fptr);
                    break;
                case Code:
                    attr[i].info.Code.max_stack = read_u2(fptr);
                    attr[i].info.Code.max_locals = read_u2(fptr);
                    c = attr[i].info.Code.code_length = read_u4(fptr);
                    attr[i].info.Code.code = NULL;

                    if (c > 0)
                    {
                        attr[i].info.Code.code = (u1 *)malloc(c);
                        if (attr[i].info.Code.code != NULL)
                            fread(attr[i].info.Code.code, sizeof(u1), c, fptr);
                    }

                    c = attr[i].info.Code.exception_table_length = read_u2(fptr);
                    attr[i].info.Code.exception_table = NULL;

                    if (c > 0)
                    {
                        attr[i].info.Code.exception_table = (struct exception_table *)calloc(c, sizeof(struct exception_table));
                        if (attr[i].info.Code.exception_table != NULL)
                            for (size_t j = 0; j < c; j++)
                            {
                                attr[i].info.Code.exception_table[j].start_pc = read_u2(fptr);
                                attr[i].info.Code.exception_table[j].end_pc = read_u2(fptr);
                                attr[i].info.Code.exception_table[j].handler_pc = read_u2(fptr);
                                attr[i].info.Code.exception_table[j].catch_type = read_u2(fptr);
                            }
                    }

                    c = attr[i].info.Code.attributes_count = read_u2(fptr);
                    attr[i].info.Code.attributes = NULL;

                    if (c > 0)
                    {
                        attr[i].info.Code.attributes = (attribute *)calloc(c, sizeof(attribute));
                        if (attr[i].info.Code.attributes != NULL)
                            read_attributes(cp, c, fptr, attr[i].info.Code.attributes);
                    }
                    break;
                default:
                    fseek(fptr, attr[i].attribute_length, SEEK_CUR);
                    break;
                }
        }
}
