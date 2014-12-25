
#include "printlib.h"
#include "my_assert.h"

char *digits = "0123456789ABCDEF"; // number to char array
char *tok_str = 0; // tokenize string holder
//SEMAPHORE print_mu;

void writechar(int fd, char c) {
    assert(fstat(fd, NULL) < 0);
    
//    mutex_lock(&print_mu);
    write(fd, &c, 1);
//    mutex_unlock(&print_mu);
}

/**
 * Print unsigned number, base 2-16.
 * @param uint32 number
 * @param Number base
 */
void print_num(int fd, uint32_t i, uint8_t base) {
    if (i / base) {
        print_num(fd, (i / base), base);
    }
    putchar(fd, digits[i % base]);
}

/**
 * Convert printf format char to number base.
 * @param Format character
 * @return Number base
 */
uint8_t char_to_base(char c) {
    switch (c) {
        case 'd':
        case 'l':
        case 'u':
            return (10);
        case 'x':
            return (16);
        case 'b':
            return (2);
        default:
            break;
    }
    fprintf(STDERR, "Illegal char_to_base >>%c<<", c);
    assert(1);
}

/**
 * Convert signed integer to unsigned and print
 * '-' character if negative.
 * @param int32 number
 * @return uint32 number
 */
uint32_t s_to_unsigned(int fd, int32_t i) {
    if (i < 0) {
        putchar(fd, '-');
        i *= -1;
    }
    return (i);
}

void fprintf(int fd, char *str, ...) {
    va_list args;
//    int pri = getpri();
    
//    setpri(7);
    va_start(args, str);
    my_print(fd, str, args);
    va_end(args);
//    setpri(pri);
    
    return;
}

void printf(char *str, ...) {
    va_list args;
//    int pri = getpri();
    
//    setpri(7);
    va_start(args, str);
    my_print(STDOUT, str, args);
    va_end(args);
//    setpri(pri);
    
    return;
}

/**
 * Prints string with integrated variables using
 * format characters.
 * 
 * Supports:
 *  - strings:
 *      c, s
 *  - 16-bit numbers:
 *      d, u, b, x
 *  - 32-bit numbers:
 *      ld, lu, lb, lx
 * @param File descriptor
 * @param format string
 * @param va_list matching format string
 */
void my_print(int fd, char *string, va_list args) {
    char *str_ptr, *base = string;
    uint32_t ui, is_chr;
    int32_t i;
    STAT stat;
    
    if (fstat(fd, &stat) < 0) {
        fprintf(STDERR, "Bad file handle: %d\n", fd);
        assert(1);
    }
    is_chr = stat.st_mode & IS_CHR;

    while (*string != '\0') {
        if (*string == '%') {
            switch (*++string) {
                case 'c':
                    putchar(fd, va_arg(args, int));
                    break;

                case 's':
                    str_ptr = (char *) va_arg(args, char *);
                    while (*str_ptr != '\0') {
                        if (*str_ptr == '\n' && is_chr) {
                            putchar(fd, '\r');
                        }
                        putchar(fd, *str_ptr++);
                    }
                    break;

                case 'd':
                    i = va_arg(args, int);
                    ui = s_to_unsigned(fd, i);
                    print_num(fd, ui, char_to_base(*string));
                    break;

                case 'u':
                case 'x':
                case 'b':
                    ui = va_arg(args, unsigned int);
                    print_num(fd, ui, char_to_base(*string));
                    break;

                case 'l':
                    i = (int32_t) va_arg(args, long);
                    switch (*++string) {
                        case 'd':
                            i = s_to_unsigned(fd, i);
                        case 'u':
                        case 'b':
                        case 'x':
                            break;
                        default:
                            // Assert?
                            break;
                    }
                    print_num(i, char_to_base(*string));
                    break;

                case '%':
                    putchar(fd, '%');
                    break;

                default:
                    fprintf(STDERR, "Malformed printf >>%s<<", base);
                    assert(1);
                    break;
            }
        } else {
            if (*string == '\n' && is_chr) {
                putchar(fd, '\r');
            }
            putchar(fd, *string);
        }

        string++;
    }

    return;
}
