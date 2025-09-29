#ifndef UTILS_H_
#define UTILS_H_

/**
 * @brief Swaps the two bytes of a 16-bit integer.
 *
 * This function takes a 16-bit integer and swaps its lower and upper bytes.
 * For example, if the input is 0x1234, the output will be 0x3412.
 *
 * @param n The 16-bit integer to swap bytes.
 * @return The 16-bit integer with its bytes swapped.
 */
unsigned int u2swap(unsigned int);

/**
 * @brief Swaps the byte order of a 32-bit integer.
 *
 * This function reverses the order of the bytes in a 32-bit integer.
 * For example, given an input of 0x12345678, the output will be 0x78563412.
 *
 * @param n The 32-bit integer to swap.
 * @return The 32-bit integer with its bytes reversed.
 */
unsigned int u4swap(unsigned int);

/**
 * @brief Returns the number of digits of a non-negative integer.
 * 
 * @param n A non-negative integer.
 * @return The number of digits of `n`.
 */
unsigned int num_digits(unsigned int);

#endif