//
//  main.c
//  7
//
//  Created by Лена on 30.11.2023.
//

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define EPS 0.0004
#define BUFFER_SIZE 256

enum status_codes
{
    fsc_ok,
    fsc_overflow,
    fsc_invalid_parameter,
    fsc_memory_error_detected,
    fsc_file_is_not_found,
    fsc_file_is_empty,
    fsc_error_undo,
    fsc_invalid_file,
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

typedef struct
{
    string* surname;
    string* name;
    string* patronymic;
    time_t birth_date;
    char gender;
    double salary;
}T_liver;

enum status_codes create_liver(T_liver** liver,
                               char* surname,
                               char* name,
                               char* patronymic,
                               time_t birth_date,
                               char gender,
                               double salary)
{
    *liver = malloc(sizeof(T_liver));
    if (*liver == NULL)
        return fsc_memory_error_detected;
    
    enum status_codes res = fsc_ok;
    
    res = create_from_char(&((*liver)->surname), surname);
    if (res != fsc_ok)
        return res;
    
    res = create_from_char(&((*liver)->name), name);
    if (res != fsc_ok)
        return res;
    
    res = create_from_char(&((*liver)->patronymic), patronymic);
    if (res != fsc_ok)
        return res;

    (*liver)->birth_date = birth_date;
    (*liver)->gender = gender;
    (*liver)->salary = salary;
    
    return fsc_ok;
}

void clear_liver(T_liver** liver)
{
    clear_string(&((*liver)->surname));
    clear_string(&((*liver)->name));
    clear_string(&((*liver)->patronymic));
    free(*liver);
    *liver = NULL;
}

typedef struct
{
    T_liver* data;
    void* p_next;
}T_node;

enum status_codes create_node(T_node** node, T_liver* liver)
{
    (*node) = malloc(sizeof(T_node));
    if (*node == NULL)
        return fsc_memory_error_detected;
    
    (*node)->data = liver;
    (*node)->p_next = NULL;
    
    return fsc_ok;
}

void clear_node(T_node** node)
{
    free(*node);
    *node = NULL;
}

void clear_list(T_node** root)
{
    T_node* current_node  = *root;
    T_node* prev_node = NULL;
    while (current_node != NULL)
    {
        clear_liver(&(current_node->data));
        prev_node = current_node;
        current_node = current_node->p_next;
        clear_node(&prev_node);
    }
    *root = NULL;
}

enum status_codes copy_liver(T_liver** new_liver, T_liver* liver)
{
    enum status_codes res = fsc_ok;
    
    *new_liver = malloc(sizeof(T_liver));
    if (*new_liver == NULL)
        return fsc_memory_error_detected;
    
    res = create_from_string(&((*new_liver)->surname), liver->surname);
    if (res != fsc_ok)
        return res;
    
    res = create_from_string(&((*new_liver)->name), liver->name);
    if (res != fsc_ok)
        return res;
    
    res = create_from_string(&((*new_liver)->patronymic), liver->patronymic);
    if (res != fsc_ok)
        return res;
    
    (*new_liver)->birth_date = liver->birth_date;
    (*new_liver)->gender = liver->gender;
    (*new_liver)->salary = liver->salary;
    
    return fsc_ok;
}

enum status_codes copy_list(T_node** new_root, T_node* root)
{
    enum status_codes res = fsc_ok;
    
    T_node* current_node = root;
    T_node* prev_node = NULL;
    T_liver* new_liver;
    while (current_node != NULL)
    {
        res = copy_liver(&new_liver, current_node->data);
        if (res != fsc_ok)
            break;
        
        T_node* new_node;
        res = create_node(&new_node, new_liver);
        if (res != fsc_ok)
        {
            clear_liver(&new_liver);
            break;
        }
        if (prev_node != NULL)
            prev_node->p_next = new_node;
        else
            *new_root = new_node;
        prev_node = new_node;
        current_node = current_node->p_next;
    }
    
    if (res != fsc_ok)
        clear_list(new_root);
    
