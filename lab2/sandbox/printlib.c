#include <assert.h>
#include <stdarg.h>
#include "types.c"

#define putchar(c) (putc(c))

char *gets(char buf[]);
void print_num(uint32_t i, uint8_t base);
uint8_t char_to_base(char c);
uint32_t s_to_unsigned(int32_t i);
void printf(char *string, ...);

char *digits = "0123456789ABCDEF";

char *gets(char buf[]) {
    char c, *ptr = buf;

    while ((c = getc()) != '\r') {
        *ptr++ = c;
    }

    *ptr = '\0';
    return(buf);
}

void print_num(uint32_t i, uint8_t base) {
    if (i / base) {
        print_num((i / base), base);
    }
    putchar(digits[i % base]);
}

uint8_t char_to_base(char c) {
    switch(c) {
        case 'd':
        case 'l':
        case 'u':
            return(10);
        case 'x':
            return(16);
        case 'b':
            return(2);
        default:
            break;
    }
    printf("Illegal char_to_base >>%c<<", c);
    while(1) { getc(); }
}

uint32_t s_to_unsigned(int32_t i) {
    if (i < 0) {
        putchar('-');
        i *= -1;
    }
    return(i);
}

void printf(char *string, ...) {
    char buf[100], *str_ptr, *base = string;
    int32_t i;
    
    va_list args;
    va_start(args, string);
    
    while (*string != '\0') {
        if (*string == '%') {
            switch(*++string) {
                case 'c':
                    putchar(va_arg(args, int));
                    break;

                case 's':
                    str_ptr = (char *)va_arg(args, char *);
                    while(*str_ptr != '\0') {
                        if (*str_ptr == '\n') {
                            putchar('\r');
                        }
                        putchar(*str_ptr++);
                    }
                    break;

                case 'd':
                    i = (int16_t)va_arg(args, int);
                    i = (int16_t)s_to_unsigned((int32_t)i);
                    print_num((uint32_t)i, char_to_base(*string));
                    break;

                case 'u':
                case 'x':
                case 'b':
                    i = (int16_t)va_arg(args, int);
                    print_num((uint32_t)i, char_to_base(*string));
                    break;

                case 'l':
                    i = (int32_t)va_arg(args, long);
                    switch(*++string) {
                        case 'd':
                            i = s_to_unsigned(i);
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
                    putchar('%');
                    break;

                default:
                    printf("Malformed printf >>%s<<", base);
                    while(1) { getc(); }
                    break;
            }
        } else {
            if (*string == '\n') {
               putchar('\r');
            }
            putchar(*string);
        }
        
        string++;
    }
    
    va_end(args);
    return;
}
