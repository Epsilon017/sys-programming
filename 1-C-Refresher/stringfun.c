#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define BUFFER_SZ 50

#define EXIT_SUCCESS 0 // ran correctly
#define EXIT_CMD_LINE_ERR 1 // error with command line arguments
#define EXIT_MEM_ALLOC_ERR 2 // error allocating memory
#define EXIT_SERVICE_ERR 3 // error with a provided service

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);
int  count_words(char *, int, int);


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions

    // check string length before doing anything
    for (int i = 0; *(user_str + i) != '\0'; i++) {
        if (i > len) {
            return -1; // user string is too large
        }
    }

    int user_str_index = 0;
    int buff_index = 0;
    char current_char = *user_str;
    bool last_char_whitespace = false;

    while (current_char != '\0') {

        // replace non-space whitespace with space
        if (current_char == '\t') {
            current_char = ' ';
        }

        // add character to buffer, avoiding duplicate whitespace
        if (current_char != ' ' || !last_char_whitespace) {
            *(buff + buff_index) = current_char;
            buff_index++;
        }

        last_char_whitespace = (current_char == ' ');

        user_str_index++;
        current_char = *(user_str + user_str_index);
    }

    // fill remaining space with periods
    while (buff_index < len) {
        *(buff + buff_index) = '.';
        buff_index++;
    }

    return user_str_index;
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    //YOU MUST IMPLEMENT
    return 0;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      If argv[1] does not exist, it will equal the null pointer. In C, the null pointer can be evaluated to 0. The dash in the
    //      condition has an associated integer value of 45. This means the condition would just be testing if 0 is equal to 45, which
    //      would safely come out as false.
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      If the help flag isn't raised, then an option plus a string is required, so argc must be at least 3.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    //          Git directions say to use return code 2, I will be using that.
    buff = malloc(sizeof(input_string));
    if (buff == NULL) {
        printf("Unable to allocate memory.");
        exit(EXIT_MEM_ALLOC_ERR);
    }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE