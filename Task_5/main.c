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
    fsc_invalid_file,
};

typedef struct
{
    char * str;
    int len;
} string;

enum status_codes create(string** new_string)
{
    *new_string = malloc(sizeof(string));
    if (new_string == NULL)
        return fsc_memory_error_detected;
    
    (*new_string)->str = NULL;
    (*new_string)->len = 0;
    
    return fsc_ok;
}

enum status_codes create_from_char(string** new_string, const char* str)
{
    enum status_codes function_result = create(new_string);
    
    if (function_result != fsc_ok)
        return function_result;
    
    const int len = (int)strlen(str);
    
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

// копирование без создания нового экземпляра
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
    if ((*current_string)->str != NULL)
        free((*current_string)->str);

    (*current_string)->str = NULL;
    (*current_string)->len = 0;

    free(*current_string);
    (*current_string) = NULL;
}

int comp(const string* a, const string* b)
{
    if (a->len != b->len)
        return (a->len > b->len) ? 1 : -1;

    return strcmp(a->str, b->str);
}

bool is_equivalents(const string* a, const string* b)
{
    return (strcmp(a->str, b->str) == 0);
}

bool is_empty(const string* a)
{
    return a->len == 0;
}

enum status_codes cat_string(string* destptr, const string* srcptr)
{
    const int new_len = destptr->len + srcptr->len;
    char* tmp_p = realloc(destptr->str, sizeof(char) * (new_len + 1));

    if (tmp_p == NULL)
    {
        // Старое значение строки остается в памяти
        return fsc_memory_error_detected;
    }
    
    destptr->str = tmp_p;
    
    strcat(destptr->str, srcptr->str);
    destptr->len = new_len;
    
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
    student** data;
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

enum status_codes create_student(student** current_student,int id, char* name, char* surname, char* group, char* marks)
{
    student *p = malloc(sizeof(student));
    *current_student = p;
    
    if (p == NULL)
        return fsc_memory_error_detected;
    
    p->name = NULL;
    p->surname = NULL;
    p->group = NULL;
    p->marks = NULL;
    
    enum status_codes function_result;
    
    function_result = create_from_char(&p->name, name);
    
    if (function_result == fsc_ok)
        function_result = create_from_char(&p->surname, surname);
    
    if (function_result == fsc_ok)
        function_result = create_from_char(&p->group, group);
    
    if (function_result == fsc_ok)
    {
        p->marks = malloc(sizeof(unsigned char) * 5);
        
        function_result = (p->marks != NULL) ? fsc_ok : fsc_memory_error_detected;
    }
    
    if (function_result == fsc_ok)
    {
        for (int i = 0; i < 5; ++i)
            (p->marks)[i] = marks[i] - '0';
        
        p->id = id;
    }
    
    if (function_result != fsc_ok)
    {
        if (p->name != NULL)
            clear_string(&p->name);
        
        if (p->surname != NULL)
            clear_string(&p->surname);
        
        if (p->group != NULL)
            clear_string(&p->group);
        
        free(p->marks);
        p->marks = NULL;
        
        free(p);
        (*current_student) = NULL;
    }
    
    return function_result;
}

void clear_student(student** current_student)
{
    student* p = *current_student;
    clear_string(&(p->name));
    clear_string(&(p->surname));
    clear_string(&(p->group));
    
    free(p->marks);

    free(*current_student);
    *current_student = NULL;
    
    return;
}


enum status_codes add_student (student_storage* student_list, student** current_student)
{
    if (student_list->capacity == 0)
    {
        const int start_capasity = 32;
        student_list->data = malloc(sizeof(student*) * start_capasity);
        if (student_list == NULL)
        {
            clear_student(current_student);
            return fsc_memory_error_detected;
        }
        student_list->capacity = start_capasity;
    }
    
    if (student_list->len == student_list->capacity)
    {
        const int capasity = student_list->capacity * 2;
        student** tmp_p = realloc(student_list->data, sizeof(student*) * capasity);
        if (tmp_p == NULL)
        {
            clear_student(current_student);
            return fsc_memory_error_detected;
        }
        
        student_list->capacity = capasity;
        student_list->data = tmp_p;
    }
    
    (student_list->data)[student_list->len] = *current_student;
    ++(student_list->len);
    
    return fsc_ok;
}

void student_printf(student* current_student)
{
    printf("id: %d\n", current_student->id);
    printf("Имя: %s\n", (current_student->name)->str);
    printf("Фамилия: %s\n", (current_student->surname)->str);
    printf("Группа: %s\n", (current_student->group)->str);
    
    printf("Оценки:\n");
    for (int i = 0; i < 5; ++i)
        printf("%d ",(current_student->marks)[i]);
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
        student_printf((student_list->data)[i]);
}

enum status_codes find_id (student_storage* student_list, int id, student** found_student)
{
    if (student_list->len == 0)
        return fsc_student_is_not_found;
    
