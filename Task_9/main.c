//
//  main.c
//  9
//
//  Created by Лена on 22.11.2023.
//

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 128

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
    int id;
    int counter;
    bool mask[BUFFER_SIZE];
    int next_id[BUFFER_SIZE];
    void* p_next[BUFFER_SIZE];
}T_node;

int create_id(void)
{
    static int id_counter = 0;
    ++id_counter;
    return id_counter;
}

enum status_codes create_node(T_node** node, int tmp_id)
{
    *node = malloc(sizeof(T_node));
    if (*node == NULL)
        return fsc_memory_error_detected;
    
    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        ((*node)->mask)[i] = false;
        ((*node)->p_next)[i] = NULL;
        ((*node)->next_id)[i] = 0;
    }
    
    (*node)->counter = 0;
    (*node)->id = create_id();
    return fsc_ok;
}

enum status_codes add_char_to_node(char data, T_node* node, T_node** next_node, int tmp_id)
{
    enum status_codes res = fsc_ok;
    
    int i = data;
    
    if (!(node->mask)[i])
    {
        (node->mask)[i] = true;
        
        res = create_node(next_node, tmp_id);
        if (res != fsc_ok)
            return res;
        
    }
    (node->p_next)[i] = *next_node;
    return fsc_ok;
}

enum status_codes add_word_to_tree(T_node* root, char* word, int tmp_id)
{
    enum status_codes res = fsc_ok;
    T_node* current_node = root;

    while (*word != 0)
    {
        res = add_char_to_node (*word, current_node,&(current_node->p_next), tmp_id);
        if (res != fsc_ok)
            break;
        
        current_node = (current_node->p_next)[*word];
        ++word;
    }
    
    current_node->counter += 1;
    return res;
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int get_depth(T_node* current_node, int depth)
{
    int res = depth;
    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        if ((current_node->mask)[i])
            res = max(res, get_depth((current_node->p_next)[i], depth + 1));
    }
    return res;
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

void print_tree(T_node* current_node, int count)
{
    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        if ((current_node->mask)[i])
        {
            for (int j = 0; j < count; ++j)
                printf(" ");
            printf("%c %d %d\n", i, ((T_node*)(current_node->p_next)[i])->counter, ((T_node*)(current_node->p_next)[i])->id);
            
            print_tree((current_node->p_next)[i], count + 1);
        }
    }
}

void print_word_count(T_node* current_node, char * const buffer, char* p_current)
{
    if (current_node->counter != 0)
    {
        *(p_current) = 0;
        printf("%s %d\n", buffer, current_node->counter);
    }
    
    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        if (current_node->mask[i])
        {
            *p_current = i;
            print_word_count((current_node->p_next)[i], buffer, p_current + 1);
        }
    }
}

int find_word_in_tree(T_node* current_node, char* word)
{
    while (*word != 0)
    {
        if ((current_node->mask)[*word])
        {
            current_node = (current_node->p_next)[*word];
            ++word;
        }
        else
            return  0;
    }
    
    return current_node->counter;
}

typedef struct
{
    char buffer[256];
    int count;
}T_word_with_count;


void find_often_words(T_word_with_count* res_buffer,int n, T_node* current_node, char * const buffer, char* p_current)
{
    if (current_node->counter != 0)
    {
        *p_current = 0;
        for (int j = 0; j < n; ++j)
        {
            if (current_node->counter > (res_buffer[j]).count)
            {
                strcpy((res_buffer[j]).buffer, buffer);
                (res_buffer[j]).count = current_node->counter;
                break;
            }
        }
    }
    
    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        if (current_node->mask[i])
        {
            *p_current = i;
            find_often_words(res_buffer, n, (current_node->p_next)[i], buffer, p_current + 1);
        }
    }
}

