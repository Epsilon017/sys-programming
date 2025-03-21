#include <stdio.h>
#include <stdlib.h>

#define OUTPUT_PATH "art_as_array.txt"

/*
    Input: an array of integers, generated by this program
    Output: prints the ascii art originally compressed

    Just copy to wherever it's needed, can't "include" this file since it has a main()
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


/*
    Input: a file containing ASCII art
    Output: a file containing an array that can be pasted into C code

    Output structure:
        Every odd number represents a character
        Every even number represents how many times that character should appear
        The last element of the array is always 0
    
    Example:
        { 40, 5, 23, 10, 0 }
        40 = '@' * 5, 23 = '#' * 10, 0 = EOF
*/

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf("Missing filename.\n");
        exit(-1);
    }

    FILE* input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Unable to open file");
        exit(-2);
    }

    FILE* output = fopen(OUTPUT_PATH, "w");
    fprintf(output, "%s", "{ ");
    char current_char = fgetc(input);
    
    while (current_char != EOF) {

        fprintf(output, "%d, ", (int) current_char);

        char last_char = current_char;
        int char_count = 0;
        while (current_char == last_char) {
            char_count++;
            current_char = fgetc(input);
        }

        fprintf(output, "%d, ", char_count);

    }

    fprintf(output, "%s", "0 }");

    fclose(input);
    if (fclose(output)) {
        perror("Unable to write file");
        exit(-2);
    }
    exit(0);

}