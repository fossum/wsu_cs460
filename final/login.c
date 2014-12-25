/* 
 * File:   login.c
 * Author: ericfoss
 *
 * Created on December 2, 2014, 7:20 PM
 */

#include <string.h>

#include "printlib.h"
#include "ucode.h"

int stdin, stdout, stderr;
int gid, uid;
char user_name[10], groups[40], user_dir[40], shell[10], tty[20];

void error_report(char *str) {
    stdout = open("/dev/tty0", O_WRONLY);
    printf("%s\n", str);
    exit(EXIT_FAILURE);
    return;
}

void prompt_creds(char *user, char *pass) {
    //flush_stdin();
    printf("--------------------------------------------------\n");
    printf("- FOSIX : terminal\n");
    printf("--------------------------------------------------\n");
    printf("Enter username: ");
    close(STDIN);
    open(tty, O_RDONLY);
    assert(fstat(STDIN, NULL) < 0);
    get_line(STDIN, user);
    printf("Password: ");
    getsecret(pass);
//    printf("u: '%s', p: '%s'\n", user, pass);
}

int authenticate(char *user, char *pass) {
    int file;
    char buf[100];
    
    // Open passwd file
    file = open("/etc/passwd", O_RDONLY);
    if (file == -1 || fstat(file, NULL) < 0) {
        error_report("Cannot open passwd file");
    }
    
    // Check each line
    while(get_line(file, buf)) {
        char *ptr;
        
        // Compare user name
        ptr = strtok(buf, ":");
//        printf("Username: %s\n", ptr);
        if (strncmp(user, ptr, 100) != 0 ) {
            continue;
        }
        strcpy(user_name, ptr);
        
        // Compare password
        ptr = strtok(0, ":");
        if (strncmp(pass, ptr, 100) != 0 ) {
            continue;
        }
        // Save user info
        if(ptr = strtok(0, ":")) {
            uid = atoi(ptr);
            if(ptr = strtok(0, ":")) {
                gid = atoi(ptr);
                chuid(uid, gid);
                if(ptr = strtok(0, ":")) {
                    strcpy(groups, ptr);
                    if(ptr = strtok(0, ":")) {
                        strcpy(user_dir, ptr);
                        chdir(user_dir);
                        if(ptr = strtok(0, ":")) {
                            strcpy(shell, "/bin/");
                            strcat(shell, ptr);
                        }
                    }
                }
            }
        }
        return(1);
    }
    
    return(0);
}

int main(int argc, char *argv[]) {
    char user[30], pass[30];
    
    // Close original IO
    close(STDIN);
    close(STDOUT);
    close(STDERR);
    
    // Verify arg count
    if (argc != 2) {
        error_report("Invalid arg. count");
        return(-1);
    }
    
    // open output
    strcpy(tty, argv[1]);
    stdin = open(tty, O_RDONLY);
    stdout = open(tty, O_WRONLY);
    stderr = open(tty, O_WRONLY);
    assert(fstat(STDIN, NULL) < 0);
    assert(fstat(STDOUT, NULL) < 0);
    assert(fstat(STDERR, NULL) < 0);
    
    // Verify output
    if (stdin == -1 || stdout == -1 || stderr == -1) {
        error_report("Cannot open terminal!");
    }
    
    // Set tty
    settty(tty);
    printf("\nFOSIX Login : opened %s as stdin, stdout, stderr\n", tty);
    
    // ignore Control-C interrupts so that 
    // Control-C KILLs other procs on this 
    // tty but not the main sh
    signal(2, 1);
    
    while (1) {
        prompt_creds(user, pass);
        if (authenticate(user, pass)) {
            break;
        }
        printf("Authentication failed, try again\n");
    }
    
    // User logged in, executing shell
    exec(shell);
    return(0);
}
