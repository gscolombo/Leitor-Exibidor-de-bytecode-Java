#ifndef PRINTER_H
#define PRINTER_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "uinteger.h"
#include "cp/writer.h"
#include "opcode_table.h"
#include "bytecode/utils.h"

void show_opcodes(const u1 *, u4, const cp_info *);

#endif
