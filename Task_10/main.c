//
//  main.c
//  10
//
//  Created by Лена on 27.11.2023.
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
    fsc_file_is_finish,
    fsc_str_is_finish,
};

typedef struct
{
    bool mask[BUFFER_SIZE];
    void* p_next[BUFFER_SIZE];
}T_node;

enum status_codes create_node(T_node** node)
{
    *node = malloc(sizeof(T_node));
    if (*node == NULL)
        return fsc_memory_error_detected;
    
    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        ((*node)->mask)[i] = false;
        ((*node)->p_next)[i] = NULL;
    }

    return fsc_ok;
}

enum status_codes add_char_to_node(char c, T_node * node)
{
    enum status_codes res = fsc_ok;
    if (!(node->mask)[c])
    {
        (node->mask)[c] = true;
        res = create_node((T_node **)&node->p_next[c]);
        if (res != fsc_ok)
            return res;
        
    }
    return fsc_ok;
}

void print_tree(T_node* current_node, int count)
{
    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        if ((current_node->mask)[i])
        {
            for (int j = 0; j < count; ++j)
                printf(" ");
            printf("%c\n", i);
            
            print_tree((current_node->p_next)[i], count + 1);
        }
    }
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

enum status_codes add_char_to_tree(T_node* current_node, FILE* file_input)
{
    enum status_codes res = fsc_ok;
    char c;
    char prev_c = 0;
    while((c = getc(file_input)) != EOF)
    {
        switch (c)
        {
            case '(':
                res = add_char_to_tree((T_node *)current_node->p_next[prev_c], file_input);
                break;
            case ')':
                return res;
                break;
            case ' ':
                break;
            case ',':
                break;
            case '\n':
                res = fsc_str_is_finish;
                break;
            default:
                prev_c = c;
                res = add_char_to_node(c, current_node);
                break;
        }
        
        if (res != fsc_ok)
            return res;
    }
    
    return fsc_file_is_finish;
}

void print_tree_in_file(FILE* file_output, T_node* current_node, int count)
{
    for (int i = 0; i < BUFFER_SIZE; ++i)
    {
        if ((current_node->mask)[i])
        {
            for (int j = 0; j < count; ++j)
                fprintf(file_output, " ");
            fprintf(file_output, "%c\n", i);
            
            print_tree_in_file(file_output, (current_node->p_next)[i], count + 1);
        }
    }
}

int main(int argc, const char * argv[])
{
    enum status_codes res = fsc_ok;
    
    if (argc < 3)
        res = fsc_invalid_parameter;
    
    FILE* file = NULL;
    if (res == fsc_ok)
    {
        file = fopen(argv[1], "r");
        if (file == NULL)
            res = fsc_file_is_not_found;
    }
    
    FILE* file_output = NULL;
    if (res == fsc_ok)
    {
        file_output = fopen(argv[2], "w");
        if (file_output == NULL)
        {
            res = fsc_file_is_not_found;
            fclose(file);
        }
    }
    
    T_node* root = NULL;
    if (res == fsc_ok)
    {
        while(true)
        {
            res = create_node(&root);
            
            if (res == fsc_ok)
                res = add_char_to_tree(root, file);
            else
                break;
            
            printf("\n");
            print_tree(root, 0);
            print_tree_in_file(file_output, root, 0);
            fprintf(file_output, "\n");
            
            clear_tree(&root);
            if (res == fsc_file_is_finish)
                break;
        }
    }
    
    fclose(file);
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
            break;
    }
    
    return res == fsc_ok ? 0 : 1;
}
