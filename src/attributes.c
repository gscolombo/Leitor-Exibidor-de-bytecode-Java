#include "types.h"

const attribute_name *convert_attr_name(const wchar_t *name)
{
    for (size_t i = 0; i < sizeof(conversion) / sizeof(conversion[0]); i++)
    {
        if (!wcscmp(name, (const wchar_t *)conversion[i].str))
            return &conversion[i].attr;
    }

    return NULL;
}