void find_shortest_and_longest(T_word_with_count* res_buffer_short, T_word_with_count* res_buffer_big, T_node* current_node, char * const buffer, char* p_current)
{
    if (current_node->counter != 0)
    {
        *p_current = 0;
        int buffer_len = (int)strlen(buffer);
        if (res_buffer_big->count < buffer_len)
        {
            res_buffer_big->count = buffer_len;
            strcpy(res_buffer_big->buffer, buffer);
        }
        
        if (res_buffer_short->count > buffer_len)
        {
            res_buffer_short->count = buffer_len;
            strcpy(res_buffer_short->buffer, buffer);
        }
    }
    
    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        if (current_node->mask[i])
        {
            *p_current = i;
            find_shortest_and_longest(res_buffer_short, res_buffer_big, (current_node->p_next)[i], buffer, p_current + 1);
        }
    }
}

void print_tree_in_file(FILE* file_output, T_node* current_node)
{
    fprintf(file_output, "%d %d", current_node->id, current_node->counter);
    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        if ((current_node->mask)[i])
        {
            fprintf(file_output, "[%c]", i);
            fprintf(file_output, "%d", ((T_node*)((current_node->p_next)[i]))->id);
        }
    }
    fprintf(file_output, "\n");
    
    for (int i = 0; i < BUFFER_SIZE; ++i)
        if ((current_node->mask)[i])
            print_tree_in_file(file_output, ((T_node*)(current_node->p_next)[i]));
}

//Восстановление дерева из файла
enum status_codes create_nodes_list(T_node*** nodes_list, int* capacity, int* len, FILE* file)
{
    *capacity = 5;
    *nodes_list = malloc(sizeof(T_node*) * (*capacity));
    if (*nodes_list == NULL)
        return fsc_memory_error_detected;
 
    int col = 0;
    *len = 0;
    
    while(true)
    {
        T_node *node;
        create_node(&node, 0);
        col = fscanf(file, "%d %d", &(node->id), &(node->counter));
        if (col != 2)
            break;

        while(true)
        {
            char mask_code;
            int next_id;
            col = fscanf(file, "[%c]%d", &mask_code, &next_id);
            if (col > 0)
            {
                node->mask[mask_code] = true;
                node->next_id[mask_code] = next_id;
            }
            else
                break;
        }
        (*nodes_list)[*len] = node;
        ++(*len);
        if ((*len) == (*capacity))
        {
            (*capacity) *= 2;
            T_node **new_node_list = realloc(*nodes_list, sizeof(T_node*) * (*capacity));
            if (new_node_list == NULL)
            {
                for(int i = 0; i < (*len); ++i)
                    free((*nodes_list)[i]);
                free(*nodes_list);
                *nodes_list = NULL;
                return fsc_memory_error_detected;
            }
            *nodes_list = new_node_list;
        }
    }

    for(int i = 0; i < (*len); ++i)
    {
        T_node *node = (*nodes_list)[i];
        for(int mask = 0; mask < BUFFER_SIZE; ++mask)
        {
            if (node->mask[mask])
            {
                for(int t = i + 1; t < (*len); ++t)
                {
                    if ((*nodes_list)[t]->id == node->next_id[mask])
                    {
                        node->p_next[mask] = (*nodes_list)[t];
                        break;
                    }
                }
            }
        }
    }
    return fsc_ok;
}

void clear_tree(T_node** current_node)
{
    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        if (((*current_node)->mask)[i])
        {
            clear_tree((T_node**)&((*current_node)->p_next[i]));
        }
    }
    free(*current_node);
    *current_node = NULL;
}

