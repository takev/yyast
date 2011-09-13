
#include <sys/types.h>
#include <yyast/count.h>

ya_position_t ya_previous_position = {0, 0, 0};
ya_position_t ya_current_position = {0, 0, 0};

ya_t ya_count(char *s, size_t s_length)
{
    uint32_t i;
    char     c;
    int      ascii;
    int      utf8_start;
    int      printable_ascii;
    ya_t     r;

    ya_previous_position = ya_current_position;
    ya_current_position.position+= s_length;

    // For the actual column and line we have to read the characters.
    for (i = 0; i < s_length; i++) {
        switch (c = s[i]) {
        case '\n': // Line feed goes to the next line and to the left.
            ya_current_position.line++;
            ya_current_position.column = 0;
            break;
        case '\r': // Carriage return only goes to the left.
            ya_current_position.column = 0;
            break;
        case '\t': // Tab moves cursor to the right on the next 8 column boundary.
            ya_current_position.column+= 8 - (ya_current_position.column % 8);
        default:
            printable_ascii = (c >= ' ') && (c <= '~');
            utf8_start = (c & 0xc0) == 0x80;

            if (printable_ascii || utf8_start) {
                // Only increment column on ASCII and UTF-8 start marker.
                ya_current_position.column++;
            }
        }
    }
    r.node = NULL;
    r.start = ya_previous_position;
    r.end = ya_current_position;
    return r;
}

