#include "parser.h"
#include "reader.h"


cp_info* parse_constant_pool(FILE* fptr, u2 count) {
    cp_info *constant_pool = (cp_info *) malloc(sizeof (cp_info) * count);
    cp_info *cp = constant_pool;

    while (cp < constant_pool + count - 1) {
        cp->tag = read_u1(fptr);
        printf("Tag: %i\n", cp->tag);
        switch (cp->tag)
        {
        case CONSTANT_Class:
            cp->info.Class.name_index = read_u2(fptr);
            break;
        
        default:
            break;
        }

        ++cp;
    }

    return cp - (count - 1);
}