    return res;
}


void add_node_to_list(T_node** root, T_node* node)
{
    //Нужно вставлять перед корнем
    if (difftime((*root)->data->birth_date, node->data->birth_date) > 0)
    {
        node->p_next = *root;
        (*root) = node;
        return;
    }
    
    T_node* current_node  = (*root);
    T_node* prev_node = NULL;
    bool flag = false;
    while (current_node != NULL)
    {
        
        if (difftime(current_node->data->birth_date, node->data->birth_date) > 0)
        {
            flag = true;
            break;
        }
        
        prev_node = current_node;
        current_node = current_node->p_next;
    }
    
    //Нужно вставлять в конец
    if (!(flag))
    {
        prev_node->p_next = node;
        return;
    }
    
    //Нужно вставлять после prev_node
    T_node* next_node = current_node;
    prev_node->p_next = node;
    node->p_next = next_node;
    return;
}

void print_list(T_node* root)
{
    T_node* current_node = root;
    while (current_node != NULL)
    {
        printf("%s %s %s ",
               current_node->data->surname->str,
               current_node->data->name->str,
               current_node->data->patronymic->str);
        
        struct tm * tm = localtime(&(current_node->data->birth_date));
        printf("%d:%d:%d ", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900);
        
        printf("%c %.0lf \n", current_node->data->gender, current_node->data->salary);
        
        current_node = current_node->p_next;
    }
}

bool copmare_livers(T_liver* current_liver, char* surname, char* name, char* patronymic, char* birth_date, char gender, char* salary)
{
    bool flag = true;
    for (int i = 0; i < 6; ++i)
    {
        switch (i)
        {
            case 0:
                if (surname[0] == '*')
                {
                    flag = true;
                    break;
                }
                flag = (strcmp(current_liver->surname->str, surname) == 0);
                break;
            case 1:
                if (name[0] == '*')
                {
                    flag = true;
                    break;
                }
                flag = (strcmp(current_liver->name->str, name) == 0);
                break;
            case 2:
                if (patronymic[0] == '*')
                {
                    flag = true;
                    break;
                }
                flag = (strcmp(current_liver->patronymic->str, patronymic) == 0);
                break;
            case 3:
            {
                if (birth_date[0] == '*')
                {
                    flag = true;
                    break;
                }
                
                struct tm tm;
                memset(&tm, 0, sizeof(struct tm));
                strptime(birth_date, "%d:%m:%Y", &tm);
                time_t time = mktime(&tm);
                
                flag = (difftime(current_liver->birth_date, time) == 0);
                break;
            }
            case 4:
                if (gender == '*')
                {
                    flag = true;
                    break;
                }
                
                flag = (current_liver->gender == gender);
                break;
            case 5:
            {
                if (salary[0] == '*')
                {
                    flag = true;
                    break;
                }
                
                double d_salary = atof(salary);
                flag = (fabs(d_salary - current_liver->salary) <= EPS);
                break;
            }
            default:
                break;
        }
        
        if (!(flag))
            return false;
    }
    return true;
}

enum status_codes find_livers(T_node** new_root, T_node* root, char* surname, char* name, char* patronymic, char* birth_date, char gender, char* salaty)
{
    enum status_codes res = fsc_ok;
    
    T_node* current_node = root;
    T_node* prev_node = NULL;
    T_liver* new_liver;
    while (current_node != NULL)
    {
        if (copmare_livers(current_node->data, surname, name, patronymic, birth_date, gender, salaty))
        {
            res = copy_liver(&new_liver, current_node->data);
            if (res != fsc_ok)
                break;
            
            T_node* new_node;
            res = create_node(&new_node, new_liver);
            if (res != fsc_ok)
            {
                clear_liver(&new_liver);
                break;
            }
            if (prev_node != NULL)
                prev_node->p_next = new_node;
            else
                *new_root = new_node;
            prev_node = new_node;
        }
        current_node = current_node->p_next;
    }
    
    if (res != fsc_ok)
        clear_list(new_root);
    
