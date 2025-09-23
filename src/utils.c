#include "utils.h"

void getClassName(char* path, char* cls) {
    int j = path[0] == '.' ? 2 : 0;
    int i = 0;

    while (path[j] != '.') {
        cls[i] = path[j] == '/' ? '.' : path[j];
        i++;
        j++;
    }
    cls[i] = '\0';
}

int _16bswap(int n) {
    return ((n>>8)&0xff) | // Change byte 0 with byte 1
            ((n<<8)&0xff00);  // Change byte 1 with byte 0
}

int _32bswap(int n) {
    /*
        In the expression below, the "|" operators concatenate
        the word (32 bits) byte segments, while the "&" operator
        used with "0x..ff.." choose the byte to be moved.

        Example: 
        n = 0x12345678
        a <= ((n>>24)&0xff) = 0x00000012 & 0x000000ff = (...0 0001 0010) & (...0 1111 1111) = (...0 0001 0010) = 0x00000012
        b <= ((n<<8)&0xff0000) = 0x34567800 & 0x00ff0000 = (0011 0100 0101 0110 0111 1000 0...) & (0000 0000 1111 1111 0...) = (0000 0000 0101 0110 0...) = 0x00560000
        a | b <= 0x00000012 | 0x00560000 = 0x00560012
        The same for bytes 2 and 3
    */
    return ((n>>24)&0xff) | // Change byte 3 with byte 0 (0xff = 1111 1111)
            ((n>>8)&0xff00) | // Change byte 1 with byte 2
            ((n<<8)&0xff0000) | // Change byte 2 with byte 1
            ((n<<24)&0xff000000); // Change byte 0 with byte 3
}

unsigned int num_digits(unsigned int n) {
    if (n < 10) return 1;
    
    int r = 2;
    while ((n /= 10) > 9) {
        ++r;
    }

    return r;
}