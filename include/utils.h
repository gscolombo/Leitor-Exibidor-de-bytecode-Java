/**
 * @brief Get class identifier from a .class file
 * 
 * This function retrieves the class identifier from the .class file path.
 * It remove the ".class" at the end of the path and any trailing "/" or "."
 * at the beginning.
 * 
 * @param path String with the path for the .class file
 * @param cls String to store the class identifier
 * 
 * @return None. It changes the `cls` parameter inplace.
 */
void getClassName(char*, char*);

/**
 * @brief Swaps the two bytes of a 16-bit integer.
 *
 * This function takes a 16-bit integer and swaps its lower and upper bytes.
 * For example, if the input is 0x1234, the output will be 0x3412.
 *
 * @param n The 16-bit integer to swap bytes.
 * @return The 16-bit integer with its bytes swapped.
 */
int _16bswap(int);

/**
 * @brief Swaps the byte order of a 32-bit integer.
 *
 * This function reverses the order of the bytes in a 32-bit integer.
 * For example, given an input of 0x12345678, the output will be 0x78563412.
 *
 * @param n The 32-bit integer to swap.
 * @return The 32-bit integer with its bytes reversed.
 */
int _32bswap(int);