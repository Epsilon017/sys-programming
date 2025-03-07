#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

#include "dshlib.h"

int last_rc = 0;

// strtok and strsep want strings, not chars, so let's convert in a somewhat creative way
#define SPACE_STR (char[2]) { (char) SPACE_CHAR, '\0' }

extern void print_dragon();

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
    cmd->infile = NULL;
    cmd->outfile = NULL;
    cmd->append = false;

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

        // redirection into cmd
        if (strcmp(token, "<") == 0) {

            while (*ptr && isspace((unsigned char)*ptr)) ptr++; // skip leading spaces

            if (*ptr == '\0') return ERR_BAD_REDIRECT;

            char* in_start = ptr;
            if (*ptr == '"') {

                ptr++;
                in_start = ptr;
                while (*ptr && *ptr != '"') ptr++;
                if (*ptr == '"') {
                    *ptr = '\0';
                    ptr++;
                }

            } else {

                while (*ptr && *ptr != ' ') ptr++;
                if (*ptr) {
                    *ptr = '\0';
                    ptr++;
                }

            }

            cmd->infile = strdup(in_start);

        }

        // redirection out of cmd
        else if (strcmp(token, ">") == 0 || strcmp(token, ">>") == 0) {

            cmd->append = (strcmp(token, ">>") == 0);

            while (*ptr && *ptr == ' ') ptr++; // skip leading whitespace
            if (*ptr == '\0') return ERR_BAD_REDIRECT;
            char* out_start = ptr;

            if (*ptr == '"') {

                ptr++;
                out_start = ptr;
                while (*ptr && *ptr != '"') ptr++;
                if (*ptr == '"') {
                    *ptr = '\0';
                    ptr++;
                }

            } else {

                while (*ptr && *ptr != ' ') ptr++;
                if (*ptr) {
                    *ptr = '\0';
                    ptr++;
                }

            }

            cmd->outfile = strdup(out_start);

        }

        // not redirection, "normal"
        else {

            if (cmd->argc >= ARG_MAX) {
                free(token);
                free(cmd_copy);
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }

            cmd->argv[cmd->argc++] = token;

        }
    }

    if (cmd->argc == 0 && cmd->infile == NULL && cmd->outfile == NULL) {
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
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    
    char* tokenized_commands[CMD_MAX] = {NULL};
    char* cmd_copy = strdup(cmd_line);
    char* current_command = strtok(cmd_copy, PIPE_STRING);
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
        current_command = strtok(NULL, PIPE_STRING);

    }

    // fill 'clist' with command buffs

    for (int j = 0; j < i; j++) {

        cmd_buff_t current_buff;
        clear_cmd_buff(&current_buff);
        int build_buff_rc = build_cmd_buff(tokenized_commands[j], &current_buff);

        if (build_buff_rc == OK) {
            clist->commands[j] = current_buff;
        } else {
            return build_buff_rc;
        }
        
    }

    clist->num = i;

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
    command_list_t cmd_list;


    // MAIN LOOP
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

        // parse input to cmd_list
        clean_input(cmd_buff);
        int parse_rc = build_cmd_list(cmd_buff, &cmd_list);

        if (parse_rc == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            continue;
        }

        if (parse_rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        if (parse_rc == ERR_CMD_OR_ARGS_TOO_BIG) {
            printf(CMD_ERR_EXECUTE, "command or args too big");
            continue;
        }

        if (parse_rc == ERR_BAD_REDIRECT) {
            printf(CMD_ERR_REDIRECT);
            continue;
        }

        // if there's only one command, try to execute built-in first
        if (cmd_list.num == 1) {
            cmd_buff_t* current_cmd = &cmd_list.commands[0];
            Built_In_Cmds built_in_rc = exec_built_in_cmd(current_cmd);
            if (built_in_rc == BI_EXECUTED) {
                last_rc = 0;
                continue;
            }
        }

        // process external and piped commands
        int prev_fd = -1;  // holds the read end of the previous pipe
        pid_t pids[CMD_MAX]; // store child PIDs for later

        for (int i = 0; i < cmd_list.num; i++) {

            int pipe_fd[2];
            // create a new pipe for all but the last command
            if (i < cmd_list.num - 1) {
                if (pipe(pipe_fd) < 0) {
                    printf(CMD_ERR_EXECUTE, "pipe failed");
                    break;
                }
            }

            pid_t pid = fork();
            if (pid < 0) {

                printf(CMD_ERR_EXECUTE, "fork failed");

                // close any pipe we created
                if (i < cmd_list.num - 1) {
                    close(pipe_fd[0]);
                    close(pipe_fd[1]);
                }
                break;

            }

            if (pid == 0) {
                // child

                // if not the first command, set STDIN to the previous pipe's read end
                if (prev_fd != -1) {
                    if (dup2(prev_fd, STDIN_FILENO) < 0) {
                        perror("dup2");
                        exit(errno);
                    }
                }

                // if not the last command, set STDOUT to the current pipe's write end
                if (i < cmd_list.num - 1) {
                    if (dup2(pipe_fd[1], STDOUT_FILENO) < 0) {
                        perror("dup2");
                        exit(errno);
                    }
                }

                // close any file descriptors that are no longer needed
                if (prev_fd != -1) {
                    close(prev_fd);
                }
                if (i < cmd_list.num - 1) {
                    close(pipe_fd[0]);
                    close(pipe_fd[1]);
                }

                // redirection if specified
                if (cmd_list.commands[i].outfile != NULL) {

                    int flags = O_WRONLY | O_CREAT;
                    flags |= cmd_list.commands[i].append ? O_APPEND : O_TRUNC;
                    int out_fd = open(cmd_list.commands[i].outfile, flags, 0644);
                    if (out_fd < 0) {
                        exit(errno);
                    }
                    if (dup2(out_fd, STDOUT_FILENO) < 0) {
                        exit(errno);
                    }
                    close(out_fd);

                }

                // check if built-in
                if (match_command(cmd_list.commands[i].argv[0]) != BI_NOT_BI) {
                    Built_In_Cmds built_in_rc = exec_built_in_cmd(&cmd_list.commands[i]);
                    if (built_in_rc == BI_EXECUTED) {
                        exit(0);
                    }
                    exit(-1);
                }

                // execute external the command
                execvp(cmd_list.commands[i].argv[0], cmd_list.commands[i].argv);
                exit(errno);
            } else {
                // parent

                pids[i] = pid;

                // close the previous pipe's read end in the parent since it's no longer needed
                if (prev_fd != -1) {
                    close(prev_fd);
                }

                // if not the last command, prepare for next
                if (i < cmd_list.num - 1) {

                    prev_fd = pipe_fd[0];
                    close(pipe_fd[1]);

                }
            }
        } // for

        // wait for all child processes to exit
        for (int i = 0; i < cmd_list.num; i++) {
            int child_status;
            waitpid(pids[i], &child_status, 0);
            last_rc = WEXITSTATUS(child_status);

            if (last_rc != 0) {
                switch(last_rc) {
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

    } // while

    return OK;

}