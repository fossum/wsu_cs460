
#include "../include/printlib.h"

char *digits = "0123456789ABCDEF";  // number to char array
char *tok_str = 0;                  // tokenize string holder

/**
 * Gets string from STDIN.
 * @param Buffer to store string.
 * @return Pointer to string.
 */
char *gets(char *buf) {
    int i = 0, done = 0;
    char c;
    
    while (!done) {
        c = getc();
        switch (c) {
            case('\r'):
                printf("\r\n");
                *(buf + i) = '\0';
                done = 1;
                break;
            case('\b'):
                if (i > 0) {
                    printf("\b \b");
                    *(buf + i) = '\0';
                    i--;
                }
                i--;
                break;
            default:
                putc(c);
                *(buf + i) = c;
                break;
        }
        i++;
        //printf(" idx: %d\n", i);
    }
    return(buf);
}

/**
 * Print unsigned number, base 2-16.
 * @param uint32 number
 * @param Number base
 */
void print_num(uint32_t i, uint8_t base) {
    if (i / base) {
        print_num((i / base), base);
    }
    putchar(digits[i % base]);
}

/**
 * Convert printf format char to number base.
 * @param Format character
 * @return Number base
 */
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

/**
 * Convert signed integer to unsigned and print
 * '-' character if negative.
 * @param int32 number
 * @return uint32 number
 */
uint32_t s_to_unsigned(int32_t i) {
    if (i < 0) {
        putchar('-');
        i *= -1;
    }
    return(i);
}

/**
 * Prints string with integrated variables using
 * format characters. Supports:
 * strings:
 *      c, s
 * 16-bit numbers:
 *      d, u, b, x
 * 32-bit numbers:
 *      ld, lu, lb, lx
 * @param format string
 * @param Any number of variables (must match number of vars in format)
 */
void printf(char *string, ...) {
    char buf[100], *str_ptr, *base = string;
    int32_t i;
    uint32_t ui;
    
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
                    i = va_arg(args, int);
                    ui = s_to_unsigned(i);
                    print_num(ui, char_to_base(*string));
                    break;
                    
                case 'u':
                case 'x':
                case 'b':
                    ui = va_arg(args, unsigned int);
                    print_num(ui, char_to_base(*string));
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

/**
 * Finds next char after a seperator character in
 * string. Replaces seperator characters with '\0'
 * so it acts as a string.
 * @param Seperator character
 * @param Beginning of string (if NULL, uses last string)
 * @return Pointer to beginning of next string
 */
char *tokenize(char separator, char *str) {
    char *ptr = str;
    if (ptr == 0) {
        ptr = tok_str;
    }
    
    // Do we have what we need?
    if (separator == 0 || ptr == 0 || *ptr == '\0') {
        return(0);
    }
    
    // Find next separation and divide strings
    while (*ptr != '\0' && *ptr != separator) { ptr++; }
    while (*ptr != '\0' && *ptr == separator) {
        *ptr++ = '\0';
    }
    
    // Return next string location
    return(ptr);
}

/**
 * Copies one string into another. Warning: not
 * safe if you cannot guarantee '\0' terminator.
 * @param Destination pointer
 * @param Source pointer
 * @return Number of characters copied
 */
int strcpy(char *dest, char *src) {
    int i = 0;
    while (*src != '\0') {
        *dest++ = *src++;
        i++;
    }
    *dest = '\0';
    return(i);
}

/**
 * Same as strcpy, but safer due to defined max
 * length.
 * @param Destination pointer
 * @param Source pointer
 * @param Maximum length
 * @return Number of characters copied (-1 for overflow)
 */
int strncpy(char *dest, char *src, int length) {
    int i = 0;
    while (*src != '\0' && i <= length) {
        *dest++ = *src++;
        i++;
    }
    if (i <= length) { *dest = '\0'; }
    return(i > length ? -1 : i);
}
