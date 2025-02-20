#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"
#include <errno.h>

// strtok and strsep want strings, not chars, so let's convert in a somewhat creative way
#define PIPE_STR (char[2]) { (char) PIPE_CHAR, '\0' }
#define SPACE_STR (char[2]) { (char) SPACE_CHAR, '\0' }

extern void print_dragon();

int last_rc = 0;

/*

    Input: a string of user input
    Output: mutates input into clean, standardized input

    Trim ALL leading and trailing spaces
    Eliminate duplicate spaces UNLESS they are in a quoted string
    Account for quoted strings in input; treat a quoted string with spaces as a single argument

*/

void clean_input(char* cmd_line) {

    // trim leading whitespace
    int start = 0;
    while (cmd_line[start] == ' ') start++;

    if (start != 0) {

        int i;
        for (i = 0; cmd_line[start + i] != '\0'; i++) {
            cmd_line[i] = cmd_line[start + i];
        }
        cmd_line[i] = '\0';

    }

    // trim tailing whitespace
    char* end = cmd_line + strlen(cmd_line) - 1;
    while (end > cmd_line && *end == SPACE_CHAR) {
        *end = '\0';
        end--;
    }

    // eliminate duplicate spaces, excluding those inside quotes
    size_t i = 0;
    bool is_prev_space = false;
    bool is_in_quotes = false;
    while (i < strlen(cmd_line)) {

        if (cmd_line[i] == '"') {
            is_in_quotes = !is_in_quotes;
        }

        if (cmd_line[i] == ' ' && !is_in_quotes) {

            if (!is_prev_space) {
                is_prev_space = true;
                i++;
            } else {
                
                // duplicate space found, delete
                size_t j;
                for (j = i; j < strlen(cmd_line); j++) {
                    cmd_line[j] = cmd_line[j + 1];
                }
                cmd_line[j + 1] = '\0';
                
            }

        } else {

            is_prev_space = false;
            i++;

        }
    }

}


/*

    Input:  cmd_line: cleaned-up command line from user
            *cmd: pointer to struct to be populated with parsed input
    Output: *cmd is fully populated

*/

int build_cmd_buff(char* cmd_line, cmd_buff_t* cmd) {

    char* cmd_copy = strdup(cmd_line);

    char* ptr = cmd_copy;
    cmd->argc = 0;

    while (*ptr) {

        while (*ptr && isspace((unsigned char)*ptr)) ptr++; // skip leading spaces, some may appear while tokenizing

        if (*ptr == '\0') break;

        char* start = ptr;
        char* token;

        // handle quotation marks
        if (*ptr == '"') {

            ptr++;  // skip first quote
            start = ptr;
            while (*ptr && *ptr != '"') ptr++; // traverse until next quotation mark or end of string
            if (*ptr == '"') {
                *ptr = '\0';
                ptr++;
            }

        } else {

            while (*ptr && *ptr != ' ') ptr++; // skip spaces
            if (*ptr) *ptr++ = '\0';
            
        }

        token = strdup(start);

        if (cmd->argc >= ARG_MAX) {
            free(token);
            free(cmd_copy);
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        cmd->argv[cmd->argc++] = token;
    }

    if (cmd->argc == 0) {
        free(cmd_copy);
        return WARN_NO_CMDS;
    }

    free(cmd_copy);
    return OK;

}


int clear_cmd_buff(cmd_buff_t* cmd_buff) {

    cmd_buff->argc = 0;
    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));
    return OK;

}


/*

    Input: command string, usually argv[0]
    Return: corresponding enumerated value

*/

Built_In_Cmds match_command(const char *cmd) {

    if (strcmp(cmd, "exit") == 0) {
        return BI_CMD_EXIT;
    }

    if (strcmp(cmd, "dragon") == 0) {
        return BI_CMD_DRAGON;
    }

    if (strcmp(cmd, "cd") == 0) {
        return BI_CMD_CD;
    }

    if (strcmp(cmd, "rc") == 0) {
        return BI_RC;
    }

    return BI_NOT_BI;

}


/*

    Input: cmd_buff_t that should be executed
    Return: BI_EXECUTED: ran correctly
            BI_NOT_BI: not a recognized built-in command

*/

Built_In_Cmds exec_built_in_cmd(cmd_buff_t* cmd_buff) {

    Built_In_Cmds cmd = match_command(cmd_buff->argv[0]);

    switch (cmd) {

        case BI_CMD_EXIT:
            exit(0);
        
        case BI_CMD_DRAGON:
            print_dragon();
            break;
        
        case BI_CMD_CD:
            // the cd command should chdir to the provided directory; if no directory is provided, do nothing
            if (cmd_buff->argc >= 2) chdir(cmd_buff->argv[1]);
            break;
        
        case BI_RC:
            printf("%d\n", last_rc);
            break;
        
        default:
            return BI_NOT_BI;

    }

    return BI_EXECUTED;

}


/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop()
{
    char cmd_buff[SH_CMD_MAX];
    cmd_buff_t cmd;


    // TODO IMPLEMENT MAIN LOOP
    while (1) {

        printf("%s", SH_PROMPT);

        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) { // for running headless
            printf("\n");
            break;
        }

        if (!strchr(cmd_buff, '\n')) {

            // input was way too long, clear input stream and ignore
            char c = 0;
            while (c != '\n' && c != EOF) {
                c = getchar();
            }
            continue;

        }

        // remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        // parse input to cmd_buff_t *cmd_buff
        clean_input(cmd_buff);
        clear_cmd_buff(&cmd);
        int parse_rc = build_cmd_buff(cmd_buff, &cmd);

        if (parse_rc == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            continue;
        }

        if (parse_rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        // if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
        Built_In_Cmds built_in_rc = exec_built_in_cmd(&cmd);
        if (built_in_rc == BI_EXECUTED) {
            last_rc = 0;
            continue;
        }

        // TODO IMPLEMENT if not built-in command, fork/exec as an external command
        // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"
        pid_t fork_rc = fork();
        if (fork_rc < 0) {
            continue;
        }

        if (fork_rc == 0) {

            // child
            execvp(cmd.argv[0], cmd.argv);
            exit(errno);

        } else {

            // parent
            int child_result;
            wait(&child_result);
            int child_rc = WEXITSTATUS(child_result);
            last_rc = child_rc;

            if (child_rc != 0) {
                switch(child_rc) {
                    case ENOENT:
                        printf(CMD_ERR_EXECUTE, "Command not found in PATH\n");
                        break;
                    
                    case EACCES:
                        printf(CMD_ERR_EXECUTE, "Permission denied\n");
                        break;
                    
                    case ENOTDIR:
                        printf(CMD_ERR_EXECUTE, "Invalid path\n");
                        break;

                    case EISDIR:
                        printf(CMD_ERR_EXECUTE, "Unable to execute directory\n");
                        break;
                    
                    case ENOMEM:
                        printf(CMD_ERR_EXECUTE, "Not enough memory to execute\n");
                        break;
                    
                    default:
                        printf(CMD_ERR_EXECUTE, "Execution failed\n");
                }
            }

        }
    
    }

    return OK;
}