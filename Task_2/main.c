//
//  main.c
//  2
//
//  Created by Лена on 27.11.2023.
//

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>

#define START_CAPASITY 32
#define EPS 0.0001

enum status_codes
{
    fsc_ok,
    fsc_overflow,
    fsc_invalid_parameter,
    fsc_memory_error_detected,
    fsc_file_is_not_found,
    fsc_file_is_empty,
};

typedef struct
{
    int n;
    double* coordinates;
}T_vector;

enum status_codes create_T_vector(T_vector** new_vector, int n, ...)
{
    (*new_vector) = malloc(sizeof(T_vector));
    if (*new_vector == NULL)
        return fsc_memory_error_detected;
    (*new_vector)->coordinates = malloc(sizeof(double) * n);
    
    va_list x;
    va_start(x, n);
    for (int i = 0; i < n; ++i)
    {
        ((*new_vector)->coordinates)[i] = va_arg(x, double);
    }
    (*new_vector)->n = n;
    
    return fsc_ok;
}

void clear_T_vector(T_vector** vector)
{
    free((*vector)->coordinates);
    *vector = NULL;
}

enum status_codes norm_inf(T_vector * v, double * res, void *_)
{
    int n = v->n;
    double* x = v->coordinates;
    *res = 0.0;
    for (int i = 0; i < n; ++i)
    {
        double new_x = x[i];
        if (fabs(new_x) > fabs(*res))
            *res = new_x;
    }
    
    return fsc_ok;
}

//int p
enum status_codes norm_p(T_vector * v, double * res,  int * p_param)
{
    int p = *p_param;
    
    int n = v->n;
    double* x = v->coordinates;
    (*res) = 0.0;
    
    for (int i = 0; i < n; ++i)
        (*res) = (*res) + pow(fabs(x[i]), p);
    
    (*res) = pow((*res), 1.0 / p);
    return fsc_ok;
}

//double* A
enum status_codes norm_A(T_vector * v, double * res, double * A)
{
    int n = v->n;
    double* x = v->coordinates;
    
    //Перемножение матрицы и вектора
    double* new_x = malloc(sizeof(double) * n);
    if (new_x == NULL)
        return fsc_memory_error_detected;
    
    for (int i = 0; i < n; ++i)
    {
        double new_x_i = 0.0;
        for (int j = 0; j < n; ++j)
        {
            new_x_i += (A[i * n + j] * x[j]);
        }
        new_x[i] = new_x_i;
    }
    
    //Скалярное произведение векторов
    *res = 0.0;
    for (int i = 0; i < n; ++i)
        (*res) += (new_x[i] * x[i]);
    
    (*res) = pow((*res), 0.5);
    return fsc_ok;
}

typedef struct
{
    T_vector** max_vectors;
    int capasity;
    int len;
}T_vector_list;

enum status_codes create_vector_list(T_vector_list** vector_list)
{
    (*vector_list) = malloc(sizeof(T_vector_list));
    if (*vector_list == NULL)
        return fsc_memory_error_detected;
    
    (*vector_list)->capasity = START_CAPASITY;
    (*vector_list)->max_vectors = malloc(sizeof(T_vector*) * (*vector_list)->capasity);
    (*vector_list)->len = 1;
    return fsc_ok;
}

void clear_vector_list(T_vector_list** vector_list)
{
    free((*vector_list));
    *vector_list = NULL;
}

void print_vector_list(T_vector_list* vector_list)
{
    int n = (vector_list->max_vectors)[0]->n;
    
    for (int i = 0; i < vector_list->len; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            printf("%lf ", (((vector_list->max_vectors)[i])->coordinates)[j]);
        }
        printf("\n");
    }
}

enum status_codes add_to_vector_list(T_vector_list** vector_list, T_vector* vector)
{
    if ((*vector_list)->len == 0)
    {
        ((*vector_list)->max_vectors)[0] = vector;
        (*vector_list)->len = 1;
        return fsc_ok;
    }
    
