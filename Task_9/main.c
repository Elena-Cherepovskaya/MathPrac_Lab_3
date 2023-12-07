//
//  main.c
//  9_str
//
//  Created by Лена on 03.12.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define BUFFER_SIZE 256

enum status_codes
{
    fsc_ok,
    fsc_overflow,
    fsc_invalid_parameter,
    fsc_memory_error_detected,
    fsc_file_is_not_found,
};

typedef struct
{
    char* str;
    int len;
} string;

enum status_codes create(string** new_string)
{
    *new_string = (string*)malloc(sizeof(string));
    if (*new_string == NULL)
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
    
    (*new_string)->str = (char*)malloc(sizeof(char) * (len + 1));
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
        clear_string(&destptr);
        return fsc_memory_error_detected;
    }
    
    destptr->str = tmp_p;
    
    strcat(destptr->str, srcptr->str);
    destptr->len = new_len;
    
    return fsc_ok;
}

typedef struct
{
    string* data;
    int counter;
    void* p_left;
    void* p_right;
}T_node;

enum status_codes create_node(T_node** node, string* word)
{
    *node = (T_node*)malloc(sizeof(T_node));
    if (*node == NULL)
        return  fsc_memory_error_detected;
    
    (*node)->data = word;
    (*node)->counter = 1;
    (*node)->p_left = NULL;
    (*node)->p_right = NULL;
    return fsc_ok;
}

enum status_codes add_word_to_tree(T_node* root, string* word)
{
    enum status_codes res = fsc_ok;
    
    bool node_is_ready = false;
    T_node* current_node = root;
    while(true)
    {
        if (node_is_ready)
            break;
        
        int c = comp(word, current_node->data);
        if (c != 0)
            c = (c > 0) ? 1 : -1;
        
        switch (c)
        {
            case 1:
                if (current_node->p_right == NULL)
                {
                    res  = create_node((T_node**)&(current_node->p_right), word);
                    node_is_ready = true;
                    break;
                }
                
                current_node = current_node->p_right;
                break;
            case -1:
                if (current_node->p_left == NULL)
                {
                    res = create_node((T_node**)&(current_node->p_left), word);
                    node_is_ready = true;
                    break;
                }
                
                current_node = current_node->p_left;
                break;
            case 0:
                ++current_node->counter;
                node_is_ready = true;
                break;
            default:
                break;
        }
    }
    
    return res;
}

void print_tree(T_node* current_node, int count)
{
    for (int i = 0; i < count; ++i)
        printf(" ");
    printf("%s %d\n", current_node->data->str, current_node->counter);
    
    if (current_node->p_left != NULL)
        print_tree(current_node->p_left, count + 1);
    
    if (current_node->p_right != NULL)
        print_tree(current_node->p_right, count + 1);
}

void clear_tree(T_node** current_node)
{
    if ((*current_node)->p_left != NULL)
        clear_tree((T_node**)&((*current_node)->p_left));
    if ((*current_node)->p_right != NULL)
        clear_tree((T_node**)&((*current_node)->p_right));
    
    clear_string(&((*current_node)->data));
    free(*current_node);
    *current_node = NULL;
}

bool is_separator(char test, char* separators_list)
{
    int len = (int)strlen(separators_list);
    for (int i = 0; i < len; ++i)
    {
        if (test == separators_list[i])
            return true;
    }
    return false;
}

void print_word_with_count(T_node* current_node)
{
    printf("%s %d\n", current_node->data->str, current_node->counter);
    if (current_node->p_left  != NULL)
        print_word_with_count(current_node->p_left);
    if (current_node->p_right != NULL)
        print_word_with_count(current_node->p_right);
}

int find_word_in_tree(T_node* root, string* word)
{
    T_node* current_node = root;
    while (true)
    {
        int c = comp(word, current_node->data);
        if (c != 0)
            c = (c > 0) ? 1 : -1;
        
        switch (c)
        {
            case 0:
                return current_node->counter;
            case 1:
                if (current_node->p_right == NULL)
                    return 0;
                current_node = current_node->p_right;
                break;
            case -1:
                if (current_node->p_left == NULL)
                    return 0;
                current_node = current_node->p_left;
            default:
                break;
        }
    }
}

typedef struct
{
    char buffer[BUFFER_SIZE];
    int count;
}T_word_with_count;


