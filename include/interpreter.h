#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "opcode_table.h"

Frame *invoke_method(ClassFile *, member_info *, java_type *, Frame *, MethodArea *);

member_info *find_method(ClassFile *, const char *);

Frame *create_frame(ClassFile *, member_info *, java_type *, Frame *);

Frame *execute_method(Frame *);

void push_operand(Frame *, java_type);

java_type pop_operand(Frame *);

cp_info *get_constant_value(cp_info *, u2);

#endif