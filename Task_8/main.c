//
//  main.c
//  8
//
//  Created by Лена on 02.01.2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include "status_code.h"
#include "polinome.h"


bool custom_getc(FILE* file, char* c)
{
    static int mode = 0;
    
    if((*c = getc(file)) == EOF)
    {
        *c = '!';
        return true;
    }
    
    switch (*c)
    {
        case '%':
            mode = 1;
            break;
        case '[':
            mode = 2;
            break;
        case '\n':
            if (mode == 1)
                mode = 0;
            break;
        case ']':
            mode = 0;
            return false;
            break;
        default:
            break;
    }
    
    return (mode == 0);
}
   
int get_command_num(const char * command_name)
{
    char* command_list[] = {"Add", "Sub", "Mult", "Div", "Mod", "Eval", "Diff", "Cmps"};
    for (int i = 0; i < 8; ++i)
    {
        if (strcmp(command_name, command_list[i]) == 0)
            return i;
    }
    return 8;
}

void print_res(T_node *a, T_node *b, char sign, T_node *res)
{
    print_polinom(a);
    printf(" %c ", sign);
    print_polinom(b);
    printf(" = ");
    print_polinom(res);
    printf("\n");
}

int main(int argc, const char * argv[])
{
    enum status_codes res = fsc_ok;
    
    if (argc != 2)
        res = fsc_invalid_parameter;
    
    FILE* file_input = NULL;
    if (res == fsc_ok)
    {
        file_input = fopen(argv[1], "r");
        if (file_input == NULL)
            res = fsc_file_is_not_found;
    }
    
    T_node* adder = NULL;
    if (res == fsc_ok)
        res = add_to_polinom(&adder, 1, 0);
    
    char command_name[BUFSIZ];
    char polinom_buffer[BUFSIZ];
    if (res == fsc_ok)
    {
        char c;
        char* p_command_name = command_name;
        char* p_polinom_buffer = polinom_buffer;
        
        T_node* head_1 = NULL;
        T_node* head_2 = NULL;
        bool first_polinom_is_ready = false;
        
        bool comment_flag = false;
        while(true)
        {
            comment_flag = custom_getc(file_input, &c);
            if (comment_flag && (c == '!'))
                break;
            if (!(comment_flag))
                continue;
            
            if (res != fsc_ok)
                break;
            
            switch (c)
            {
                case '(':
                    *p_command_name = 0;
                    p_command_name = command_name;
                    while(true)
                    {
                        comment_flag = custom_getc(file_input, &c);
                        if (!(comment_flag))
                            continue;
                        
                        if (c == ',' || c == ')')
                        {
                            
                            switch (c)
                            {
                                case ',':
                                    *p_polinom_buffer = 0;
                                    res = read_polinom(&head_1, polinom_buffer);
                                    first_polinom_is_ready = true;
                                    p_polinom_buffer = polinom_buffer;
                                    comment_flag = custom_getc(file_input, &c);
                                    if (!(comment_flag))
                                        continue;
                                    break;
                                case ')':
                                    *p_polinom_buffer = 0;
                                    if (first_polinom_is_ready)
                                        res = read_polinom(&head_2, polinom_buffer);
                                    else
                                    {
                                        res = read_polinom(&head_1, polinom_buffer);
                                        first_polinom_is_ready = true;
                                    }
                                    p_polinom_buffer = polinom_buffer;
                                default:
                                    break;
                            }
                            
                            if (c == ')')
                                break;
                        }
                        
                        *p_polinom_buffer = c;
                        ++p_polinom_buffer;
                    }
                    break;
                case ';':
                {
                    int command_num = get_command_num(command_name);
                    T_node* res_head = NULL;
                    
                    switch (command_num)
                    {
                        case 0:
                        {
                            T_node *a, *b;
                            if (head_2 == NULL)
                            {
                                a = adder;
                                b = head_1;
                            }
                            else
                            {
                                a = head_1;
                                b = head_2;
                            }
                            
                            res = add_polinoms(&res_head, a, b);
                            
                            if (res == fsc_ok)
                            {
                                print_res(a, b, '+', res_head);
                                
                                clear_polinom(&adder);
                                res = copy_polinom(&adder, res_head);
                            }
                                                        
                            break;
                        }
                        case 1:
                        {
                            T_node *a, *b;
                            if (head_2 == NULL)
                            {
                                a = adder;
                                b = head_1;
                            }
                            else
                            {
                                a = head_1;
                                b = head_2;
                            }
                            
                            res = sub_polinoms(&res_head, a, b);
                                                   
                            if (res == fsc_ok)
                            {
                                print_res(a, b, '-', res_head);
                                
                                clear_polinom(&adder);
                                res = copy_polinom(&adder, res_head);
                            }
                                                        
                            break;
                        }
                        case 2:
                        {
                            T_node *a, *b;
                            if (head_2 == NULL)
                            {
                                a = adder;
                                b = head_1;
                            }
                            else
                            {
                                a = head_1;
                                b = head_2;
                            }
                            
                            res = mult_polinoms(&res_head, a, b);
                                                   
                            if (res == fsc_ok)
                            {
                                print_res(a, b, '*', res_head);
                                
                                clear_polinom(&adder);
                                res = copy_polinom(&adder, res_head);
                            }
                                                        
                            break;
                        }
                        case 3:
                        {
                            T_node *a, *b;
                            if (head_2 == NULL)
                            {
                                a = adder;
                                b = head_1;
                            }
                            else
                            {
                                a = head_1;
                                b = head_2;
                            }
                            
                            res = div_polinoms(&res_head, a, b);
                            
                            if (res == fsc_ok)
                            {
                                print_polinom(a);
                                printf(" div ");
                                print_polinom(b);
                                printf(" = ");
                                print_polinom(res_head);
                                printf("\n");
                                clear_polinom(&adder);
                                res = copy_polinom(&adder, res_head);
                                
                            }
                            
                            break;
                        }
                        case 4:
                        {
                            if (head_2 == NULL)
                            {
                                res = mod_polinoms(&res_head, adder, head_1);
                                print_polinom(adder);
                                printf(" mod ");
                                print_polinom(head_1);
                                printf(" = ");
                            }
                            else
                            {
                                res = mod_polinoms(&res_head, head_1, head_2);
                                print_polinom(head_1);
                                printf(" mod ");
                                print_polinom(head_2);
                                printf(" = ");
                            }
                            
                            if (res == fsc_ok)
                            {
                                clear_polinom(&adder);
                                res = copy_polinom(&adder, res_head);
                            }
                            
                            if (res == fsc_ok)
                            {
                                print_polinom(res_head);
                                printf("\n");
                            }
                            break;
                        }
                        case 5:
                        {
                            int res_value = 0;
                            res = eval_polinom(&res_value, adder, head_1->coeff);
                            
                            if (res == fsc_ok)
                            {
                                print_polinom(adder);
                                printf(" in point %d: %d\n", head_1->coeff, res_value);
                            }
                            break;
                        }
                        case 6:
                        {
                            res = diff_polinom(&res_head, head_1);
                            
                            if (res == fsc_ok)
                            {
                                clear_polinom(&adder);
                                res = copy_polinom(&adder, res_head);
                            }
                            
                            if (res == fsc_ok)
                            {
                                print_polinom(head_1);
                                printf(" -> ");
                                print_polinom(res_head);
                                printf("\n");
                            }
                        }
                        case 7:
                        {
                            if (head_2 == NULL)
                            {
                                res = cmps_polinoms(&res_head, adder, head_1);
                                printf("g: )");
                                print_polinom(adder);
                                printf(" f: ");
                                print_polinom(head_1);
                            }
                            else
                            {
                                res = cmps_polinoms(&res_head, head_1, head_2);
                                printf("g: )");
                                print_polinom(head_1);
                                printf(" f: ");
                                print_polinom(head_2);
                            }
                            
                            if (res == fsc_ok)
                            {
                                clear_polinom(&adder);
                                res = copy_polinom(&adder, res_head);
                            }
                            
                            if (res == fsc_ok)
                            {
                                printf(" g(f): ");
                                print_polinom(res_head);
                                printf("\n");
                            }
                            break;
                        }
                        default:
                            break;
                    }
                    
                    if (res_head != NULL)
                        clear_polinom(&res_head);
                    if (head_1 != NULL)
                        clear_polinom(&head_1);
                    if (head_2 != NULL)
                        clear_polinom(&head_2);
                    first_polinom_is_ready = false;
                    break;
                }
                case '\n':
                    break;
                default:
                    *p_command_name = c;
                    ++p_command_name;
                    break;
            }
        }
        
    }
    
    if (file_input != NULL)
        fclose(file_input);
    
    
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

/*
 2x^2 -1x^1 2x^0  + -1x^2 3x^1 -1x^0  = 1x^2 2x^1 1x^0
 1x^5  div 1x^2 -1x^0  = 1x^3 1x^1
 1x^3 1x^1  in point 1: 2
 */
