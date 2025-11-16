#ifndef FRAME_H
#define FRAME_H

#include <stdbool.h>
#include "uinteger.h"
#include "constants.h"
#include "Classfile.h"
#include "MethodArea.h"
#include "java_type.h"

typedef struct
{
    int16_t top;
    u2 capacity;
    java_type *stack;
} OperandStack;

typedef struct Frame
{
    u4 pc;
    java_type *local_variables;
    OperandStack operand_stack;
    struct Frame *previous_frame;
    Method *method;
    Class *class;
    MethodArea *method_area;
} Frame;

#endif