int main(int argc, const char * argv[])
{
    enum status_codes res = fsc_ok;
    
    FILE* file_input = fopen(argv[1], "rb");
    if (file_input == NULL)
        res = fsc_memory_error_detected;
    
    int ind = 0;
    char separators_list[BUFFER_SIZE];
    if (res == fsc_ok)
    {
        for (int i = 2; i < argc - 1; ++i)
        {
            separators_list[ind] = *(char*)argv[i];
            if (ind + 1 == BUFFER_SIZE)
            {
                res = fsc_overflow;
                break;
            }
        }
    }
    
    int tmp_id;
    
    if (res == fsc_ok)
    {
        T_node* root;
        res = create_node(&root, tmp_id);
        
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
                        add_word_to_tree(root, buffer, tmp_id);
                        word_is_ready = false;
                        i = 0;
                    }
                }
            }
            
            if (i > 0)
            {
                buffer[i] = 0;
                add_word_to_tree(root, buffer, tmp_id);
            }
        }
    
    
        bool flag = false;
        T_node** nodes_list = NULL;
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
                //Сколько раз кажное слово встрчается в файле
                case 2:
                    print_word_count(root, word_buffer, word_buffer);
                    break;
                case 3://Сколько раз заданное слово встречается в файле
                    printf("Введите слово:\n");
                    scanf("%s", word_buffer);
                    printf("%d\n", find_word_in_tree(root, word_buffer));
                    break;
                case 4://Вывод первых n часто встречающихся слов
                {
                    int n;
                    printf("Введите n:\n");
                    scanf("%d", &n);
                    
                    T_word_with_count* res_buffer = malloc(sizeof(T_word_with_count) * n);
                    if (res_buffer == NULL)
                        res = fsc_memory_error_detected;
                    
                    if (res != fsc_ok)
                        break;
                    
                    for (int i = 0; i < n; ++i)
                        res_buffer[i].count = 0;
                    
                    find_often_words(res_buffer, n, root, word_buffer, word_buffer);
                    for (int i = 0; i < n; ++i)
                        printf("%s\n", res_buffer[i].buffer);
                    
                    free(res_buffer);
                    break;
                }
                case 5://Поиск и вывод самого длинного и самого короткого слова
                {
                    T_word_with_count* res_buffer_short = malloc(sizeof(T_word_with_count));
                    if (res_buffer_short == NULL)
                    {
                        res = fsc_memory_error_detected;
                        break;
                    }
                    
                    T_word_with_count* res_buffer_big = malloc(sizeof(T_word_with_count));
                    if (res_buffer_big == NULL)
                    {
                        free(res_buffer_short);
                        res = fsc_memory_error_detected;
                        break;
                    }
                    
                    res_buffer_big->count = 0;
                    res_buffer_short->count = BUFFER_SIZE;
                    find_shortest_and_longest(res_buffer_short, res_buffer_big, root, word_buffer, word_buffer);
                    
                    printf("%s %s\n", res_buffer_short->buffer, res_buffer_big->buffer);
                    free(res_buffer_short);
                    free(res_buffer_big);
                    break;
                }
                case 6://Поиск глубины дерева
                {
                    printf("Глубина дерева: %d\n", get_depth(root, 0));
                    break;
                }
                case 7://Сохранение дерева в файл
                {
                    FILE* file_output = NULL;
                    file_output = fopen(argv[argc - 1], "wt");
                    if (file_output == NULL)
                    {
                        res = fsc_memory_error_detected;
                        break;
                    }
                    
                    print_tree_in_file(file_output, root);
                    fclose(file_output);
                    break;
                }
                case 8://Восстановление дерева из файла
                {
                    FILE* file_output = fopen(argv[argc - 1], "rt");
                    int len = 0;
                    int capacity = 0;
                    res = create_nodes_list(&nodes_list, &capacity, &len, file_output);
                    print_tree(*nodes_list, 0);
                    
                    if (res == fsc_ok)
                        clear_tree(nodes_list);
                    
                    break;
                }
                case 9:
                    flag = true;
                    break;
                default:
                    res = fsc_invalid_parameter;
                    break;
            }
            
            if (res != fsc_ok)
                break;
            
            if (flag)
                break;
        }
        if ((nodes_list != NULL) && (*nodes_list != NULL))
            clear_tree(nodes_list);
        
        clear_tree(&root);
        fclose(file_input);
    }
    
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