    return res;
}


void change_nodes_in_list(T_node* root, char* surname, char* name, char* patronymic, char* birth_date, char gender, char* salaty, char* new_surname, char* new_name, char* new_patronymic, char* new_birth_date, char new_gender, char* new_salaty)
{
    T_node* current_node = root;
    while(current_node != NULL)
    {
        if (copmare_livers(current_node->data, surname, name, patronymic, birth_date, gender, salaty))
        {
            for (int i = 0; i < 6; ++i)
            {
                switch (i)
                {
                    case 0:
                        if (new_surname[0] == '*')
                            break;
                        create_from_char(&(current_node->data->surname), new_surname);
                        break;
                    case 1:
                        if (new_name[0] == '*')
                            break;
                        create_from_char(&(current_node->data->name), new_name);
                        break;
                    case 2:
                        if (new_patronymic[0] == '*')
                            break;
                        create_from_char(&(current_node->data->patronymic), new_patronymic);
                        break;
                    case 3:
                    {
                        if (new_birth_date[0] == '*')
                            break;
                        
                        struct tm tm;
                        memset(&tm, 0, sizeof(struct tm));
                        strptime(new_birth_date, "%d:%m:%Y", &tm);
                        time_t time = mktime(&tm);
                        
                        current_node->data->birth_date = time;
                        break;
                    }
                    case 4:
                        if (new_gender == '*')
                            break;
                        current_node->data->gender = new_gender;
                        break;
                    case 5:
                        if (new_salaty[0] == '*')
                            break;
                        
                        double d_salary = atof(new_salaty);
                        current_node->data->salary = d_salary;
                        break;
                    default:
                        break;
                }
            }
        }
        current_node = current_node->p_next;
    }
}


void delele_liver_from_list(T_node** root, char* surname, char* name, char* patronymic, char* birth_date, char gender, char* salaty)
{
    T_node* current_node = *root;
    T_node* prev_node = NULL;
    while (current_node != NULL)
    {
        if (copmare_livers(current_node->data, surname, name, patronymic, birth_date, gender, salaty))
        {
            T_node* tmp_node = current_node;
            //Удалаем корень
            if (prev_node == NULL)
            {
                T_node* next_node = (*root)->p_next;
                clear_node(root);
                *root = next_node;
                current_node = *root;
            }
            else
            {
                //Удаляем последний
                if (tmp_node->p_next == NULL)
                {
                    clear_node(&current_node);
                    prev_node->p_next = NULL;
                    break;
                }
                //Удаляем из середины
                else
                {
                    T_node* next_node = current_node->p_next;
                    clear_node(&current_node);
                    prev_node->p_next = next_node;
                    current_node = next_node;
                }
            }
        }
        else
        {
            prev_node = current_node;
            current_node = current_node->p_next;
        }
    }
}


enum status_codes scan_from_file(FILE* file_input, T_node** root)
{
    enum status_codes res = fsc_ok;
    
    char surname[BUFFER_SIZE];
    char name[BUFFER_SIZE];
    char patronymic[BUFFER_SIZE];
    char birth_date[BUFFER_SIZE];
    char gender;
    double salary;
    
    int count = 0;
    bool root_is_ready = false;
    T_node* current_node;
    while (!(feof(file_input)))
    {
        count = fscanf(file_input, "%255s %255s %255s %255s %c %lf", surname, name, patronymic, birth_date, &gender, &salary);
        
        if (count != 6)
        {
            res = fsc_invalid_file;
            break;
        }
        
        T_liver* liver;
        struct tm tm;
        memset(&tm, 0, sizeof(struct tm));
        strptime(birth_date, "%d:%m:%Y", &tm);
        res = create_liver(&liver, surname, name, patronymic, mktime(&tm), gender, salary);
        if (res != fsc_ok)
            break;
        
        if (!(root_is_ready))
        {
            create_node(root, liver);
            root_is_ready = true;
        }
        else
        {
            create_node(&current_node, liver);
            add_node_to_list(root, current_node);
        }
    }
    
    fclose(file_input);
    return res;
}

void print_list_in_file(FILE* file_output, T_node* root)
{
    T_node* current_node =  root;
    while (current_node != NULL)
    {
        fprintf(file_output, "%s %s %s ", current_node->data->surname->str, current_node->data->name->str, current_node->data->patronymic->str);
        struct tm tm = *localtime(&(current_node->data->birth_date));
        fprintf(file_output, "%d:%d:%d %c %lf\n", tm.tm_mday, tm.tm_mon, tm.tm_year, current_node->data->gender, current_node->data->salary);
        
        current_node = current_node->p_next;
    }
    fclose(file_output);
}

typedef struct
{
    T_node** data;
    int len;
    int capasity;
}T_undo_list;


enum status_codes create_undo_list(T_undo_list** undo_list)
{
    enum status_codes res = fsc_ok;
    
    *undo_list = malloc(sizeof(undo_list));
    if (*undo_list == NULL)
    {
        res = fsc_memory_error_detected;
        return res;
    }
    
    (*undo_list)->len = 0;
    (*undo_list)->data = malloc(sizeof(T_node*) * (*undo_list)->capasity);
    if ((*undo_list)->data == NULL)
    {
        free(*undo_list);
        return fsc_memory_error_detected;
    }
    
    (*undo_list)->capasity = 0;
    return fsc_ok;
}

enum status_codes push_to_undo_list(T_undo_list** undo_list, T_node* root)
{
    static int number_action = 0;
    ++number_action;

