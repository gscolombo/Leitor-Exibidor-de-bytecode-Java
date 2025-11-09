#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "Thread.h"
#include "opcode_table.h"
#include "MethodArea.h"

Thread *initialize_thread();

void invoke_method(Thread *, ClassFile *, member_info *, java_type *, Frame *);

void push_frame(Thread *, Frame *);

void pop_frame(Thread *, Frame *);

member_info *find_method(ClassFile *, const char *);

Frame *create_frame(ClassFile *, member_info *, java_type *, Frame *);

void execute_method(Thread *);

void free_thread(Thread *);

void push_operand(Frame *, java_type);

java_type pop_operand(Frame *);

cp_info *get_constant_value(cp_info *, u2);

#endif