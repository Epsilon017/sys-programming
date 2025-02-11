#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

// strtok and strsep want strings, not chars, so let's convert in a somewhat creative way
#define PIPE_STR (char[2]) { (char) PIPE_CHAR, '\0' }
#define SPACE_STR (char[2]) { (char) SPACE_CHAR, '\0' }

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    
    char* tokenized_commands[CMD_MAX] = {NULL};
    char* cmd_copy = strdup(cmd_line);
    char* current_command = strtok(cmd_copy, PIPE_STR);
    int i = 0;

    // fill 'commands' with cmd_line tokenized by pipe
    while (current_command != NULL) {

        if (i >= CMD_MAX) {

            // too many commands
            free(cmd_copy);
            return ERR_TOO_MANY_COMMANDS;

        }

        // trim leading whitespace
        while (*current_command == SPACE_CHAR) {
            current_command++;
        }

        // trim tailing whitespace
        char* end = current_command + strlen(current_command) - 1;
        while (end > current_command && *end == SPACE_CHAR) {
            *end = '\0';
            end--;
        }

        tokenized_commands[i] = strdup(current_command);
        i++;
        current_command = strtok(NULL, PIPE_STR);

    }

    // fill 'clist' with commands tokenized by space
    clist->num = i;
    for (i = 0; i < clist->num; i++) {

        current_command = tokenized_commands[i];

        char* exe = strsep(&current_command, SPACE_STR);
        char* args = current_command ? current_command : "";

        if (strlen(exe) > EXE_MAX || strlen(args) > ARG_MAX) {

            // exe or args too long
            free(cmd_copy);
            for (int j = 0; j < i; j++) {
                free(tokenized_commands[j]);
            }
            return ERR_CMD_OR_ARGS_TOO_BIG;

        }

        // copy data into command struct
        memset(&clist->commands[i], 0, sizeof(command_t));
        strcpy(clist->commands[i].exe, exe);
        strcpy(clist->commands[i].args, args);

    }

    free(cmd_copy);
    for (i = 0; i < clist->num; i++) {
        free(tokenized_commands[i]);
    }

    return OK;

}