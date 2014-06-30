#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define ALPHABET_LEN 255
#define NOT_FOUND patlen
#define max(a, b) ((a < b) ? b : a)

#define DEBUG

int chars_compared;

// delta1 table: delta1[c] contains the distance between the last
// character of pat and the rightmost occurrence of c in pat.
// If c does not occur in pat, then delta1[c] = patlen.
// If c is at string[i] and c != pat[patlen-1], we can
// safely shift i over by delta1[c], which is the minimum distance
// needed to shift pat forward to get string[i] lined up 
// with some character in pat.
// this algorithm runs in alphabet_len+patlen time.
void make_delta1(int *delta1, uint8_t *pat, int32_t patlen) {
    int i;
#ifdef DEBUG
    int j;
    int delta1_chars[patlen];
    int max_chars = 0;
    bool is_matched = false;
#endif

    for (i=0; i < ALPHABET_LEN; i++) {
        delta1[i] = NOT_FOUND;
    }
    for (i=0; i < patlen-1; i++) {
        delta1[pat[i]] = patlen-1 - i;
#ifdef DEBUG
        is_matched = false;
        for (j=0; j <= max_chars; j++) {
            if (delta1_chars[j] == pat[i]) {
                is_matched = true;
            }
        }
        if (is_matched == false) {
            delta1_chars[max_chars] = pat[i];
            max_chars++;
        }
#endif        
    }
#ifdef DEBUG
    int t;
    printf ("\n");
    for (t=0; t < max_chars; t++) 
        printf("delta1[%c] = %d\n", delta1_chars[t], delta1[delta1_chars[t]]);
    printf("delta1[other] = %d\n", NOT_FOUND);
#endif
}

// true if the suffix of word starting from word[pos] is a prefix 
// of word
int is_prefix(uint8_t *word, int wordlen, int pos) {
    int i;
    int suffixlen = wordlen - pos;

    for (i=0; i < suffixlen; i++) {
        if (word[i] != word[pos+i]) {
            return 0;
        }
    }
    return 1;
}

// length of the longest suffix of word ending on word[pos].
// suffix_length("dddbcabc", 8, 4) = 2
int suffix_length(uint8_t *word, int wordlen, int pos) {
    int i;
    // increment suffix length i to the first mismatch or beginning
    // of the word
    for (i = 0; (word[pos-i] == word[wordlen-1-i]) && (i < pos); i++);
    return i;
}

// delta2 table: given a mismatch at pat[pos], we want to align 
// with the next possible full match could be based on what we
// know about pat[pos+1] to pat[patlen-1].
//
// In case 1:
// pat[pos+1] to pat[patlen-1] does not occur elsewhere in pat,
// the next plausible match starts at or after the mismatch.
// If, within the substring pat[pos+1 .. patlen-1], lies a prefix
// of pat, the next plausible match is here (if there are multiple
// prefixes in the substring, pick the longest). Otherwise, the
// next plausible match starts past the character aligned with 
// pat[patlen-1].
// 
// In case 2:
// pat[pos+1] to pat[patlen-1] does occur elsewhere in pat. The
// mismatch tells us that we are not looking at the end of a match.
// We may, however, be looking at the middle of a match.
// 
// The first loop, which takes care of case 1, is analogous to
// the KMP table, adapted for a 'backwards' scan order with the
// additional restriction that the substrings it considers as 
// potential prefixes are all suffixes. In the worst case scenario
// pat consists of the same letter repeated, so every suffix is
// a prefix. This loop alone is not sufficient, however:
// Suppose that pat is "ABYXCDEYX", and text is ".....ABYXCDEYX".
// We will match X, Y, and find B != E. There is no prefix of pat
// in the suffix "YX", so the first loop tells us to skip forward
// by 9 characters.
// Although superficially similar to the KMP table, the KMP table
// relies on information about the beginning of the partial match
// that the BM algorithm does not have.
//
// The second loop addresses case 2. Since suffix_length may not be
// unique, we want to take the minimum value, which will tell us
// how far away the closest potential match is.
void make_delta2(int *delta2, uint8_t *pat, int32_t patlen) {
    int p;
    int last_prefix_index = 1;

    // first loop, prefix pattern
    for (p=patlen-1; p>=0; p--) {
        if (is_prefix(pat, patlen, p+1)) {
            last_prefix_index = p+1;
        }
        delta2[p] = (patlen-1 - p) + last_prefix_index;
    }

    // this is overly cautious, but will not produce anything wrong
    // second loop, suffix pattern
    for (p=0; p < patlen-1; p++) {
        int slen = suffix_length(pat, patlen, p);
        if (pat[p - slen] != pat[patlen-1 - slen]) {
            delta2[patlen-1 - slen] = patlen-1 - p + slen;
        }
    }
#ifdef DEBUG
    int t;
    printf ("\n");
    for (t=0; t < patlen; t++) {
        printf("delta2[%c]: %d\n", pat[t], delta2[t]);
    }
#endif    
}

uint32_t boyer_moore (uint8_t *string, uint32_t stringlen, uint8_t *pat, uint32_t patlen) {
    int i;
    int delta1[ALPHABET_LEN];
    int *delta2 = malloc(patlen * sizeof(int));
    make_delta1(delta1, pat, patlen);
    make_delta2(delta2, pat, patlen);
    int n_shifts = 0;
    chars_compared = 0;
#ifdef DEBUG
    printf ("\n");
#endif
    i = patlen-1;
    while (i < stringlen) {
        int j = patlen-1;
        while (j >= 0 && (string[i] == pat[j])) {
            --i;
            --j;
            chars_compared++;
        }
        if (j < 0) {
            free(delta2);
            return (uint32_t) i+1;
        }
        chars_compared++;
#ifdef DEBUG
        n_shifts++;
        if (delta1[string[i]] > delta2[j]) {
            printf("shift #%d, using delta1, shifting by %d\n", n_shifts, delta1[string[i]]);
        } else {
            printf("shift #%d, using delta2, shifting by %d\n", n_shifts, delta2[j]);
        }
#endif

        i += max(delta1[string[i]], delta2[j]);
    }
    free(delta2);
    return 0;
}

void test(uint8_t *string, uint8_t *pat) {
    printf("-------------------------------------------------------\n");
    printf("Looking for '%s' in '%s':\n", pat, string);

    uint32_t pos = boyer_moore(string, strlen(string), pat, strlen(pat));
#ifdef DEBUG
    printf("\n");
#endif
    if (pos == 0)
        printf("Not Found - %d chars compared.\n", chars_compared);
    else 
        printf("Found at position %u - %d chars compared.\n", pos, chars_compared);
}

int main(int argc, char const *argv[]) {
    test(".....ABYXCDEYX", "ABYXCDEYX");
    test("WHICH-FINALLY-HALTT-THAT", "TT-THAT");
    test("....ccbc.zbc", "ccbc.zbc");
    test("...........................", "ABYXCDEYX");
    test("..cbcabcabc", "bcabcabc");
    test("..adbdadbda", "adbda");

    return 0;
}
