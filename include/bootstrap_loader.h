#ifndef BOOTSTRAP_LOADER_H
#define BOOTSTRAP_LOADER_H

#include <stdio.h>

#include "Classfile.h"
#include "reader.h"
#include "MethodArea.h"
#include "method_area_utils.h"


Class *bootstrap_loader(char *, MethodArea *, const char *);

#endif