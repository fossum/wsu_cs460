/* 
 * File:   cat.c
 * Author: ericfoss
 *
 * Created on December 8, 2014, 10:54 AM
 */

#include "my_assert.h"
#include "printlib.h"
#include "type.h"

#define COLS    70

int fd;

void quit(int code) {
    close(fd);
    exit(code);
}

/*
 * 
 */
int main(int argc, char** argv) {
    int eof;
    char buf[COLS];
    assert(fstat(STDIN, NULL) < 0);
    assert(fstat(STDOUT, NULL) < 0);
    assert(fstat(STDERR, NULL) < 0);
    
//    fprintf(STDERR, "*******************************************************\n");
    switch (argc) {
        case 1:
//            fprintf(STDERR, "* Fosix: cat(STDIN)                                   *\n");
            fd = dup(STDIN);
            break;
        case 2:
//            fprintf(STDERR, "* Fosix: cat(%s)                              *\n", argv[1]);
            fd = open(argv[1], O_RDONLY);
            break;
        default:
            fprintf(STDERR, "* Fosix: cat                                          *\n");
            fprintf(STDERR, "*    Usage: cat [filename]                            *\n");
            exit(EXIT_FAILURE);
            break;
    }
//    fprintf(STDERR, "*******************************************************\n");
    
    if (fd == -1) {
        fprintf(STDERR, "Could not open '%s' for reading\n", argv[1]);
        quit(EXIT_FAILURE);
    }
    
    while(1) {
        get_line(fd, buf, COLS, &eof);
        printf("%s\n", buf);
        if (eof) {
//            printf("breakc\0");
            break;
        }
    }
    
    quit(EXIT_SUCCESS);
}
