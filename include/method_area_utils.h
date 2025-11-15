#ifndef METHOD_AREA_UTILS_H
#define METHOD_AREA_UTILS_H

#include <string.h>

#include "reader.h"
#include "cp/writer.h"
#include "MethodArea.h"
#include "free.h"

Class *lookup_class(const char *, const MethodArea *);

Method *lookup_method(const char *, const Class *);

Class *create_and_load_class(const char *);

void cleanup(MethodArea);

#endif