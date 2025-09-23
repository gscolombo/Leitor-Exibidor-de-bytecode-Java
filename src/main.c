#include "utils.h"
#include "reader.h"

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
            printf("Magic: %#X\n", cf->magic);
            printf("Version: %i.%i\n", cf->major_version, cf->minor_version);
            printf("Constant Pool count: %i\n", cf->constant_pool_count);
        }

        free(cf);
    }

    return 0;
}