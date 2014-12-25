/* 
 * File:   more.c
 * Author: ericfoss
 *
 * Created on December 8, 2014, 8:19 AM
 */

#include "my_assert.h"
#include "printlib.h"
#include "type.h"

#define ROWS    24
#define COLS    70

int fd, in;

void quit(int code) {
    close(fd);
    close(in);
    exit(code);
}

/*
 * 
 */
int main(int argc, char** argv) {
    int i, eof, is_chr;
    char buf[79];
    STAT out_stat;
    assert(fstat(STDIN, NULL) < 0);
    assert(fstat(STDOUT, NULL) < 0);
    assert(fstat(STDERR, NULL) < 0);
    
//    fprintf(STDERR, "***************************************************************\n");
//    fprintf(STDERR, "* Fosix: more                                                 *\n");
//    fprintf(STDERR, "***************************************************************\n");
    
    switch (argc) {
        case 1:
//            printf("Using STDIN\n");
            fd = dup(STDIN);
            close(STDIN);
            in = open(gettty(), O_RDONLY);
            dup(in, STDIN);
            break;
        case 2:
//            printf("Using %s\n", argv[1]);
            fd = open(argv[1], O_RDONLY);
            break;
        default:
            printf("Usage: more [filename]\n");
            exit(EXIT_FAILURE);
            break;
    }
    
    assert(fstat(STDIN, NULL) < 0);
    if (fd == -1) {
        fprintf(STDERR, "Could not open input for reading\n");
        quit(EXIT_FAILURE);
    }
    fstat(STDOUT, &out_stat);
    is_chr = out_stat.st_mode & IS_CHR;
    
    i = 1;
    while(1) {
        get_line(fd, buf, COLS, &eof);
        printf("%s\n", buf);
        if (eof) {
            break;
        }
        if (i++ >= ROWS) {
            if (getc() == 'q') {
                break;
            }
        }
    }
    
    quit(EXIT_SUCCESS);
}
