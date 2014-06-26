#include <stdlib.h>
#include <stdio.h>

#define PATTERN_LENGTH 4
unsigned char original_text[] = "On a dark desert highway, cool wind in my hair.";
unsigned char original_pattern[PATTERN_LENGTH + 1 ] = "waind";

int bm_search(unsigned char *text, unsigned char *pattern)
{
    int table[256];
    int text_index, pattern_index, text_len, i;

    for ( i = 0; i < 256; i++ ) {
        table[i] = PATTERN_LENGTH;
    }
    for ( i = 0; i < PATTERN_LENGTH - 1; i ++ ) {
        table[pattern[i]] = PATTERN_LENGTH - i - 1;
    }
    for ( text_len = 0; text[text_len] != '\0'; ++text_len )
        ;

    text_index = PATTERN_LENGTH - 1;
    while ( text_index < text_len ) {
        pattern_index = PATTERN_LENGTH - 1;
        while ( text[text_index] == pattern[pattern_index] ) {
            if ( pattern_index == 0 ) {
                return text_index;
            }
            text_index--;
            pattern_index--;
        }

    if ( table[text[text_index]] > PATTERN_LENGTH - pattern_index ) 
        text_index += table[text[text_index]];
    else
        text_index += PATTERN_LENGTH - pattern_index;

    }
    return -1;
}

int main(void)
{
    int index;
    index = bm_search(original_text, original_pattern);

    if ( index ) {
        printf("%d\n", index);
    }
    else {
        printf("not found\n");
    }
    return EXIT_SUCCESS;
}
