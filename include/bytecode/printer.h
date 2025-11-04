#ifndef PRINTER_H
#define PRINTER_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "bytecode/opcodes.h"


void show_opcodes(const u1 *code, u4 length);

#endif
