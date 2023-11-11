//
//  main.c
//  1
//
//  Created by Лена on 30.10.2023.
//

#include <stdio.h>
#include <stdlib.h>

enum status_codes
{
    fsc_ok,
    fsc_overflow,
    fsc_invalid_parameter,
    fsc_memory_error_detected,
    fsc_unknown,
};

char num_to_char(int n)
{
    if ((n <= 9) && (n >= 0))
        return n + '0';

    return n - 10 + 'a';
}

enum status_codes translation_10_to_x(unsigned int n_10, unsigned int base, char** n_x)
{
    if ((base & 1) || (base < 2) || (base > 32))
        return fsc_invalid_parameter;
    
    if (n_10 == 0)
    {
        *n_x = malloc(2);
        (*n_x)[0] = '0';
        (*n_x)[1] = 0;
        return fsc_ok;
    }

    int pow_2 = 1;
    int mask_2 = 1;
    base >>= 1;
    while((base & 1) == 0)
    {
        ++pow_2;
        mask_2 <<= 1;
        mask_2 |= 1;
        
        base >>= 1;
    }
    if (base != 1)
        return fsc_invalid_parameter;
    
    // base = 2
    // pow_2 = 1
    // mask_2 = 1
    
    // base = 16
    // pow_2 = 4
    // mask_2 = 1111

    // Расчет длины числа
    unsigned int tmp = n_10;
    int len = 0;
    while(tmp > 0)
    {
        ++len;
        tmp >>= pow_2;
    }

    *n_x = malloc(sizeof(char) * (len + 1));
    if (*n_x == NULL)
        return fsc_memory_error_detected;

    char *p = *n_x;
    
    // Добавить 0 в конец строки
    p += len;
    *p = 0;
    --p;
    
    while(n_10 > 0)
    {
        int res = n_10 & mask_2;
        *p = num_to_char(res);
        --p;
        
        n_10 >>= pow_2;
    }

    return fsc_ok;
}

int main(int argc, const char * argv[])
{
    enum status_codes function_result = fsc_unknown;
    
    char* str = NULL;
    function_result = translation_10_to_x(5, 2, &str);
    
    if (function_result == fsc_ok)
    {
        printf("%s\n", str);
        free(str);
    }
    

    switch (function_result)
    {
        case fsc_ok:
            break;
        case fsc_overflow:
            printf("Overflow detected\n");
            break;
        case fsc_invalid_parameter:
            printf("Invalid parameter detected\n");
            break;
        case fsc_memory_error_detected:
            printf("Memory error detected\n");
            break;
        default:
            printf("function_result is unknown\n");
    }

    return function_result == fsc_ok ? 0 : 1;
}
