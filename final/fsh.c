/* 
 * File:   sh2.c
 * Author: ericfoss
 *
 * Created on December 4, 2014, 11:10 AM
 */

#include <string.h>

#include "type.h"
#include "ucode.h"
#include "printlib.h"

char *cmds[] = {
    "ls", "cd", "pwd", "cat", "cp", "mv", "ps", "mkdir", "rmdir", "creat", "rm",
    "chmod", "more", "grep", "lpr", ">", ">>", "<", "|", "logout", "?"
};
int hist_fd;

void print_help();
int prompt();
void logout();

/**
 * Main sh body
 * @param argc - Argument count
 * @param argv - Argument pointer array
 * @return exit code
 */
int main(int argc, char *argv[]) {
    char user_dir[100];
    signal(2, 1);
    assert(fstat(STDIN, NULL) < 0);
    assert(fstat(STDOUT, NULL) < 0);
    assert(fstat(STDERR, NULL) < 0);
    
//    getcwd(user_dir);
//    strcat(user_dir, ".sh_his");
//    creat(user_dir);
//    hist_fd = open(user_dir, O_RDWR|O_APPEND);
    printf("------------------------------------------------------\n");
    printf("Fosix Shell\n");
    printf("------------------------------------------------------\n");
    printf("Enter ? for a list of available commands.\n");
    printf("------------------------------------------------------\n");
    return(prompt());
}

void logout() {
    printf("Logging out\n");
    exit(EXIT_SUCCESS);
    return;
}

void print_help() {
    int i, total_cmds = sizeof(cmds)/sizeof(char *);
    
    printf("--------------------------------------------------------------------");
    for (i=0; i < total_cmds; i++) {
        if (! (i % 5)) {
            printf("\n-");
        }
        printf(" %s ", cmds[i]);
    }
    printf("\n");
    printf("--------------------------------------------------------------------\n");
    
    return;
}

int prompt() {
    int cid;
    char cwd[80], tty[80], request[255], command[255];
    
    while(1) {
        assert(fstat(STDIN, NULL) < 0);
        assert(fstat(STDOUT, NULL) < 0);
        assert(fstat(STDERR, NULL) < 0);
        getcwd(cwd);
        gettty(tty);
        printf("%s@%s$ ", tty, cwd);
        get_line(STDIN, request, 255);
        
        // If help needed
        if (strncmp(request, "?", 2) == NULL) {
            print_help();
            continue;
        // If logout requested
        } else if (strncmp(request, "logout", 7) == NULL) {
            logout();
        // If empty request
        } else if (strncmp(request, "", 1) == NULL) {
            continue;
        // Else run it
        } else {
            char *tok;
            
            cid = fork();
            if (!cid) {
                strcpy(command, "/bin/");
                
                tok = strtok(request, " ");
                while (tok != NULL) {

                    // Append to file
                    if(strncmp(tok, ">>", 3) == 0) {
                        tok = strtok(NULL, " ");
                        if (tok == NULL) {
                            fprintf(STDERR, "NULL output filename\n");
                            exit(EXIT_FAILURE);
                        }
                        creat(tok);
                        close(STDOUT);
                        if(open(tok, O_WRONLY|O_APPEND) < 0) {
                            fprintf(STDERR, "%s: Could not open for writing\n", tok);
                            exit(EXIT_FAILURE);
                        }
                    // Overwrite file
                    } else if(strncmp(tok, ">", 2) == 0) {
                        tok = strtok(NULL, " ");
                        if (tok == NULL) {
                            fprintf(STDERR, "NULL output filename\n");
                            exit(EXIT_FAILURE);
                        }
                        close(STDOUT);
                        creat(tok);
                        if(open(tok, O_WRONLY) < 0) {
                            fprintf(STDERR, "%s: Could not open for writing\n", tok);
                            exit(EXIT_FAILURE);
                        }
                    // Read file
                    } else if(strncmp(tok, "<", 2) == 0) {
                        tok = strtok(NULL, " ");
                        if (tok == NULL) {
                            fprintf(STDERR, "NULL output filename\n");
                            exit(EXIT_FAILURE);
                        }
                        if(stat(tok, NULL) < 0) {
                            fprintf(STDERR, "Could not open %s for reading\n", tok);
                            exit(EXIT_FAILURE);
                        }
                        close(STDIN);
                        if(open(tok, O_RDONLY) < 0) {
                            fprintf(STDERR, "Could not open %s for reading\n", tok);
                            exit(EXIT_FAILURE);
                        }
                    // Pipe stuff
                    } else if(strncmp(tok, "|", 2) == 0) {
                        int new_pipe[2], pipe_pid;
                        pipe(new_pipe);
                        pipe_pid = fork();
                        if (!pipe_pid) {
                            // restart command string
                            strcpy(command, "/bin/");
                            
                            // Configure new STDIN
                            close(new_pipe[STDOUT]);
                            close(STDIN);
                            dup2(new_pipe[STDIN], STDIN);
                        } else {
                            // Done with command sting
                            tok = NULL;
                            
                            // Configure new STDOUT
                            close(new_pipe[STDIN]);
                            close(STDOUT);
                            dup2(new_pipe[STDOUT], STDOUT);
                            
                            // Break out of loop, we can't tok again
                            continue;
                        }
                    // Append param to current command
                    } else {
                        strcat(command, tok);
                        strcat(command, " ");
                    }

                    tok = strtok(NULL, " ");
                }

//                fprintf(STDOUT, "PID:%d executing %s\n", getpid(), command);
                exec(command);
                fprintf(STDERR, "PID:%d failed to execute\r\n", getpid());
                exit(EXIT_FAILURE);
            } else {
                // Parent waits for child to end
                wait();
            }
        }
    }
    
    return(-1);
}
