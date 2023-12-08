//
//  main.c
//  3
//
//  Created by Лена on 01.11.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

enum status_codes
{
    fsc_ok,
    fsc_overflow,
    fsc_invalid_parameter,
    fsc_memory_error_detected,
    fsc_file_is_not_found,
    fsc_invalid_file,
};

#define BUFFER_SIZE 256

//Эпсилон сделать парметром функции
#define EPS 0.0001

typedef struct
{
    unsigned int id;
    char* name;
    char* sername;
    double salary;//неотрицательное
}employee;

int comp(const void * a, const void * b)
{
    employee* employee_a = *(employee**)a;
    employee* employee_b = *(employee**)b;
    
    if (fabs(employee_a->salary - employee_b->salary) > EPS)
        return (employee_a->salary > employee_b->salary) ? 1 : -1;

    int cmp = strcmp(employee_a->sername, employee_b->sername);
    if (cmp != 0)
        return cmp;
    
    cmp = strcmp(employee_a->name, employee_b->name);
    if (!cmp)
        return cmp;
    
    return (employee_a->id > employee_b->id) ? 1 : -1;
}

int comp_d(const void * a, const void * b)
{
    int c = comp(a, b);
    
    if (c == 0)
        return c;
    return 0 - c;
}

enum status_codes create_employees_list(employee*** employees_list, int start_list_size)
{
    if (*employees_list != NULL)
        return fsc_invalid_parameter;
    
    *employees_list = (employee**)malloc(sizeof(employee*) * start_list_size);

    if (*employees_list == NULL)
        return fsc_memory_error_detected;
    
    return fsc_ok;
}

enum status_codes create_employee(employee** new_employee, int id, char* name, char* sername, double salary)
{
    if (*new_employee != NULL)
        return fsc_invalid_parameter;
    
    *new_employee = malloc(sizeof(employee));
    if (*new_employee == NULL)
        return fsc_memory_error_detected;
    
    (*new_employee)->id = id;
    
    (*new_employee)->name = malloc(sizeof(char) * (strlen(name) + 1));
    if ((*new_employee)->name == NULL)
    {
        free(*new_employee);
        *new_employee = NULL;
        return fsc_memory_error_detected;
    }
    strcpy((*new_employee)->name, name);
    
    (*new_employee)->sername = malloc(sizeof(char) * (strlen(sername) + 1));
    if ((*new_employee)->sername == NULL)
    {
        free((*new_employee)->sername);
        free(*new_employee);
        *new_employee = NULL;
        return fsc_memory_error_detected;
    }
    strcpy((*new_employee)->sername, sername);
    
    (*new_employee)->salary = salary;
    return fsc_ok;
}

enum status_codes clear_employees_list(employee*** employees_list, int* max_list_size, int* ind)
{
    if (*employees_list == NULL)
        return fsc_invalid_parameter;
    if (max_list_size == NULL)
        return fsc_invalid_parameter;
    if (ind == NULL)
        return fsc_invalid_parameter;
    
    for (int i = 0; i < (*ind); ++i)
    {
        free((*employees_list)[i]->name);
        free((*employees_list)[i]->sername);
        free((*employees_list)[i]);
    }
    free(*employees_list);
    *employees_list = NULL;
    *max_list_size = 0;
    *ind = 0;
    return fsc_ok;
}


enum status_codes add_employee_to_employees_list(employee*** employees_list, int* max_list_size, int* ind, employee* current_employee)
{
    if (*employees_list == NULL)
        return fsc_invalid_parameter;
    
    if ((*ind) == (*max_list_size))
    {
        employee** tmp_employees_list = realloc(*employees_list, sizeof(employee*) * ((*max_list_size) * 2));
        if (tmp_employees_list == NULL)
        {
            clear_employees_list(employees_list, max_list_size, ind);
            return fsc_memory_error_detected;
        }
        *employees_list = tmp_employees_list;
    }
    
    (*employees_list)[*ind] = current_employee;
    ++(*ind);
    
    return fsc_ok;
}

enum status_codes print_employees_list(FILE* output, employee** employees_list, int* ind)
{
    if (employees_list == NULL)
        return fsc_invalid_parameter;
    if (ind == NULL)
        return fsc_invalid_parameter;
    
    for (int i = 0; i < *ind; ++i)
    {
        fprintf(output, "%d\n", employees_list[i]->id);
        fprintf(output, "%s\n", employees_list[i]->name);
        fprintf(output, "%s\n", employees_list[i]->sername);
        fprintf(output, "%lf\n\n", employees_list[i]->salary);
    }
    
    return fsc_ok;
}

int main(int argc, const char * argv[])
{
    enum status_codes res = fsc_ok;
    
    FILE* file_input = NULL;
    FILE* file_output = NULL;
    
    if (argc < 4)
        res = fsc_invalid_parameter;
     
    if (res == fsc_ok)
    {
        file_input = fopen(argv[1], "r");
        if (file_input == NULL)
            res = fsc_file_is_not_found;
    }

    if (res == fsc_ok)
    {
        file_output = fopen(argv[3], "w");
        if (file_output == NULL)
        {
            res = fsc_memory_error_detected;
            fclose(file_input);
        }
    }
    
    int max_list_size = 32;
    employee** employees_list = NULL;
    res = create_employees_list(&employees_list, max_list_size);
    
    int ind = 0;
    if (res == fsc_ok)
    {
        unsigned int id;
        char name[BUFFER_SIZE];
        char sername[BUFFER_SIZE];
        double salaty;
        
        while (!(feof(file_input)))
        {
            int count = fscanf(file_input, "%d %225s %225s %lf",&id, name, sername, &salaty);
            
            if (count != 4)
            {
                res = fsc_invalid_file;
                break;
            }
            
            employee* new_employee = NULL;
            res = create_employee(&new_employee, id, name, sername, salaty);
            if (res != fsc_ok)
                break;
            
            res = add_employee_to_employees_list(&employees_list, &max_list_size, &ind, new_employee);
            if (res != fsc_ok)
                break;
        }
    }
    
    
    if (res == fsc_ok)
    {
        char flag = argv[2][1];
        
        printf("Изначальный список\n");
        print_employees_list(stdout, employees_list, &ind);
        
        switch (flag)
        {
            // по возрастанию
            case 'a':
            {
                qsort(employees_list, ind, sizeof(employee*), comp);
                printf("Список отсортированный по возрастанию:\n");
                print_employees_list(stdout, employees_list, &ind);
                print_employees_list(file_output, employees_list, &ind);
                break;
            }
            // по убыванию
            case 'd':
                qsort(employees_list, ind, sizeof(employee*), comp_d);
                printf("Список отсортированный по убыванию:\n");
                print_employees_list(stdout, employees_list, &ind);
                print_employees_list(file_output, employees_list, &ind);
                break;
            default:
                break;
        }
    }
   
    if (employees_list != NULL)
        clear_employees_list(&employees_list, &max_list_size, &ind);
    
    if (file_input != NULL)
        fclose(file_input);
    if (file_output != NULL)
        fclose(file_output);
    
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
            printf("function_result is unknown\n");
    }
    
    return res == fsc_ok ? 0 : 1;
}
