#ifndef BOOTSTRAP_LOADER_H
#define BOOTSTRAP_LOADER_H

#include <stdio.h>

#include "Classfile.h"
#include "reader.h"
#include "MethodArea.h"
#include "cp/writer.h"

ClassFile *bootstrap_loader(const char *, MethodArea *);

#endif