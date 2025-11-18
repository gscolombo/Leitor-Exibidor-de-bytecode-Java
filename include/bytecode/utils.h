#ifndef BYTECODE_UTILS_H
#define BYTECODE_UTILS_H

#include <stdint.h>
#include "uinteger.h"
#include "Frame.h"
#include "interpreter.h"

extern int32_t get_switch_32B_values(u4, const u1 *);
void _print(Frame *, const char *, char, char);

void init_stringbuffer(Frame *, dtype *);
void strbuf_append(Frame *);
void strbuf_tostring(Frame *);

void allocref(Frame *);

#endif