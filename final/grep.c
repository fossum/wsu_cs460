/* 
 * File:   grep.c
 * Author: ericfoss
 *
 * Created on December 8, 2014, 11:13 AM
 */

#include "my_assert.h"
#include "printlib.h"
#include "type.h"

#define FIND_ARG    1
#define FILE_ARG    2

int fd;

void quit(int code) {
    close(fd);
    close(STDIN);
    close(STDOUT);
    close(STDERR);
    exit(code);
}

int containsString(char *buf, char *thing) {
    int i = strlen(buf)-strlen(thing);
    
    while(i >= 0) {
        if (strncmp(buf + i, thing, strlen(thing)) == 0) {
            return(1);
        }
        i--;
    }
    
    return(0);
}

/*
 * 
 */
int main(int argc, char** argv) {
    int eof, is_chr, count, line;
    char buf[1024];
    STAT out_stat;
    assert(fstat(STDIN, NULL) < 0);
    assert(fstat(STDOUT, NULL) < 0);
    assert(fstat(STDERR, NULL) < 0);
    
//    fprintf(STDERR, "***************************************************************\n");
//    fprintf(STDERR, "* Fosix: grep                                                 *\n");
    switch (argc) {
        case 2:
//            fprintf(STDERR, "* Finding %s using STDIN\n", argv[FIND_ARG]);
            fd = dup(STDIN);
            break;
        case 3:
//            fprintf(STDERR, "* Finding %s using %s\n", argv[FIND_ARG], argv[FILE_ARG]);
            fd = open(argv[FILE_ARG], O_RDONLY);
            break;
        default:
            fprintf(STDERR, "* Usage: grep pattern [filename]\n");
            exit(EXIT_FAILURE);
            break;
    }
//    fprintf(STDERR, "***************************************************************\n");
    
    if (fd == -1) {
        printf("Could not open input for reading\n");
        quit(EXIT_FAILURE);
    }
    fstat(STDOUT, &out_stat);
    is_chr = out_stat.st_mode & IS_CHR;
    
    count = 0; line = 0;
    while(1) {
        get_line(fd, buf, sizeof(buf), &eof);
        line++;
        if (containsString(buf, argv[FIND_ARG])) {
            count++;
            printf("Line %d: %s\n", line, buf);
        }
        if (eof) {
            printf("Found %d lines containing '%s'\n", count, argv[FIND_ARG]);
            break;
        }
    }
    
    quit(EXIT_SUCCESS);
}
