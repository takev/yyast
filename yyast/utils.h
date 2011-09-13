#ifndef YA_UTILS_H
#define YA_UTILS_H

#include <stdlib.h>
#include <stdint.h>

typedef uint32_t fourcc_t;

/** Convert a fourcc string into an 32 bit integer.
 * @param s    A four character string.
 * @returns    An integer representing a four character string.
 */
uint32_t fourcc(char *s);

/** Align to a 32 bit boundary.
 * @param x    A size of an object in memory
 * @returns    Either the size if it was aligned, or the next larger size that is aligned.
 */
size_t ya_align32(size_t x);

/** Convert an UTF-8 string into an UCS-4 string.
 * @param in       The UTF-8 string to convert.
 * @param in_size  The size in bytes of the UTF-8 string to convert.
 * @param out      The pre allocated memory the UCS-4 string should be entered, worst case is in_size * 4.
 * @returns        The number of UCS-4 characters emited.
 */
size_t ya_utf8_to_ucs4(const char *_in, size_t in_size, uint32_t *out);

/** Escape characters in a string.
 * The original string is modified by this function.
 *
 * @param string         The UCS-4 string with escape characters.
 * @param string_length  The number of characters in the string.
 * @param raw            When 0 standard escape sequence is decoded.
 *                       When 1 only the double quote escape \" is decoded.
 *                       When 2 only the double quote slash \/ is decoded.
 * @returns              The number of characters after decoding.
 */
size_t ya_string_escape(uint32_t *string, size_t string_length, int raw);

#endif
