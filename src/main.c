#include "utils.h"
#include "reader.h"
#include "writer.h"

int main(const int argc, char *argv[]) {
    if (argc < 2) {
        printf("Invalid number of arguments. Pass the path for the .class file.\n");
        return 1;
    }

    FILE *fptr = open_classfile(argv[1]);
    
    if (fptr != NULL) {
        ClassFile cf = read_classfile(fptr);
        show_classfile(&cf);        
        free_classfile(&cf);
    }

    return 0;
}