    if ((*vector_list)->len == (*vector_list)->capasity)
    {
        ((*vector_list)->capasity) *= 2;
        T_vector_list* new_vector_list = realloc((*vector_list), sizeof(T_vector_list) * (*vector_list)->capasity);
        if (new_vector_list == NULL)
        {
            clear_vector_list(vector_list);
            return fsc_memory_error_detected;
        }
    }
    
    int i = (*vector_list)->len;
    ((*vector_list)->max_vectors)[i] = vector;
    ++((*vector_list)->len);
    
    return fsc_ok;
}

enum status_codes max_vector(int n,
                             enum status_codes (*norm)(T_vector *, double *, void *),
                             void * input_param,
                             T_vector_list** res_list,
                             int count, ...)
{
    enum status_codes res = fsc_ok;
    res = create_vector_list(res_list);
    if (res != fsc_ok)
        return res;
    
    va_list tmp_x;
    va_start(tmp_x, count);
    double max_norm = -1.0;
    for (int i = 0; i < count; ++i)
    {
        T_vector* current_vector  = va_arg(tmp_x, T_vector*);
        double d;
        res = norm(current_vector, &d, input_param);
        
        if (fabs(d - max_norm) <= EPS)
        {
            res = add_to_vector_list(res_list, current_vector);
            if (res != fsc_ok)
                return res;
        }
        
        if (d > max_norm)
        {
            max_norm = d;
            (*res_list)->len = 0;
            add_to_vector_list(res_list, current_vector);
        }
    }
    return res;
}

int main(int argc, const char * argv[])
{
    enum status_codes res = fsc_ok;
    
    int n = 3;
    
    T_vector* a = NULL;
    res = create_T_vector(&a, n, 1.0, 2.0, 3.0);
    
    T_vector* b = NULL;
    if (res == fsc_ok)
        create_T_vector(&b, n, 1.0, 1.0, 1.0);
    
    T_vector* c = NULL;
    if (res == fsc_ok)
        create_T_vector(&c, n, 2.0, 1.0, 3.0);
    
    T_vector* d = NULL;
    if (res == fsc_ok)
        create_T_vector(&d, n, 3.0, 2.0, 1.0);

    T_vector* e = NULL;
    if (res == fsc_ok)
        create_T_vector(&e, n, 1.0, 2.0, 1.0);
    
    T_vector_list *res_list = NULL;
    if (res == fsc_ok)
        res = max_vector(n,
               norm_inf, NULL,
               &res_list,
                     5, a, b, c, d, e);
    
    if (res == fsc_ok)
    {
        printf("Vectors with max norm_inf:\n");
        print_vector_list(res_list);
        clear_vector_list(&res_list);
    }
    
    if (res == fsc_ok)
    {
        int p = 2;
        res = max_vector(n,
                   norm_p, &p,
                   &res_list,
                   5, a, b, c, d, e);
    }

    if (res == fsc_ok)
    {
        printf("Vectors with max norm_p:\n");
        print_vector_list(res_list);
        clear_vector_list(&res_list);
    }
    
    if (res == fsc_ok)
    {
        double A[9] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
        res = max_vector(n,
                   norm_A, A,
                   &res_list,
                   5, a, b, c, d, e);
    }
    
    if (res == fsc_ok)
    {
        printf("Vectors with max norm_A:\n");
        print_vector_list(res_list);
        clear_vector_list(&res_list);
    }

    if (a != NULL)
        clear_T_vector(&a);
    if (b != NULL)
        clear_T_vector(&b);
    if (c != NULL)
        clear_T_vector(&c);
    if (d != NULL)
        clear_T_vector(&d);
    if (e != NULL)
        clear_T_vector(&e);

    switch (res)
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
            break;
    }
    
    return res == fsc_ok ? 0 : 1;
}
