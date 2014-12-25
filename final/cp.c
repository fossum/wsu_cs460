/* 
 * File:   cp.c
 * Author: ericfoss
 *
 * Created on December 8, 2014, 3:27 PM
 */

#include "my_assert.h"
#include "printlib.h"
#include "type.h"

int in_fd, out_fd;
STAT in_st, out_st;

void quit(int code) {
    close(in_fd);
    close(out_fd);
    exit(code);
}

char *basename(char *path) {
    char *ptr = path+strlen(path);
    while(path != ptr && *--ptr != '/');
//    printf("%s\n", ptr);
    return(*ptr == '/' ? ++ptr : ptr);
}

void open_input(char *path) {
    in_fd = open(path, O_RDONLY);
    stat(path, &in_st);
    if (in_fd == -1) {
        printf("Could not open input for reading\n");
        quit(EXIT_FAILURE);
    }
}

void open_output(char *path) {
    stat(path, &out_st);
    if (out_st.st_ino == in_st.st_ino) {
        printf("Cannot copy file into itself\n");
        quit(EXIT_FAILURE);
    }
    creat(path);
    out_fd = open(path, O_WRONLY);
    if (out_fd == -1) {
        printf("Could not open output for writing\n");
        quit(EXIT_FAILURE);
    }
}

/*
 * 
 */
int main(int argc, char** argv) {
    char c;
    
//    fprintf(STDERR, "***************************************************************\n");
//    fprintf(STDERR, "* Fosix: cp                                                   *\n");
//    fprintf(STDERR, "***************************************************************\n");
    
    switch (argc) {
        case 2:
//            printf("Using %s\n", argv[1]);
            open_input(argv[1]);
            open_output(basename(argv[1]));
            break;
        case 3:
//            printf("Using %s\n", argv[1]);
            open_input(argv[1]);
            open_output(argv[2]);
            break;
        default:
            printf("Usage: cp filename [filename]\n");
            exit(EXIT_FAILURE);
            break;
    }
    
    if (out_fd == -1) {
        printf("Could not open output for writing\n");
        quit(EXIT_FAILURE);
    }
    
    while(read(in_fd, &c, 1)) {
        fprintf(out_fd, "%c", c);
    }
    
    quit(EXIT_SUCCESS);
}
