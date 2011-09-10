
#include <yyast/count.h>

ast_position_t previous_position = {0, 0, 0};
ast_position_t current_position = {0, 0, 0};

ast_t ast_count(char *s, size_t s_length)
{
    off_t   i;
    char    c;
    int     ascii;
    int     utf8_start;
    int     printable_ascii;
    ast_t   r;

    previous_position = current_position;
    current_position.position+= s_length;

    // For the actual column and line we have to read the characters.
    for (i = 0; i < s_length; i++) {
        switch (c = s[i]) {
        case '\n': // Line feed goes to the next line and to the left.
            current_position.line++;
            current_position.column = 0;
            break;
        case '\r': // Carriage return only goes to the left.
            current_position.column = 0;
            break;
        case '\t': // Tab moves cursor to the right on the next 8 column boundary.
            current_position.column+= 8 - (current_position.column % 8);
        default:
            printable_ascii = (c >= ' ') && (c <= '~');
            utf8_start = (c & 0xc0) == 0x80;

            if (printable_ascii || utf8_start) {
                // Only increment column on ASCII and UTF-8 start marker.
                current_position.column++;
            }
        }
    }
    r.node = NULL;
    r.start = previous_position;
    r.end = current_position;
    return r;
}

