#include "reader.h"
#include "types.h"

u1 read_u1(FILE* fptr) {
    u1 u;
    fread(&u, sizeof (u1), 1, fptr);

    return u;
}

u2 read_u2(FILE* fptr) {
    u2 u;
    fread(&u, sizeof (u2), 1, fptr);

    if (LITTLE_ENDIAN) {
        u = _16bswap(u);
    }

    return u;
}

u4 read_u4(FILE* fptr) {
    u4 u;
    fread(&u, sizeof (u4), 1, fptr);

    if (LITTLE_ENDIAN) {
        u = _32bswap(u);
    }

    return u;
}

FILE* open_classfile(char* path) {
    size_t l;

    if ((l = strlen(path)) > 6) {
        char *ext = &path[l - 6];

        if (strcmp(ext, ".class")) {
            printf("Wrong file format. Give the path of a \".class\" file.\n");
            return NULL;
        }
        
        return fopen(path, "rb");
    }

    printf("Invalid path.\n");
    return NULL;
}

ClassFile* read_classfile(FILE* fptr) {
    if (fseek(fptr, 0, SEEK_END) == 0) {
        size_t fsize = ftell(fptr);

        printf("Size: %lu bytes.\n", fsize);
        fseek(fptr, 0, SEEK_SET);
        ClassFile *cf = (ClassFile *) malloc(fsize);
    
        cf->magic = read_u4(fptr);
        cf->minor_version = read_u2(fptr);
        cf->major_version = read_u2(fptr);
        cf->constant_pool_count = read_u2(fptr);

        fclose(fptr);
        return cf;
    }

    return NULL;
}