    if ((*undo_list)->len == (*undo_list)->capasity)
    {
        ((*undo_list)->capasity) = (number_action + 1) / 2;
        T_node** tmp_p = realloc((*undo_list)->data, (sizeof(T_node*) * (*undo_list)->capasity));
        if (tmp_p == NULL)
        {
            return fsc_memory_error_detected;
        }
        (*undo_list)->data = tmp_p;
        
        
        if ((*undo_list)->len == (*undo_list)->capasity)
        {
            clear_list(&(*undo_list)->data[0]);
            memcpy(&(*undo_list)->data[1], &(*undo_list)->data[0], ((*undo_list)->capasity - 1) * sizeof(T_node *));
            (*undo_list)->len -= 1;
        }
        
        {
            ((*undo_list)->data)[(*undo_list)->len] = root;
            ++((*undo_list)->len);
        }
        
        return fsc_ok;
    }
    
    ((*undo_list)->data)[(*undo_list)->len] = root;
    ++((*undo_list)->len);
    return fsc_ok;
}

enum status_codes pop_undo_list(T_undo_list* undo_list, T_node** root)
{
    if (undo_list->len > 0)
    {
        int i = undo_list->len - 1;
        clear_list(root);
        *root = (undo_list->data)[i];
        undo_list->len -= 1;

        return fsc_ok;
    }
    else
        return fsc_error_undo;
}

int main(int argc, const char * argv[])
{
    enum status_codes res = fsc_ok;
    
    T_node* root = NULL;
    FILE* file_input = fopen(argv[1], "r");
    if (file_input == NULL)
        res = fsc_file_is_not_found;
    
    if (res == fsc_ok)
        res = scan_from_file(file_input, &root);
    
    if (res == fsc_ok)
    {
        T_undo_list* undo_list = NULL;
        res = create_undo_list(&undo_list);
    }
    
    T_undo_list* undo_list;
    res = create_undo_list(&undo_list);
    
    if (res == fsc_ok)
    {
        bool end_appl = false;
        while (!end_appl)
        {
            printf("Меню:\n0 - Печать списка\n1 - Поиск жителя с заданными параметрами\n2 - Изменение существующего жителя списка\n3 - удаление жителя\n4 - Добавление жителя\n5 - Выгрузка данных из списка в файл\n6 - Отмнить последнюю модификацию\n7 - Завершить работу\n");
            int p = 0;
            scanf("%d", &p);
            switch (p)
            {
                    //Печать списка
                case 0:
                    print_list(root);
                    break;

                    //Поиск жителя с заданными параметрами
                case 1:
                {
                    char surname[BUFFER_SIZE];
                    char name[BUFFER_SIZE];
                    char patronymic[BUFFER_SIZE];
                    char birth_date[BUFFER_SIZE];
                    char gender;
                    char salary[BUFFER_SIZE];
                    printf("Введите искомого жителя в формате:\n [фамилия имя отчество дата_рождения пол зарплата]\n");
                    
                    scanf("%s %s %s %s %c %s", surname, name, patronymic, birth_date, &gender, salary);
                    
                    printf("Найденные жители:\n");
                    T_node* root_2 = NULL;
                    res = find_livers(&root_2, root, surname, name, patronymic, birth_date, gender, salary);
                    if (res == fsc_ok)
                    {
                        print_list(root_2);
                        clear_list(&root_2);
                    }
                    break;
                }
                    //Изменение существующего жителя списка
                case 2:
                {
                    T_node* new_root;
                    copy_list(&new_root, root);
                    res = push_to_undo_list(&undo_list, new_root);
                    if (res != fsc_ok)
                    {
                        //Очистка
                        break;
                    }
                    
                    char surname[BUFFER_SIZE];
                    char name[BUFFER_SIZE];
                    char patronymic[BUFFER_SIZE];
                    char birth_date[BUFFER_SIZE];
                    char gender;
                    char salary[BUFFER_SIZE];
                    printf("Введите изменяемого жителя в формате: [фамилия имя отчество дата_рождения пол зарплата]\n");
                    scanf("%s %s %s %s %c %s", surname, name, patronymic, birth_date, &gender, salary);
                    
                    char new_surname[BUFFER_SIZE];
                    char new_name[BUFFER_SIZE];
                    char new_patronymic[BUFFER_SIZE];
                    char new_birth_date[BUFFER_SIZE];
                    char new_gender;
                    char new_salary[BUFFER_SIZE];
                    printf("Введите замену в формате: [фамилия имя отчество дата_рождения пол зарплата]\n");
                    scanf("%s %s %s %s %c %s", new_surname, new_name, new_patronymic, new_birth_date, &new_gender, new_salary);
                    change_nodes_in_list(root, surname, name, patronymic, birth_date, gender, salary, new_surname, new_name, new_patronymic, new_birth_date, new_gender, new_salary);
                    print_list(root);
                    break;
                }
                    //удаление жителя
                case 3:
                {
                    T_node* new_root = NULL;
                    copy_list(&new_root, root);
                    res = push_to_undo_list(&undo_list, new_root);
                    if (res != fsc_ok)
                    {
                        end_appl = true;
                        break;
                    }
                    
                    char surname[BUFFER_SIZE];
                    char name[BUFFER_SIZE];
                    char patronymic[BUFFER_SIZE];
                    char birth_date[BUFFER_SIZE];
                    char gender;
                    char salary[BUFFER_SIZE];
                    printf("Введите удаляемого жителя в формате: [фамилия имя отчество дата_рождения пол зарплата]\n");
                    scanf("%s %s %s %s %c %s", surname, name, patronymic, birth_date, &gender, salary);
                    delele_liver_from_list(&root, surname, name, patronymic, birth_date, gender, salary);
                    print_list(root);
                    break;
                }
                    //Добавление жителя
                case 4:
                {
                    T_node* new_root;
                    copy_list(&new_root, root);
                    res = push_to_undo_list(&undo_list, new_root);
                    if (res != fsc_ok)
                    {
                        end_appl = true;
                        break;
                    }
                    
                    char surname[BUFFER_SIZE];
                    char name[BUFFER_SIZE];
                    char patronymic[BUFFER_SIZE];
                    char birth_date[BUFFER_SIZE];
                    char gender;
                    char salary[BUFFER_SIZE];
                    
                    printf("Введите добавляемого жителя в формате: [фамилия имя отчество дата_рождения пол зарплата]\n");
                    scanf("%s %s %s %s %c %s", surname, name, patronymic, birth_date, &gender, salary);
                    T_node* current_node;
                    
                    T_liver* liver;
                    struct tm tm;
                    memset(&tm, 0, sizeof(struct tm));
                    strptime(birth_date, "%d:%m:%Y", &tm);
                    res = create_liver(&liver, surname, name, patronymic, mktime(&tm), gender, atof(salary));
                    if (res != fsc_ok)
                    {
                        end_appl = true;
                        break;
                    }

                    res = create_node(&current_node, liver);
                    if (res != fsc_ok)
                    {
                        end_appl = true;
                        break;
                    }
                    
                    add_node_to_list(&root, current_node);
                    printf("Новый список:\n");
                    print_list(root);
                    break;
                }
                    //Выгрузка данных из списка в файл
                case 5:
                {
                    FILE* file_output = fopen(argv[2], "w");
                    if (file_output == NULL)
                    {
                        res = fsc_file_is_not_found;
                        break;
                    }
                    print_list_in_file(file_output, root);
                    break;
                }
                    //Отмнить последнюю модификацию
                case 6:
                {
                    res = pop_undo_list(undo_list, &root);
                    if (res != fsc_ok)
                        printf("Данных для восстановления нет\n");
                    break;
                }
                    //Завершить работу
                case 7:
                {
                    end_appl = true;
                    break;
                }
                default:
                    break;
            }
        }
        
        // Очистка памяти
        for (int i = 0; i < undo_list->len; ++i)
            clear_list(&((undo_list->data)[i]));
        free(undo_list->data);
        free(undo_list);
        clear_list(&root);
    }

    /*
     enum status_codes
     {
         fsc_error_undo,
         fsc_invalid_file,
     };
     */
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
        case fsc_file_is_empty:
            printf("File is empty\n");
            break;
        case fsc_error_undo:
            printf("Undo error detected\n");
            break;
        case fsc_invalid_file:
            printf("Involid file detected\n");
            break;
        default:
            break;
    }
    return res == fsc_ok ? 0 : 1;
}

/*
 Ivanov Pavel Ivanovich 01:01:2002 M 45000
 Sergeev Andrey Petrovich 05:05:2002 M 50000
 Smirnov Nikolay Andreevich 02:02:2002 M 45000
 Kozlova Ulyana Alrxandrovna 03:03:2002 W 48000
 Noskova Olga Pavlovna 04:04:2002 W 50000
 */
