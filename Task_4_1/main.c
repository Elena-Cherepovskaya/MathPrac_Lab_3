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
    
    int len = (int)strlen(str);
    
    (*new_string)->str = malloc(sizeof(char) * (len + 1));
    if ((*new_string)->str == NULL)
    {
        free(*new_string);
        *new_string = NULL;
        return fsc_memory_error_detected;
    }
    
    strcpy((*new_string)->str, str);
    
    (*new_string)->len = len;
    
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
        char* tmp_p = realloc(destination->str, sizeof(char) * (source->len + 1));
        
        if (tmp_p == NULL)
        {
            free(destination->str);
            destination->str = NULL;
            destination->len = 0;
            return fsc_memory_error_detected;
        }
        
        destination->len = source->len;
        destination->str = tmp_p;
        
    }
    
    strcpy(destination->str, source->str);
    
    return fsc_ok;
}

void clear_string(string** current_string)
{
    //if (*current_string == NULL)
    //   return;
    
    free ((*current_string)->str);
    (*current_string)->str = NULL;
    (*current_string)->len = 0;
    free(*current_string);
    (*current_string) = NULL;
}

int comp (const string* a, const string* b)
{
    if (a->len != b->len)
        return (a->len > b->len) ? 1 : -1;

    return strcmp(a->str, b->str);
}

bool is_equivalents (const string* a, const string* b)
{
    return (strcmp(a->str, b->str) == 0);
}

enum status_codes cat_string(string* destptr, const string* srcptr)
{
    int new_len = destptr->len + srcptr->len;
    char* tmp_p = realloc(destptr->str, sizeof(char) * (new_len + 1));

    if (tmp_p == NULL)
    {
        free(destptr->str);
        destptr->len = 0;
        return fsc_memory_error_detected;
    }
    
    destptr->str = tmp_p;
    
    strcat(destptr->str, srcptr->str);
    destptr->len = new_len;
    
    return fsc_ok;
}

int main(int argc, const char * argv[])
{
    enum status_codes function_result = fsc_unknown;
    
    //Создание первого string
    string* a = NULL;
    function_result = create_from_char(&a, "123456");
    
    if (function_result == fsc_ok)
        printf("a string:'%s' len=%d\n", a->str, a->len);
    
    string* b = NULL;
    if (function_result == fsc_ok)
    {
        //Создание второго string
        function_result = create_from_char(&b, "78");
        
        if (function_result == fsc_ok)
        {
            printf("b string:'%s' len=%d\n", b->str, b->len);
            
            //Отношение порядка
            printf("a %s b\n", (comp(a, b) == 1) ? "bigger" : "less");
            
            //Отношение эквалентности
            printf("%s %s %s\n", "is", is_equivalents(a, b) ? "" : "not", "equivalents");
        }
    }
    
    
    string* c = NULL;
    if (function_result == fsc_ok)
    {
        //Копирование в существующий экземпляр
        function_result = create(&c);
    }
    
    if (function_result == fsc_ok)
    {
        function_result = copy(c, b);
        if (function_result == fsc_ok)
            printf("c is copy of b:'%s' len=%d\n", c->str, c->len);
    }
                        
    //Копирование в несуществующий экземпляр
    string* d = NULL;
    if (function_result == fsc_ok)
    {
        function_result = create_from_string(&d, b);
        if (function_result == fsc_ok)
            printf("d is create from b: %s %d\n", d->str, d->len);
    }

    if (function_result == fsc_ok)
    {
        function_result = cat_string(d, b);
        if (function_result == fsc_ok)
            printf("d + b = %s %d\n", d->str, d->len);
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
        case fsc_file_is_not_found:
            printf("File is not found\n");
            break;
        default:
            printf("function_result is unknown\n");
    }

    if (d != NULL)
        clear_string(&d);
    
    if (c != NULL)
        clear_string(&c);
    
    if (d != NULL)
        clear_string(&d);
    
    if (a != NULL)
        clear_string(&a);
    
    return function_result == fsc_ok ? 0 : 1;
}
