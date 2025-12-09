#ifndef FRAME_H
#define FRAME_H

#include <stdbool.h>
#include "uinteger.h"
#include "types/cp/constants.h"
#include "Classfile.h"
#include "MethodArea.h"
#include "java_type.h"

typedef struct
{
    int16_t top;
    u2 capacity;
    dtype *stack;
} OperandStack;

typedef struct Frame
{
    u4 pc;
    dtype *local_variables;
    OperandStack operand_stack;
    struct Frame *previous_frame;
    Method *method;
    Class *class;
    const char *class_name; // For pointer update
    MethodArea *method_area;
} Frame;

#endif