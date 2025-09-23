#include "utils.h"
#include "reader.h"
#include "writer.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Invalid number of arguments. Pass the path for the .class file.\n");
        return 1;
    }

    FILE* fptr;
    fptr = open_classfile(argv[1]);
    
    if (fptr != NULL) {
        char cls[255];
        getClassName(argv[1], cls);
        printf("Classfile for class %s\n\n", cls);

        ClassFile* cf = read_classfile(fptr);
        
        if (cf != NULL) {
            show_classfile(cf);
        }

        free(cf);
    }

    return 0;
}