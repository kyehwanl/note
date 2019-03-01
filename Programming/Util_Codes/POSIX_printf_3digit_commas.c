

< method 1 >

/* POSIX printf, try 3 digit commas */

#include <locale.h>
setlocale(LC_ALL, ""); /* use user selected locale */
printf("%'d", 1000000);







< method 2 >

// format 1234567.89 -> 1 234 567.89
extern char *strmoney(double value){
    static char result[64];
    char *result_p = result;
    char separator = ' ';
    size_t tail;

    snprintf(result, sizeof(result), "%.2f", value);

    while(*result_p != 0 && *result_p != '.')
        result_p++;

    tail = result + sizeof(result) - result_p;

    while(result_p - result > 3){
        result_p -= 3;
        memmove(result_p + 1, result_p, tail);
        *result_p = separator;
        tail += 4;
    }

    return result;
}
