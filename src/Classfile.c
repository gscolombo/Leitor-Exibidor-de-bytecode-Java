#include "Classfile.h"
#include "tags.h"

void free_classfile(ClassFile *cf)
{
    cp_info *cp = cf->constant_pool;

    free(cf->interfaces);
    free(cf->fields);

    for (size_t i = 0; i < cf->methods_count; i++)
        free_attributes(cp, cf->methods[i].attributes_count, cf->methods[i].attributes);
    free(cf->methods);

    for (size_t i = 1; i < (size_t)cf->constant_pool_count - 1; i++)
    {
        if (cp[i].tag == CONSTANT_UTF8)
        {
            free(cp[i].info.UTF8.bytes);
            free(cp[i].info.UTF8.str);
        }
    }

    free(cf->constant_pool);
}

void free_attributes(cp_info *cp, u2 count, attribute *attr)
{
    if (count > 0)
        for (size_t i = 0; i < count; i++)
        {
            attribute_name attr_name = *convert_attr_name(cp[attr[i].attribute_name_index - 1].info.UTF8.str);
            switch (attr_name)
            {
            case Code:
                free(attr[i].info.Code.code);
                free(attr[i].info.Code.exception_table);
                free_attributes(cp, attr[i].info.Code.attributes_count, attr[i].info.Code.attributes);
                break;
            case LineNumberTable:
                free(attr[i].info.LineNumberTable.line_number_table);
            default:
                break;
            }
        }
    
    free(attr);
}
