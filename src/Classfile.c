#include "Classfile.h"
#include "tags.h"

void free_classfile(ClassFile *cf)
{
    cp_info *cp = cf->constant_pool;
    for (size_t i = 1; i < (size_t)cf->constant_pool_count - 1; i++)
    {
        if (cp[i].tag == CONSTANT_UTF8)
        {
            free(cp[i].info.UTF8.bytes);
            free(cp[i].info.UTF8.str);
        }
    }
    free(cf->constant_pool);
    
    free(cf->interfaces);
    free(cf->fields);
}