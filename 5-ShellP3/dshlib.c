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