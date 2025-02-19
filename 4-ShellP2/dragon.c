#include <stdio.h>

#define DRAGON_ART_ARRAY { 32, 72, 64, 1, 37, 4, 32, 23, 10, 1, 32, 69, 37, 6, 32, 25, 10, 1, 32, 68, 37, 6, 32, 26, 10, 1, 32, 65, 37, 1, 32, 1, 37, 7, 32, 11, 64, 1, 32, 14, 10, 1, 32, 64, 37, 10, 32, 8, 37, 7, 32, 11, 10, 1, 32, 39, 37, 7, 32, 2, 37, 4, 64, 1, 32, 9, 37, 12, 64, 1, 32, 4, 37, 6, 32, 2, 64, 1, 37, 4, 32, 8, 10, 1, 32, 34, 37, 22, 32, 6, 37, 28, 32, 10, 10, 1, 32, 32, 37, 26, 32, 3, 37, 12, 32, 1, 37, 15, 32, 11, 10, 1, 32, 31, 37, 29, 32, 1, 37, 19, 32, 5, 37, 3, 32, 12, 10, 1, 32, 29, 37, 28, 64, 1, 32, 1, 64, 1, 37, 18, 32, 8, 37, 2, 32, 12, 10, 1, 32, 28, 37, 33, 32, 1, 37, 22, 32, 16, 10, 1, 32, 28, 37, 58, 32, 14, 10, 1, 32, 28, 37, 50, 64, 1, 37, 6, 64, 1, 32, 14, 10, 1, 32, 6, 37, 8, 64, 1, 32, 11, 37, 16, 32, 8, 37, 26, 32, 6, 37, 2, 32, 16, 10, 1, 32, 4, 37, 13, 32, 9, 37, 2, 64, 1, 37, 12, 32, 11, 37, 11, 32, 1, 37, 12, 32, 6, 64, 1, 37, 1, 32, 16, 10, 1, 32, 2, 37, 10, 32, 3, 37, 3, 32, 8, 37, 14, 32, 12, 37, 24, 32, 24, 10, 1, 32, 1, 37, 9, 32, 7, 37, 1, 32, 9, 37, 13, 32, 13, 37, 12, 64, 1, 37, 11, 32, 23, 10, 1, 37, 9, 64, 1, 32, 16, 37, 1, 32, 1, 37, 13, 32, 12, 64, 1, 37, 25, 32, 21, 10, 1, 37, 8, 64, 1, 32, 17, 37, 2, 64, 1, 37, 12, 32, 12, 64, 1, 37, 28, 32, 18, 10, 1, 37, 7, 64, 1, 32, 19, 37, 15, 32, 11, 37, 33, 32, 14, 10, 1, 37, 10, 32, 18, 37, 15, 32, 10, 37, 35, 32, 6, 37, 4, 32, 2, 10, 1, 37, 9, 64, 1, 32, 19, 64, 1, 37, 14, 32, 9, 37, 12, 64, 1, 32, 1, 37, 4, 32, 1, 37, 17, 32, 3, 37, 8, 10, 1, 37, 10, 32, 18, 37, 17, 32, 8, 37, 13, 32, 6, 37, 18, 32, 1, 37, 9, 10, 1, 37, 9, 64, 1, 37, 2, 64, 1, 32, 16, 37, 16, 64, 1, 32, 7, 37, 14, 32, 5, 37, 24, 32, 2, 37, 2, 10, 1, 32, 1, 37, 10, 32, 18, 37, 1, 32, 1, 37, 14, 64, 1, 32, 8, 37, 14, 32, 3, 37, 26, 32, 1, 37, 2, 10, 1, 32, 2, 37, 12, 32, 2, 64, 1, 32, 11, 37, 18, 32, 8, 37, 40, 32, 2, 37, 3, 32, 1, 10, 1, 32, 3, 37, 13, 32, 1, 37, 2, 32, 2, 37, 1, 32, 2, 37, 1, 64, 1, 32, 1, 37, 18, 32, 10, 37, 37, 32, 4, 37, 3, 32, 1, 10, 1, 32, 4, 37, 18, 32, 1, 37, 22, 32, 11, 64, 1, 37, 31, 32, 4, 37, 7, 32, 1, 10, 1, 32, 5, 37, 39, 32, 14, 37, 28, 32, 8, 37, 3, 32, 3, 10, 1, 32, 6, 64, 1, 37, 35, 32, 18, 37, 25, 32, 15, 10, 1, 32, 8, 37, 32, 32, 22, 37, 19, 32, 2, 37, 7, 32, 10, 10, 1, 32, 11, 37, 26, 32, 27, 37, 15, 32, 2, 64, 1, 37, 9, 32, 9, 10, 1, 32, 14, 37, 20, 32, 11, 64, 1, 37, 1, 64, 1, 37, 1, 32, 18, 64, 1, 37, 18, 32, 3, 37, 3, 32, 8, 10, 1, 32, 18, 37, 15, 32, 8, 37, 10, 32, 20, 37, 15, 32, 4, 37, 1, 32, 9, 10, 1, 32, 16, 37, 36, 32, 22, 37, 14, 32, 12, 10, 1, 32, 16, 37, 26, 32, 2, 37, 4, 32, 1, 37, 3, 32, 22, 37, 10, 32, 2, 37, 3, 64, 1, 32, 10, 10, 1, 32, 21, 37, 19, 32, 1, 37, 6, 32, 1, 37, 2, 32, 26, 37, 13, 64, 1, 32, 10, 10, 1, 32, 81, 37, 7, 64, 1, 32, 7, 0 }


/*
    Input: an array of integers, in the format { char, count, ...}
    Output: prints the ascii art originally compressed

    Array generated by ../3-ShellP1/art_to_array.c
*/

void print_art_array(int art[]) {

    for (int i = 0; art[i] != 0; i += 2) {

        char current_char = (char) art[i];
        for (int count = 0; count < art[i + 1]; count++) {
            printf("%c", current_char);
        }

    }

    printf("\n");

}


// EXTRA CREDIT - print the drexel dragon from the readme.md
extern void print_dragon(){

    int dragon[] = DRAGON_ART_ARRAY;
    print_art_array(dragon);

}