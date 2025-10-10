#include "utils.h"

unsigned int u2swap(const unsigned int n)
{
    return ((n >> 8) & 0xff) |  // Troca o byte 0 com o byte 1
           ((n << 8) & 0xff00); // Troca o byte 1 com o byte 0
}

unsigned int u4swap(const unsigned int n)
{
    /*
        Na expressão abaixo, os operadores "|" concatenam
        os segmentos de bytes da palavra (32 bits), enquanto o operador "&"
        usado com "0x...ff..." escolhe o byte a ser movido.

        Exemplo:
        n = 0x12345678
        a <= ((n>>24)&0xff) = 0x00000012 & 0x000000ff = (...0 0001 0010) & (...0 1111 1111) = (...0 0001 0010) = 0x00000012
        b <= ((n<<8)&0xff0000) = 0x34567800 & 0x00ff0000 = (0011 0100 0101 0110 0111 1000 0...) & (0000 0000 1111 1111 0...) = (0000 0000 0101 0110 0...) = 0x00560000
        a | b <= 0x00000012 | 0x00560000 = 0x00560012
        O mesmo para os bytes 2 e 3
    */
    return ((n >> 24) & 0xff) |      // Troca o byte 3 com o byte 0
           ((n >> 8) & 0xff00) |     // Troca o byte 1 com o byte 2
           ((n << 8) & 0xff0000) |   // Troca o byte 2 com o byte 1
           ((n << 24) & 0xff000000); // Troca o byte 0 com o byte 3
}

unsigned int num_digits(unsigned int n)
{
    if (n < 10)
        return 1;

    int r = 2;
    while ((n /= 10) > 9)
    {
        ++r;
    }

    return r;
}