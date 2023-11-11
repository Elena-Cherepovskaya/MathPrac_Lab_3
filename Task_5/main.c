//
//  main.c
//  5
//
//  Created by Лена on 09.11.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define MAX_BUFFER_SIZE 256

enum status_codes
{
    fsc_ok,
    fsc_overflow,
    fsc_invalid_parameter,
    fsc_memory_error_detected,
    fsc_file_is_not_found,
    fsc_invalid_data,
    fsc_student_is_not_found,
    fsc_student_is_found,
    fsc_student_storage_is_empty,
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

void clear_string(string* current_string)
{
    free (current_string->str);
    free(current_string);
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

bool is_empty(string* current_string)
{
        return (current_string->len == 0) ? true : false;
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

typedef struct
{
    int id;
    string* name;
    string* surname;
    string* group;
    unsigned char* marks;//оценки за 5 экзаменов
}student;

typedef struct
{
    student* data;
    int len;
    int capacity;
}student_storage;

enum status_codes create_student_storage(student_storage** student_list)
{
    *student_list = malloc(sizeof(student_storage));
    if (*student_list == NULL)
        return fsc_memory_error_detected;
    (*student_list)->data = NULL;
    (*student_list)->capacity = 0;
    (*student_list)->len = 0;
    
    return fsc_ok;
}

enum status_codes create_student(student** current_student,int id, char* name, char* surname, char* group, unsigned char* marks)
{
    (*current_student) = malloc(sizeof(student));
    if (*current_student == NULL)
        return fsc_memory_error_detected;
    
    (*current_student)->id = id;
    create_from_char(&((*current_student)->name), name);
    create_from_char(&((*current_student)->surname), surname);
    create_from_char(&((*current_student)->group), group);
    (*current_student)->marks = malloc(sizeof(unsigned char) * 6);
    
    strcpy((*current_student)->marks, marks);
    return fsc_ok;
}

enum status_codes add_student (student current_student, student_storage** student_list)
{
    if ((*student_list)->capacity == 0)
    {
        (*student_list)->capacity = 32;
        (*student_list)->data = malloc(sizeof(student) * 32);
        if (*student_list == NULL)
            return fsc_memory_error_detected;
    }
    
    if ((*student_list)->len + 1 == (*student_list)->capacity)
    {
        (*student_list)->capacity *= 2;
        (*student_list)->data = malloc(sizeof(student) * (*student_list)->capacity);
        if (*student_list == NULL)
            return fsc_memory_error_detected;
    }
    else
    {
        student_storage* p_list = *student_list;
        
        (p_list->data)[p_list->len] = current_student;
        (p_list->len) += 1;
    }
    
    return fsc_ok;
}

void student_printf(student* current_student)
{
    printf("id: %d\n", current_student->id);
    printf("Имя: %s\n", (current_student->name)->str);
    printf("Фамилия: %s\n", (current_student->surname)->str);
    printf("Группа: %s\n", (current_student->group)->str);
    printf("Оценки: %s\n", current_student->marks);
    printf("\n");
}

void student_storage_printf(student_storage* student_list)
{
    if (student_list->len == 0)
    {
        printf("Student storage is empty\n");
        return;
    }
    
    for (int i = 0; i < student_list->len; ++i)
        student_printf(&((student_list->data)[i]));
    
}

enum status_codes find_id (student_storage* student_list, int id, student** found_student)
{
    if (student_list->len == 0)
        return fsc_student_is_not_found;
    
    bool id_found = false;
    for (int i = 0; i < student_list->len; ++i)
        {
            student* current_student = &(student_list->data)[i];
            
            if (current_student->id == id)
            {
                id_found = true;
                *found_student = current_student;
                break;
            }
        }
    
    return id_found ? fsc_student_is_found : fsc_student_is_not_found;
}

enum status_codes find_surname (student_storage* student_list, char* surname, student_storage* res_student_list)
{
    if (student_list->len == 0)
        return fsc_student_is_not_found;
    
    bool surname_found = false;
    string* surname_string;
    create_from_char(&surname_string, surname);
    for (int i = 0; i < student_list->len; ++i)
    {
        student current_student = (student_list->data)[i];

        if (is_equivalents(current_student.surname, surname_string))
        {
            surname_found = true;
            add_student(current_student, &res_student_list);
        }
        
    }
    
    clear_string(surname_string);
    return surname_found ? fsc_student_is_found : fsc_student_is_not_found;
}

enum status_codes find_name (student_storage* student_list, char* name, student_storage* res_student_list)
{
    if (student_list->len == 0)
        return fsc_student_is_not_found;
    
    bool name_found = false;
    string* name_string;
    create_from_char(&name_string, name);
    for (int i = 0; i < student_list->len; ++i)
    {
        student current_student = (student_list->data)[i];

        if (is_equivalents(current_student.name, name_string))
        {
            name_found = true;
            add_student(current_student, &res_student_list);
        }
        
    }
    
    clear_string(name_string);
    
    return name_found ? fsc_student_is_found : fsc_student_is_not_found;
}


enum status_codes find_group (student_storage* student_list, char* group, student_storage* res_student_list)
{
    if (student_list->len == 0)
        return fsc_student_is_not_found;
    
    bool group_found = false;
    string* group_string;
    create_from_char(&group_string, group);
    for (int i = 0; i < student_list->len; ++i)
    {
        student current_student = (student_list->data)[i];

        if (is_equivalents(current_student.group, group_string))
        {
            group_found = true;
            add_student(current_student, &res_student_list);
        }
        
    }
    
    clear_string(group_string);
    return group_found ? fsc_student_is_found : fsc_student_is_not_found;
}

void clear_student(student* current_student)
{
    clear_string(current_student->name);
    clear_string(current_student->surname);
    clear_string(current_student->group);
    
    free(current_student->marks);
    return;
}


void clear_student_storage (student_storage* student_list)
{
    for (int i = 0; i < student_list->len; ++i)
    {
        student* current_student = &((student_list->data)[i]);
        clear_student(current_student);
    }
    
    free(student_list);
    return;
}


int student_comp(const void * a, const void * b)
{
    student student_a = *((student*)a);
    student student_b = *((student*)b);
    
    if (student_a.id != student_b.id)
        return (student_a.id > student_b.id) ? 1 : -1;
        
    int res = comp(student_a.surname, student_b.surname);
    if (res != 0)
        return res;
    
    res = comp(student_a.name, student_b.name);
    if (res != 0)
        return res;
    
    res = comp(student_a.group, student_b.surname);
    if (res != 0)
        return res;
    
    return 0;
}

int char_to_num (char c)
{
    return (int)(c - '0');
}

double middle_mark (student* current_student)
{
    double res_mark = 0.0;
    
    for (int i = 0; i < 5; ++i)
        res_mark += char_to_num((current_student->marks)[i]);
    
    res_mark /= 5.0;
    
    return res_mark;
}

double maiddle_mark_all_students (student_storage* student_list)
{
    double res_mark = 0.0;
    
    for (int i = 0; i < student_list->len; ++i)
        res_mark += middle_mark(&((student_list->data)[i]));
    
    res_mark /= (student_list->len);
    
    return res_mark;
}


int main(int argc, const char * argv[])
{
    enum status_codes function_result = fsc_ok;
    FILE* file_input = NULL;
    
    student_storage* student_list;
    create_student_storage(&student_list);
    
    if (argc < 2)
        function_result = fsc_invalid_parameter;
    else
    {
        file_input = fopen(argv[1], "rb");
        if (file_input == NULL)
            function_result = fsc_file_is_not_found;
        else
        {
            
            int id = 0;
            char name[MAX_BUFFER_SIZE];
            char surname[MAX_BUFFER_SIZE];
            char group[MAX_BUFFER_SIZE];
            unsigned char marks[6];
            
            while (true)
            {

                int param_num = fscanf(file_input, "%d %s %s %s %s\n", &id, name, surname, group, marks);
                
                if (param_num == 0)
                    break;
                
                student* current_student;
                create_student(&current_student, id, name, surname, group, marks);
                
                add_student(*current_student, &student_list);
            }
        }
        
        int i;
        bool flag = false;
        printf ("0 - Вывести список студентов\n1 - Найти студента по id\n2 - Найти студентов по фамилии\n3 - Найти студента по имени\n4 - Найти студента по номеру группы\n5 - Отсортировать список студентов\n6 - Вывод среднего балла студента\n7 - Вывод среднего балла по всем студентам\n8 - Вывод в файл найденного по id студента\n9 - Вывод в файл студентов, чей средний балл выше, чем у всех студентов\n10 - Завершить\n");
        while (true)
        {
            printf("Выберите пункт меню: \n");
            
            scanf("%d", &i);
            
            switch (i)
            {
                case 0://Вывести список студентов
                    student_storage_printf(student_list);
                    break;
                case 1://Найти студента по id
                    printf("Введите id:\n");
                    int id;
                    scanf("%d", &id);
                    
                    student* found_student = NULL;
                    if (find_id(student_list, id, &found_student) == fsc_student_is_found)
                        student_printf(found_student);
                    else
                        printf("Student with such id is not found\n");
                    break;
                case 2://Найти студентов по фамилии
                {
                    student_storage* res_student_list;
                    create_student_storage(&res_student_list);
                    
                    char surname[MAX_BUFFER_SIZE];
                    printf("Введите фамилию:\n");
                    scanf("%s", surname);
                    
                    if (find_surname(student_list, surname, res_student_list) == fsc_student_is_found)
                        student_storage_printf(res_student_list);
                    else
                        printf("Students witn such surname are not found\n");
                    
                    free(res_student_list);
                    break;
                }
                case 3://Найти студента по имени
                {
                    student_storage* res_student_list;
                    create_student_storage(&res_student_list);
                    
                    char name[MAX_BUFFER_SIZE];
                    printf("Введите имя:\n");
                    scanf("%s", name);
                    
                    if (find_name(student_list, name, res_student_list) == fsc_student_is_found)
                        student_storage_printf(res_student_list);
                    else
                        printf("Students witn such name are not found\n");
                    
                    free(res_student_list);
                    break;
                }
                case 4://Найти студента по номеру группы
                {
                    student_storage* res_student_list;
                    create_student_storage(&res_student_list);
                    
                    char group[MAX_BUFFER_SIZE];
                    printf("Введите номер группы:\n");
                    scanf("%s", group);
                    
                    if (find_group(student_list, group, res_student_list) == fsc_student_is_found)
                        student_storage_printf(res_student_list);
                    else
                        printf("Students witn such group are not found\n");
                    
                    free(res_student_list);
                    break;
                }
                case 5://Отсортировать список студентов
                {
                    qsort(student_list->data, student_list->len, sizeof(student), student_comp);
                    break;
                }
                case 6://Вывод среднего балла студента
                {
                    printf("Введите id:\n");
                    int id;
                    scanf("%d", &id);
                    
                    student* found_student = NULL;
                    if (find_id(student_list, id, &found_student) == fsc_student_is_found)
                    {
                        student_printf(found_student);
                        printf("Средний балл студента: %lf\n", middle_mark(found_student));
                    }
                    else
                        printf("Student with such id is not found\n");
                    break;
                }
                case 7://Вывод среднего балла по всем студентам
                {
                    printf("Общий средний балл всех студентов: %lf\n", maiddle_mark_all_students(student_list));
                    break;
                }
                case 8://Вывод в файл найденного по id студента
                {
                    if (argc < 3)
                        function_result = fsc_invalid_parameter;
                    else
                    {
                        FILE* file_output = NULL;
                        file_output = fopen(argv[2], "wb");
                        
                        if (file_output == NULL)
                        {
                            function_result = fsc_memory_error_detected;
                            fclose(file_input);
                        }
                        else
                        {
                            printf("Введите id:\n");
                            int id;
                            scanf("%d", &id);
                            
                            student* found_student = NULL;
                            if (find_id(student_list, id, &found_student) == fsc_student_is_found)
                            {
                                fprintf(file_output, "%s %s %s %lf\n", (found_student->surname)->str, (found_student->name)->str, (found_student->group)->str, middle_mark(found_student));
                            
                            }
                            else
                                printf("Student with such id is not found\n");
                            fclose(file_output);
                        }
                    }
                    break;
                }
                case 9://Вывод в файл студентов, чей средний балл выше, чем у всех студентов
                {
                    if (argc < 3)
                        function_result = fsc_invalid_parameter;
                    else
                    {
                        FILE* file_output = NULL;
                        file_output = fopen(argv[2], "wb");
                        
                        if (file_output == NULL)
                        {
                            function_result = fsc_memory_error_detected;
                            fclose(file_input);
                        }
                        else
                        {
                            double mm = maiddle_mark_all_students(student_list);
                            
                            for (int i = 0; i < student_list->len; ++i)
                            {
                                double current_middle_mark = middle_mark(&(student_list->data)[i]);
                                
                                if (mm < current_middle_mark)
                                    fprintf(file_output, "%s %s %lf\n", (((student_list->data)[i]).surname)->str, (((student_list->data)[i]).name)->str, current_middle_mark);
                            }
                                
                            fclose(file_output);
                        }
                        break;
                    }
                }
                case 10:
                    flag = true;
                    break;
                default:
                    break;
            }
            
            if (flag)
                break;
        }
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
        case fsc_invalid_data:
            printf("Invalid data detected\n");
            break;
        case fsc_student_is_found:
            printf("Stident is not found\n");
            break;
        case fsc_student_storage_is_empty:
            printf("Student storage is empty\n");
            break;
        default:
            break;
    }
    
    return function_result == fsc_ok ? 0 : 1;
}