    bool id_found = false;
    for (int i = 0; i < student_list->len; ++i)
        {
            student* current_student = (student_list->data)[i];
            
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
    
    enum status_codes function_result;
    string* surname_string;
    function_result = create_from_char(&surname_string, surname);
    if (function_result == fsc_ok)
    {
        for (int i = 0; i < student_list->len; ++i)
        {
            student* current_student = (student_list->data)[i];

            if (is_equivalents(current_student->surname, surname_string))
            {
                surname_found = true;
                function_result = add_student(res_student_list, &current_student);
                
                if (function_result != fsc_ok)
                {
                    clear_string(&surname_string);
                    return function_result;
                }
            }
            
        }
        
        clear_string(&surname_string);
    }
    
    return surname_found ? fsc_student_is_found : fsc_student_is_not_found;
}

enum status_codes find_name(student_storage* student_list, char* name, student_storage* res_student_list)
{
    if (student_list->len == 0)
        return fsc_student_is_not_found;
    
    bool name_found = false;
    
    enum status_codes function_result;
    string* name_string;
    function_result = create_from_char(&name_string, name);
    if (function_result == fsc_ok)
    {
        for (int i = 0; i < student_list->len; ++i)
        {
            student* current_student = (student_list->data)[i];

            if (is_equivalents(current_student->name, name_string))
            {
                name_found = true;
                function_result = add_student(res_student_list, &current_student);
                
                if (function_result != fsc_ok)
                {
                    clear_string(&name_string);
                    return function_result;
                }
            }
        }
        
        clear_string(&name_string);
    }
    
    return name_found ? fsc_student_is_found : fsc_student_is_not_found;
}


enum status_codes find_group(student_storage* student_list, char* group, student_storage* res_student_list)
{
    if (student_list->len == 0)
        return fsc_student_is_not_found;
    
    bool group_found = false;
    
    enum status_codes function_result;
    string* group_string;
    function_result = create_from_char(&group_string, group);
    if (function_result == fsc_ok)
    {
        for (int i = 0; i < student_list->len; ++i)
        {
            student* current_student = (student_list->data)[i];

            if (is_equivalents(current_student->group, group_string))
            {
                group_found = true;
                function_result = add_student(res_student_list, &current_student);
                
                if (function_result != fsc_ok)
                {
                    clear_string(&group_string);
                    return function_result;
                }
            }
        }
        
        clear_string(&group_string);
    }
    
