/* 
 * File:   l2u.c
 * Author: ericfoss
 *
 * Created on December 8, 2014, 1:30 PM
 */

#include "my_assert.h"
#include "printlib.h"
#include "type.h"

#define COLS 70

int in_fd, out_fd;

void quit(int code) {
    close(in_fd);
    close(out_fd);
    exit(code);
}

void convertToUppercase(char *buf) {
    char *ptr = buf;
    while(*ptr != '\0') {
        if (*ptr >= 97 && *ptr <= 122) {
            *ptr -= 32;
        }
        ptr++;
    }
    return;
}

/*
 * 
 */
int main(int argc, char** argv) {
    int is_chr;
    char c;
    STAT out_stat;
    
    fprintf(STDERR, "******************************************************\n");
    fprintf(STDERR, "* Lowercase to Uppercase\n");
    fprintf(STDERR, "******************************************************\n");
    
    switch (argc) {
        case 1:
//            printf("Using STDIN to STDOUT\n");
            in_fd = dup(STDIN);
            out_fd = dup(STDOUT);
            break;
        case 2:
//            printf("Using %s to STDOUT\n", argv[1]);
            in_fd = open(argv[1], O_RDONLY);
            out_fd = dup(STDOUT);
            break;
        case 3:
//            printf("Using %s to %s\n", argv[1], argv[2]);
            in_fd = open(argv[1], O_RDONLY);
            if (in_fd == -1) {
                fprintf(STDERR, "Could not open input for reading\n");
                quit(EXIT_FAILURE);
            }
            if (stat(argv[2], NULL) < 0) {
                unlink(argv[2]);
            }
            creat(argv[2]);
            out_fd = open(argv[2], O_WRONLY);
            break;
        default:
            printf("Usage: l2u [infile [outfile]]\n");
            exit(EXIT_FAILURE);
            break;
    }
    
    if (in_fd == -1) {
        fprintf(STDERR, "Could not open input for reading\n");
        quit(EXIT_FAILURE);
    }
    if (out_fd == -1) {
        fprintf(STDERR, "Could not open output for writing\n");
        quit(EXIT_FAILURE);
    }
    fstat(out_fd, &out_stat);
    is_chr = out_stat.st_mode & IS_CHR;
//    fprintf(STDERR, "CHR: %d\n", is_chr);
    
    while(read(in_fd, &c, 1)) {
        if (c >= 97 && c <= 122) {
            c -= 32;
        }
        if (is_chr && (c == '\n')) {
            fprintf(out_fd, "%c", '\r');
        }
        fprintf(out_fd, "%c", c);
    }
    
    quit(EXIT_SUCCESS);
}
