//
//  main.c
//  4
//
//  Created by Лена on 04.11.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

enum status_codes
{
    fsc_ok,
    fsc_overflow,
    fsc_invalid_parameter,
    fsc_unknown,
    fsc_memory_error_detected,
    fsc_file_is_not_found,
};

typedef struct
{
    char* str;
    int len;
}string;

enum status_codes create(string** new_string)
{
    *new_string = malloc(sizeof(string));
    if (new_string == NULL)
        return fsc_memory_error_detected;
    
    (*new_string)->str = NULL;
    (*new_string)->len = 0;
    
    return fsc_ok;
}

enum status_codes create_from_char(string** new_string,const char* str)
{
    enum status_codes function_result = create(new_string);
    
    if (function_result != fsc_ok)
        return function_result;
    
    (*new_string)->str = malloc(sizeof(char) * (strlen(str) + 1));
    if (*new_string == NULL)
        return fsc_memory_error_detected;
    
    strcpy((*new_string)->str, str);
    
    (*new_string)->len = (int)strlen(str);
    
    return fsc_ok;
}

//копирование с созданием нового экземпляра
enum status_codes create_from_string(string** destination, string const * const source)
{
    return create_from_char(destination, source->str);
}

//копирование без создания нового экземпляра
enum status_codes copy(string* destination, string const * const source)
{
    if (destination->len != source->len)
    {
        destination->str = realloc(destination->str, sizeof(char) * (source->len + 1));
        
        if (destination == NULL)
            return fsc_memory_error_detected;
    }
    
    strcpy(destination->str, source->str);
    destination->len = source->len;
    
    return fsc_ok;
}

void clear_string(string** current_string)
{
    free ((*current_string)->str);
    free(*current_string);
}

int comp (const string* a, const string* b)
{
    if (a->len != b->len)
        return (a->len > b->len) ? 1 : -1;

    int cmp = strcmp(a->str, b->str);
    return (cmp > 0) ? 1 : -1;
}

bool is_equivalents (const string* a, const string* b)
{
    int cmp = strcmp(a->str, b->str);
    
    return (cmp == 0) ? true : false;
}

enum status_codes cat_string(string* destptr, const string* srcptr)
{
    destptr->str = realloc(destptr->str, sizeof(char) * (destptr->len + srcptr->len + 1));
    
    if (destptr->str == NULL)
        return fsc_memory_error_detected;
    
    strcat(destptr->str, srcptr->str);
    destptr->len = destptr->len + srcptr->len;
    
    return fsc_ok;
}

int main(int argc, const char * argv[])
{
    enum status_codes function_result = fsc_unknown;
    
    //Создание первого string
    string* a;
    function_result = create_from_char(&a, "123456");
    if (function_result != fsc_ok)
        goto finish_1;
    
    printf("a string: %s %d\n", a->str, a->len);
    
    //Создание второго string
    string* b;
    function_result = create_from_char(&b, "78");
    if (function_result != fsc_ok)
        goto finish_2;
    
    printf("b string:%s %d\n", b->str, b->len);
    
    //Отношение порядка
    printf("a %s b\n", (comp(a, b) == 1) ? "bigger" : "less");
    
    //Отношение эквалентности
    printf("%s %s %s\n", "is", is_equivalents(a, b) ? "" : "not", "equivalents");
    
    //Копирование в существующий экземпляр
    string* c = malloc(sizeof(string));
    if (c == NULL)
        goto finish_3;
    
    c->str = malloc(sizeof(char) * (b->len + 1));
    if (c->str == NULL)
        goto finish_4;
    
    function_result = copy(c, b);
    if (function_result != fsc_ok)
        goto finish_4;
    
    printf("Copy of b: %s %d\n", c->str, c->len);
    
    //Копирование в несуществующий экземпляр
    string* d = NULL;
    function_result = create_from_string(&d, b);
    if (function_result != fsc_ok)
        goto finish_4;
    
    printf("Copy of b: %s %d\n", d->str, d->len);
    
    //Конкатенация
    function_result = cat_string(d, b);
    if (function_result != fsc_ok)
        goto finish_5;
    
    printf("d + b = %s %d\n", d->str, d->len);
    
finish_5:
    clear_string(&d);
    
finish_4:
    clear_string(&c);
    
finish_3:
    clear_string(&b);
    
finish_2:
    clear_string(&a);
    
finish_1:
    
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
        case fsc_file_is_not_found:
            printf("File is not found\n");
            break;
        default:
            printf("function_result is unknown\n");
    }
    
    return function_result == fsc_ok ? 0 : 1;
}
