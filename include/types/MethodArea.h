#ifndef METHOD_AREA_H
#define METHOD_AREA_H

#include <stdio.h>
#include "Classfile.h"

typedef struct
{
    size_t num_classes;
    ClassFile *classes;
} MethodArea;

#endif