    return group_found ? fsc_student_is_found : fsc_student_is_not_found;
}


void clear_student_storage (student_storage** student_list)
{
    for (int i = 0; i < (*student_list)->len; ++i)
    {
        clear_student(&(((*student_list)->data)[i]));
    }
    
    free(*student_list);
    *student_list = NULL;
    return;
}


int student_comp(const void * a, const void * b)
{
    student* student_a = *(student **)a;
       student* student_b = *(student **)b;
    
    if (student_a->id != student_b->id)
        return (student_a->id > student_b->id) ? 1 : -1;
        
    int res = comp(student_a->surname, student_b->surname);
    if (res != 0)
        return res;
    
    res = comp(student_a->name, student_b->name);
    if (res != 0)
        return res;
    
    res = comp(student_a->group, student_b->surname);
    if (res != 0)
        return res;
    
    return 0;
}


double middle_mark (student* current_student)
{
    double res_mark = 0.0;
    
    for (int i = 0; i < 5; ++i)
        res_mark += (current_student->marks)[i];
    
    res_mark /= 5.0;
    
    return res_mark;
}

double maiddle_mark_all_students (student_storage* student_list)
{
    double res_mark = 0.0;
    
    for (int i = 0; i < student_list->len; ++i)
        res_mark += middle_mark((student_list->data)[i]);
    
    res_mark /= (student_list->len);
    
    return res_mark;
}


int main(int argc, const char * argv[])
{
    enum status_codes function_result = fsc_ok;
    FILE* file_input = NULL;
    
    student_storage* student_list;
    function_result = create_student_storage(&student_list);
    
    if (function_result == fsc_ok)
    {
        function_result = (argc >= 2) ? fsc_ok : fsc_invalid_parameter;
    }
    
    if (function_result == fsc_ok)
    {
        file_input = fopen(argv[1], "rb");
        if (file_input == NULL)
            function_result = fsc_file_is_not_found;
    }
        
    if (function_result == fsc_ok)
    {
        int id = 0;
        char name[MAX_BUFFER_SIZE];
        char surname[MAX_BUFFER_SIZE];
        char group[MAX_BUFFER_SIZE];
        char marks[6];
        
        while (true)
        {
            int param_num = fscanf(file_input, "%d %s %s %s %5s\n", &id, name, surname, group, marks);
            
            if (param_num == 0)
                break;
            
            if (param_num != 5)
            {
                function_result = fsc_invalid_file;
                break;
            }
            
            student* current_student;
            function_result = create_student(&current_student, id, name, surname, group, marks);
            
            if (function_result == fsc_ok)
                function_result = add_student(student_list, &current_student);
            else
                break;
            
            if (function_result != fsc_ok)
                break;
        }
    }
    
    if (function_result == fsc_ok)
    {
        int i;
        bool flag = false;
        
        while (true)
        {

            printf ("0 - Вывести список студентов\n1 - Найти студента по id\n2 - Найти студентов по фамилии\n3 - Найти студента по имени\n4 - Найти студента по номеру группы\n5 - Отсортировать список студентов\n6 - Вывод среднего балла студента\n7 - Вывод среднего балла по всем студентам\n8 - Вывод в файл найденного по id студента\n9 - Вывод в файл студентов, чей средний балл выше, чем у всех студентов\n10 - Завершить\n");
            
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
                    function_result = create_student_storage(&res_student_list);
                    
                    if (function_result == fsc_ok)
                    {
                        char surname[MAX_BUFFER_SIZE];
                        printf("Введите фамилию:\n");
                        scanf("%s", surname);
                        
                        if (find_surname(student_list, surname, res_student_list) == fsc_student_is_found)
                            student_storage_printf(res_student_list);
                        else
                            printf("Students witn such surname are not found\n");
                        
                        free(res_student_list);
                    }
                    break;
                }
                case 3://Найти студента по имени
                {
                    student_storage* res_student_list;
                    function_result = create_student_storage(&res_student_list);
                    
                    if (function_result == fsc_ok)
                    {
                        char name[MAX_BUFFER_SIZE];
                        printf("Введите имя:\n");
                        scanf("%s", name);
                        
                        if (find_name(student_list, name, res_student_list) == fsc_student_is_found)
                            student_storage_printf(res_student_list);
                        else
                            printf("Students witn such name are not found\n");
                        
                        free(res_student_list);
                    }
                    break;
                }
                case 4://Найти студента по номеру группы
                {
                    student_storage* res_student_list;
                    function_result = create_student_storage(&res_student_list);
                    
                    if (function_result == fsc_ok)
                    {
                        char group[MAX_BUFFER_SIZE];
                        printf("Введите номер группы:\n");
                        scanf("%s", group);
                        
                        if (find_group(student_list, group, res_student_list) == fsc_student_is_found)
                            student_storage_printf(res_student_list);
                        else
                            printf("Students with such group are not found\n");
                        
                        free(res_student_list);
                    }
                    break;
                }
                case 5://Отсортировать список студентов
                {
                    qsort(student_list->data, student_list->len, sizeof(student*), student_comp);
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
                    FILE* file_output = NULL;
                    
                    function_result = (argc >= 3) ? fsc_ok : fsc_invalid_parameter;

                    if (function_result == fsc_ok)
                    {
                        file_output = fopen(argv[2], "wb");
                        
                        if (file_output == NULL)
                        {
                            function_result = fsc_memory_error_detected;
                            fclose(file_input);
                        }
                    }
                    
                    if (function_result == fsc_ok)
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
                    
                    break;
                }
                case 9://Вывод в файл студентов, чей средний балл выше, чем у всех студентов
                {
                    FILE* file_output = NULL;
                    
                    function_result = (argc >= 3) ? fsc_ok : fsc_invalid_parameter;
                    
                    if (function_result == fsc_ok)
                    {

                        file_output = fopen(argv[2], "wb");
                        
                        if (file_output == NULL)
                        {
                            function_result = fsc_memory_error_detected;
                            fclose(file_input);
                        }
                    }
                    
                    if (function_result == fsc_ok)
                    {
                        double mm = maiddle_mark_all_students(student_list);
                        
                        for (int i = 0; i < student_list->len; ++i)
                        {
                            double current_middle_mark = middle_mark((student_list->data)[i]);
                            
                            if (mm < current_middle_mark)
                                fprintf(file_output, "%s %s %lf\n", (((student_list->data)[i])->surname)->str, (((student_list->data)[i])->name)->str, current_middle_mark);
                        }
                            
                        fclose(file_output);
                    }
                    break;
                }
                case 10:
                    flag = true;
                    break;
                default:
                    function_result = fsc_invalid_parameter;
                    break;
            }
            
            if (flag)
                break;
            
            
            if (function_result != fsc_ok)
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
        default:
            break;
    }
    
    return function_result == fsc_ok ? 0 : 1;
}