void find_often_words(T_word_with_count* res_buffer, int n, T_node* current_node)
{
    int min_counter = current_node->counter;
    int res_i = 0;
    for (int i = 0; i < n; ++i)
    {
        if (res_buffer[i].count < min_counter)
        {
            min_counter = res_buffer[i].count;
            res_i = i;
        }
    }
    
    if (current_node->counter > min_counter)
    {
        strcpy((res_buffer[res_i]).buffer, current_node->data->str);
        (res_buffer[res_i]).count = current_node->counter;
    }
    
    if (current_node->p_left != NULL)
        find_often_words(res_buffer, n, current_node->p_left);
    if (current_node->p_right != NULL)
        find_often_words(res_buffer, n, current_node->p_right);
}

void find_shortest_and_longest(T_node* current_node, T_word_with_count* short_word, T_word_with_count* long_word)
{
    int len = current_node->data->len;
    if (len > long_word->count)
    {
        long_word->count = len;
        strcpy(long_word->buffer, current_node->data->str);
    }
    
    if (len < short_word->count)
    {
        short_word->count = len;
        strcpy(short_word->buffer, current_node->data->str);
    }
    
    if (current_node->p_left != NULL)
        find_shortest_and_longest(current_node->p_left, short_word, long_word);
    
    if (current_node->p_right != NULL)
        find_shortest_and_longest(current_node->p_right, short_word, long_word);
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int get_depth(T_node* current_node)
{
    int res = 0;
    
    if (current_node->p_left != NULL)
        res = max(res, get_depth(current_node->p_left));
    
    if (current_node->p_right != NULL)
        res = max(res, get_depth(current_node->p_right));
    
    return res + 1;
}

void print_tree_in_file(FILE* file_output, T_node* current_node, int count)
{
    for (int i = 0; i < count; ++i)
        fprintf(file_output, " ");
    fprintf(file_output, "%s %d\n", current_node->data->str, current_node->counter);
    
    if (current_node->p_left != NULL)
        print_tree_in_file(file_output, current_node->p_left, count + 1);
    
    if (current_node->p_right != NULL)
        print_tree_in_file(file_output, current_node->p_right, count + 1);
}


int main(int argc, const char * argv[])
{
    enum status_codes res = fsc_ok;
    bool input_file_is_opened = false;
    
    if (argc < 3)
        res = fsc_invalid_parameter;
    
    FILE* file_input = NULL;
    if (res == fsc_ok)
    {
        file_input = fopen(argv[1], "r");
        if (file_input == NULL)
            res = fsc_file_is_not_found;
        else
            input_file_is_opened = true;
    }
    
    int ind = 0;
    char separators_list[BUFFER_SIZE];
    if (res == fsc_ok)
    {
        for (int i = 0; i < argc - 1; ++i)
        {
            separators_list[ind] = (char)*argv[i];
            if (ind + 1 == BUFFER_SIZE)
            {
                res = fsc_overflow;
                break;
            }
        }
    }
    
    T_node* root = NULL;
    bool root_is_ready = false;
    if (res == fsc_ok)
    {
        int i = 0;
        char buffer[BUFFER_SIZE];
        bool word_is_ready = false;
        char c;
        while ((c = getc(file_input)) != EOF)
        {
            if (!(is_separator(c, separators_list)))
            {
                buffer[i] = c;
                word_is_ready = true;
                if (i + 1 == BUFFER_SIZE)
                {
                    res = fsc_overflow;
                    break;
                }
                ++i;
            }
            else
            {
                if (word_is_ready)
                {
                    buffer[i] = 0;
                    string* str;
                    res = create_from_char(&str, buffer);
                    if (res != fsc_ok)
                        break;
                    if (!(root_is_ready))
                    {
                        root_is_ready = true;
                        res = create_node(&root, str);
                        if (res != fsc_ok)
                            break;
                    }
                    else
                    {
                        res = add_word_to_tree(root, str);
                    }
                    i = 0;
                }
            }
        }
        
        if ((i > 0) && (res == fsc_ok))
        {
            buffer[i] = 0;
            string* str;
            res = create_from_char(&str, buffer);
            
            if (res == fsc_ok)
            {
                if (!(root_is_ready))
                {
                    root_is_ready = true;
                    res = create_node(&root, str);
                }
                else
                    res = add_word_to_tree(root, str);
            }
        }
    }
    
    if (input_file_is_opened)
        fclose(file_input);
    
    if (res == fsc_ok)
    {
        bool flag = false;
        char word_buffer[BUFFER_SIZE];
        int p;
        while (true)
        {
            printf("Выберите пункт меню: \n1 - Вывести дерево на экран\n2 - Сколько раз каждое слово встрчается в файле\n3 - Сколько раз заданное слово встречается в файле\n4 - Вывод первых n часто встречающихся слов\n5 - Поиск и вывод самого длинного и самого короткого слова\n6 - Поиск глубины дерева\n7 - Сохранение дерева в файл\n8 - Восстановление дерева из файла\n9 - Завершить\n");
            scanf("%d", &p);
            switch (p)
            {
                //Вывести дерево на экран
                case 1:
                    print_tree(root, 0);
                    break;
                //Сколько раз каждое слово встречается в файле
                case 2:
                    print_word_with_count(root);
                    break;
                //Сколько раз заданное слово встречается в файле
                case 3:
                    printf("Введите слово:\n");
                    scanf("%255s", word_buffer);
                    string* word_str;
                    res = create_from_char(&word_str, word_buffer);
                    if (res != fsc_ok)
                        break;
                    
                    printf("%d\n", find_word_in_tree(root, word_str));
                    clear_string(&word_str);
                    break;
                //Вывод первых n часто встречающихся слов
                case 4:
                {
                    int n;
                    printf("Введите n:\n");
                    scanf("%d", &n);
                    
                    T_word_with_count* res_buffer = (T_word_with_count*)malloc(sizeof(T_word_with_count) * n);
                    if (res_buffer == NULL)
                        res = fsc_memory_error_detected;
                    
                    if (res != fsc_ok)
                        break;
                    
                    for (int i = 0; i < n; ++i)
                        res_buffer[i].count = 0;
                    
                    find_often_words(res_buffer, n, root);
                    for (int i = 0; i < n; ++i)
                        printf("%s\n", res_buffer[i].buffer);
                    
                    free(res_buffer);
                    break;
                }
                //Поиск и вывод самого длинного и самого короткого слова
                case 5:
                {
                    T_word_with_count short_word;
                    T_word_with_count long_word;
                    short_word.count = BUFFER_SIZE;
                    long_word.count = 0;
                    
                    find_shortest_and_longest(root, &short_word, &long_word);
                    printf("%s %s\n", short_word.buffer, long_word.buffer);
                    break;
                }
                //Поиск глубины дерева
                case 6:
                    printf("%d\n", get_depth(root));
                    break;
                //Сохранение дерева в файл
                case 7:
                {
                    FILE* file_output = NULL;
                    file_output = fopen(argv[argc - 1], "wt");
                    if (file_output == NULL)
                    {
                        res = fsc_memory_error_detected;
                        break;
                    }
                    
                    print_tree_in_file(file_output, root, 0);
                    fclose(file_output);
                    break;
                }
                //Восстановление дерева из файла
                case 8:
                {
                    FILE* file_output = fopen(argv[argc - 1], "r");
                    if (file_output == NULL)
                    {
                        res = fsc_memory_error_detected;
                        break;
                    }
                    
                    T_node* new_root = NULL;
                    int counter = 0;
                    char buffer[BUFFER_SIZE];
                    bool root_is_ready = false;
                    while (!(feof(file_output)))
                    {
                        fscanf(file_output, "%255s %d", buffer, &counter);
                        string* str;
                        res = create_from_char(&str, buffer);
                        if (res != fsc_ok)
                            break;
                        if (!(root_is_ready))
                        {
                            root_is_ready = true;
                            res = create_node(&new_root, str);
                            if (res != fsc_ok)
                                break;
                        }
                        else
                        {
                            res = add_word_to_tree(new_root, str);
                            if (res != fsc_ok)
                                break;
                        }
                    }
                    
                    if (new_root != NULL)
                    {
                        print_tree(new_root, 0);
                        clear_tree(&new_root);
                    }
                    
                    fclose(file_output);
                    break;
                }
                //Заверешение работы
                case 9:
                    flag = true;
                    break;
                default:
                    break;
            }
            
            if (flag)
                break;
        }
    }
    
    if (root != NULL)
        clear_tree(&root);
    
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
