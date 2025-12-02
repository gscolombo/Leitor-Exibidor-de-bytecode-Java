#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "opcode_table.h"

void invoke_method(Class *, Method *, dtype *, Frame *, MethodArea *);

void execute_method(Frame *);

void push_operand(Frame *, dtype);

dtype pop_operand(Frame *);

dtype initialize_var(type_enum, Frame *);

#endif