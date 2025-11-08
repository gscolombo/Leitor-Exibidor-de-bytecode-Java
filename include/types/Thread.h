#ifndef THREAD_H
#define THREAD_H

#include "uinteger.h"
#include "Frame.h"

typedef struct {
    int16_t top;
    Frame *stack;
} JVMStack;

typedef struct {
    u4 pc_register;
    u4 stack_size;
    Frame *current_frame;
} Thread;

#endif 