#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "opcode_table.h"

void invoke_method(Class *, Method *, java_type *, Frame *, MethodArea *);

void execute_method(Frame *);

void push_operand(Frame *, java_type);

java_type pop_operand(Frame *);

#endif