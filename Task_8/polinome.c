//
//  polinome.c
//  8
//
//  Created by Лена on 24.01.2024.
//

#include "polinome.h"
#include "status_code.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

enum status_codes create_node(T_node** head, int coeff, int pow)
{
    if (*head != NULL)
        return fsc_invalid_parameter;
    
    *head = (T_node*)malloc(sizeof(T_node));
    if (*head == NULL)
        return fsc_memory_error_detected;
    
    (*head)->coeff = coeff;
    (*head)->pow = pow;
    (*head)->p_next = NULL;
    
    return fsc_ok;
}

//Добавить в конец
enum status_codes add_to_polinom(T_node** head, int coeff, int pow)
{
    if (*head == NULL)
        return create_node(head, coeff, pow);
    
    T_node* current_node = *head;
    while(current_node->p_next != NULL)
        current_node = current_node->p_next;
    
    return create_node((T_node**)&(current_node->p_next), coeff, pow);
}


void print_polinom(T_node* head)
{
    T_node* current_node = head;
    while(current_node != NULL)
    {
        printf("%dx^%d ", current_node->coeff, current_node->pow);
        current_node = current_node->p_next;
    }
}

enum status_codes clear_polinom(T_node** head)
{
    if (*head == NULL)
        return fsc_invalid_parameter;
    
    T_node* current_node = *head;
    T_node* prev_node = NULL;
    
    while(current_node != NULL)
    {
        prev_node = current_node;
        current_node = current_node->p_next;
        free(prev_node);
    }
    
    *head = NULL;
    return fsc_ok;
}

enum status_codes copy_polinom(T_node** new_head, T_node* head)
{
    enum status_codes res = fsc_ok;
    
    if (*new_head != NULL || head == NULL)
        return fsc_invalid_parameter;
    
    T_node* current_node = head;
    while(current_node != NULL)
    {
        res = add_to_polinom(new_head, current_node->coeff, current_node->pow);
        if (res != fsc_ok)
        {
            clear_polinom(new_head);
            return res;
        }
        
        current_node = current_node->p_next;
    }
    
    return fsc_ok;
}

enum status_codes similar_terms(T_node** head)
{
    if (*head == NULL)
        return fsc_invalid_parameter;
    
    T_node* current_node_1 = *head;
    while(current_node_1 != NULL)
    {
        T_node* current_node_2 = current_node_1->p_next;
        T_node* node_2_prev = current_node_1;
        while(current_node_2 != NULL)
        {
            if (current_node_1->pow == current_node_2->pow)
            {
                current_node_1->coeff += current_node_2->coeff;
                
                node_2_prev->p_next = current_node_2->p_next;
                free(current_node_2);
                current_node_2 = node_2_prev->p_next;
            }
            else
            {
                node_2_prev = current_node_2;
                current_node_2 = current_node_2->p_next;
            }
        }
        current_node_1 = current_node_1->p_next;
    }
    
    return fsc_ok;
}

//SUM
enum status_codes add_polinoms(T_node** res_head, T_node* head_1, T_node* head_2)
{
    enum status_codes res = fsc_ok;
    
    if (*res_head != NULL || head_1 == NULL || head_2 == NULL)
        return fsc_invalid_parameter;
    
    res = copy_polinom(res_head, head_1);
    if (res != fsc_ok)
    {
        clear_polinom(res_head);
        return res;
    }
    
    T_node* current_node = head_2;
    while(current_node != NULL)
    {
        T_node* res_curren_node = *res_head;
        bool sum_is_ready = false;
        
        while(res_curren_node != NULL)
        {
            if (res_curren_node->pow == current_node->pow)
            {
                res_curren_node->coeff += current_node->coeff;
                sum_is_ready = true;
                break;
            }
            res_curren_node = res_curren_node->p_next;
        }
        
        if (!(sum_is_ready))
        {
            res = add_to_polinom(res_head, current_node->coeff, current_node->pow);
            if (res != fsc_ok)
            {
                clear_polinom(res_head);
                return res;
            }
        }
        
        current_node = current_node->p_next;
    }
    
    return fsc_ok;
}

enum status_codes sub_polinoms(T_node** res_head, T_node* head_1, T_node* head_2)
{
    enum status_codes res = fsc_ok;
    
    if (*res_head != NULL || head_1 == NULL || head_2 == NULL)
        return fsc_invalid_parameter;
    
    res = copy_polinom(res_head, head_1);
    if (res != fsc_ok)
    {
        clear_polinom(res_head);
        return res;
    }
    
    T_node* current_node = head_2;
    while(current_node != NULL)
    {
        T_node* res_current_node = *res_head;
        bool sub_is_ready = false;
        
        while(res_current_node != NULL)
        {
            if (res_current_node->pow == current_node->pow)
            {
                res_current_node->coeff -= current_node->coeff;
                sub_is_ready = true;
                break;
            }
            res_current_node = res_current_node->p_next;
        }
        
        if (!(sub_is_ready))
        {
            res = add_to_polinom(res_head, -(current_node->coeff), current_node->pow);
            if (res != fsc_ok)
            {
                clear_polinom(res_head);
                return res;
            }
        }
        
        current_node = current_node->p_next;
    }
    
    return fsc_ok;
}

//Умножение
enum status_codes mult_polinoms(T_node** res_head, T_node* head_1, T_node* head_2)
{
    enum status_codes res = fsc_ok;
    
    if (*res_head != NULL || head_1 == NULL || head_2 == NULL)
        return fsc_invalid_parameter;
    
    T_node* current_node_1 = head_1;
    while(current_node_1 != NULL)
    {
        T_node* current_node_2 = head_2;
        while(current_node_2 != NULL)
        {
            res = add_to_polinom(res_head, current_node_1->coeff * current_node_2->coeff, current_node_1->pow + current_node_2->pow);
            if (res != fsc_ok)
            {
                clear_polinom(res_head);
                return res;
            }
            current_node_2 = current_node_2->p_next;
        }
        
        current_node_1 = current_node_1->p_next;
    }
    
    similar_terms(res_head);
    return fsc_ok;
}

//Добавить в начало
enum status_codes push_in_polinome(T_node** head, int coeff, int pow)
{
    if (*head == NULL)
    {
        *head = malloc(sizeof(T_node));
        if (*head == NULL)
            return fsc_invalid_parameter;
        
        (*head)->coeff = coeff;
        (*head)->pow = pow;
        (*head)->p_next = NULL;
        
        return fsc_ok;
    }
    
    T_node* next_node = *head;
    T_node* new_head = malloc(sizeof(T_node));
    if (new_head == NULL)
        return fsc_invalid_parameter;
    new_head->coeff = coeff;
    new_head->pow = pow;
    new_head->p_next = next_node;
    *head = new_head;
    
    return fsc_ok;
}

int max_pow_in_polinom(T_node* head)
{
    if (head == NULL)
        return 0;
    
    int max_pow = 0;
    T_node* current_node = head;
    while (current_node != NULL)
    {
        max_pow = (current_node->pow > max_pow) ? current_node->pow : max_pow;
        current_node = current_node->p_next;
    }
    
    return max_pow;
}

//Расположить слагаемые по убыванию и дополнить недостающие слагаемые
enum status_codes transform_polinom(T_node** res_head, T_node* head)
{
    enum status_codes res = fsc_ok;
    
    if (*res_head != NULL || head == NULL)
        return fsc_invalid_parameter;
    
    int max_pow = max_pow_in_polinom(head);
    for (int i = 0; i <= max_pow; ++i)
    {
        T_node* current_node = head;
        bool node_is_ready = false;
        while(current_node != NULL)
        {
            if (current_node->pow == i)
            {
                node_is_ready = true;
                res = push_in_polinome(res_head, current_node->coeff, i);
                if (res != fsc_ok)
                {
                    clear_polinom(res_head);
                    return res;
                }
                
                break;
            }
            
            current_node = current_node->p_next;
        }
        
        if (!(node_is_ready))
        {
            res = push_in_polinome(res_head, 0, i);
            if (res != fsc_ok)
            {
                clear_polinom(res_head);
                return res;
            }
        }
    }
    
    return fsc_ok;
}

//Удаление первого слагаемого
enum status_codes pop_from_polinom(T_node** head)
{
    if (*head == NULL)
        return fsc_invalid_parameter;
    
    T_node* tmp_node = *head;
    *head = (*head)->p_next;
    
    free(tmp_node);
    
    return fsc_ok;
}

enum status_codes div_and_mod_polinoms(T_node** res_head_div, T_node** res_head_mod, T_node* head_1, T_node* head_2)
{
    enum status_codes res = fsc_ok;
    
    if (*res_head_div != NULL || *res_head_mod != NULL || head_1 == NULL || head_2 == NULL)
        return fsc_invalid_parameter;
    
    res = copy_polinom(res_head_mod, head_1);
    if (res != fsc_ok)
        return res;
    
    bool flag = true;
    //слагаемое под которое подбирается частное
    T_node* current_node_mod = *res_head_mod;
    while(flag)
    {
        if (current_node_mod->pow < head_2->pow)
            break;
        
        int tmp_coeff = current_node_mod->coeff / head_2->coeff;
        int tmp_pow = current_node_mod->pow - head_2->pow;
        
        T_node* sum_in_div = malloc(sizeof(T_node));
        sum_in_div->coeff = tmp_coeff;
        sum_in_div->pow = tmp_pow;
        sum_in_div->p_next = NULL;
        
        //Добавляем к частному найденное слагаемое
        res = add_to_polinom(res_head_div, tmp_coeff, tmp_pow);
        
        if (res != fsc_ok)
        {
            clear_polinom(res_head_mod);
            return res;
        }
        
        //Полином, который будет вычитаться
        T_node* tmp_sub = NULL;
        res = mult_polinoms(&tmp_sub, head_2, sum_in_div);
        if (res != fsc_ok)
        {
            clear_polinom(res_head_mod);
            clear_polinom(res_head_div);
            return res;
        }
        
        //Вычитаем из делимого
        T_node* new_head_mod = NULL;
        res = sub_polinoms(&new_head_mod, *res_head_mod, tmp_sub);
        if (res != fsc_ok)
        {
            clear_polinom(res_head_mod);
            clear_polinom(res_head_div);
            return res;
        }
        
        //Очистили старое делимое
        clear_polinom(res_head_mod);
        
        //Присвоили старому делимому новое
        *res_head_mod = new_head_mod;

        current_node_mod = *res_head_mod;
        flag = false;
        while(current_node_mod != NULL)
        {
            if ((current_node_mod->pow >= head_2->pow) && (current_node_mod->coeff >=head_2->coeff))
            {
                flag = true;
                break;
            }
            current_node_mod = current_node_mod->p_next;
        }
    }
    
    if (*res_head_div == NULL)
        res = add_to_polinom(res_head_div, 0, 0);
    if (res != fsc_ok)
        return res;
    
    if (*res_head_mod == NULL)
        res = add_to_polinom(res_head_mod, 0, 0);
    if (res != fsc_ok)
        return res;
    
    return fsc_ok;
}

enum status_codes div_polinoms(T_node** res_head, T_node* head_1, T_node* head_2)
{
    if (*res_head != NULL || head_1 == NULL || head_2 == NULL)
        return fsc_invalid_parameter;
    
    T_node* head_mod = NULL;
    return div_and_mod_polinoms(res_head, &head_mod, head_1, head_2);
}

enum status_codes mod_polinoms(T_node** res_head, T_node* head_1, T_node* head_2)
{
    if (*res_head != NULL || head_1 == NULL || head_2 == NULL)
        return fsc_invalid_parameter;
    
    T_node* head_div = NULL;
    return div_and_mod_polinoms(&head_div, res_head, head_1, head_2);
}

enum status_codes eval_polinom(int* res, T_node* head, int point)
{
    if (head == NULL)
        return fsc_invalid_parameter;
    
    *res = 0;
    T_node* current_node = head;
    while(current_node != NULL)
    {
        *res += (current_node->coeff * pow(point, current_node->pow));
        current_node = current_node->p_next;
    }
    
    return fsc_ok;
}

enum status_codes diff_polinom(T_node** res_head, T_node* head)
{
    enum status_codes res = fsc_ok;
    
    if (*res_head != NULL || head == NULL)
        return fsc_invalid_parameter;
    
    T_node* current_node = head;
    while(current_node != NULL)
    {
        if (current_node->pow != 0)
            res = add_to_polinom(res_head, current_node->coeff * current_node->pow, current_node->pow - 1);
        if (res != fsc_ok)
            return res;
        current_node = current_node->p_next;
    }
    
    return fsc_ok;
}


enum status_codes pow_polinom(T_node** res_head, int pow_value, T_node* head)
{
    enum status_codes res = fsc_ok;
    
    if (*res_head != NULL || head == NULL)
        return fsc_invalid_parameter;
    
    res = add_to_polinom(res_head, 1, 0);
    if (res != fsc_ok)
        return res;
    
    for (int i = 0; i < pow_value; ++i)
    {
        T_node* tmp_head = NULL;
        res = mult_polinoms(&tmp_head, *res_head, head);
        if (res != fsc_ok)
        {
            clear_polinom(res_head);
            return res;
        }
        
        clear_polinom(res_head);
        res = copy_polinom(res_head, tmp_head);
        if (res != fsc_ok)
        {
            clear_polinom(&tmp_head);
            return res;
        }
        
        clear_polinom(&tmp_head);
    }
    
    return fsc_ok;
}

//композиция
enum status_codes cmps_polinoms(T_node** res_head, T_node* head_1, T_node* head_2)
{
    enum status_codes res = fsc_ok;
    
    if (*res_head != NULL || head_1 == NULL || head_2 == NULL)
        return fsc_invalid_parameter;

    
    res = add_to_polinom(res_head, 0, 0);
    if (res != fsc_ok)
        return res;
    
    T_node* current_node = head_1;
    while(current_node != NULL)
    {
        //возводим в степень
        T_node* tmp_head_pow = NULL;
        res = pow_polinom(&tmp_head_pow, current_node->pow, head_2);
        if (res != fsc_ok)
            return res;
        
        //умножаем
        T_node* tmp_head_coeff = NULL;
        res = copy_polinom(&tmp_head_coeff, tmp_head_pow);
        if (res != fsc_ok)
        {
            clear_polinom(&tmp_head_pow);
            return res;
        }
        T_node* tmp_node = tmp_head_coeff;
        while(tmp_node != NULL)
        {
            tmp_node->coeff *= current_node->coeff;
            tmp_node = tmp_node->p_next;
        }
        
        T_node* tmp_head_sum = NULL;
        res = add_polinoms(&tmp_head_sum, *res_head, tmp_head_coeff);
        if (res != fsc_ok)
        {
            clear_polinom(&tmp_head_pow);
            clear_polinom(&tmp_head_coeff);
            return res;
        }
        
        clear_polinom(res_head);
        res = copy_polinom(res_head, tmp_head_sum);
        if (res != fsc_ok)
        {
            clear_polinom(&tmp_head_pow);
            clear_polinom(&tmp_head_coeff);
            clear_polinom(&tmp_head_sum);
            return res;
        }
        
        clear_polinom(&tmp_head_pow);
        clear_polinom(&tmp_head_coeff);
        clear_polinom(&tmp_head_sum);
        
        current_node = current_node->p_next;
    }
    
    return fsc_ok;
}


char* read_num_from_str(char* str, int* value)
{
    bool sign = true;
    
    char* p_str = str;
    if (*p_str == '-' || *p_str == '+')
    {
        sign = (*p_str == '+');
        ++p_str;
    }
    
    *value = 0;
    if (isnumber(*p_str))
    {
        while(isnumber(*p_str))
        {
            *value = *value * 10 + (*p_str - '0');
            ++p_str;
        }
    }
    else
        *value = 1;
    
    (*value) *= (sign) ? 1 : (-1);
    
    return p_str;
}

enum status_codes read_polinom(T_node** res_head, char* str)
{
    enum status_codes res = fsc_ok;
    
    if (*res_head != NULL || str == NULL)
        return fsc_invalid_parameter;
    
    int coeff = 1;
    int power = 0;
    char* p_str = str;
    
    while (*p_str != 0)
    {
        p_str = read_num_from_str(p_str, &coeff);
        
        if (*p_str == 'x' || *p_str == 'X')
        {
            ++p_str;
            if (*p_str == '^')
            {
                ++p_str;
                p_str = read_num_from_str(p_str, &power);
            }
            else
                power = 1;
        }
        
        res = add_to_polinom(res_head, coeff, power);
        if (res != fsc_ok)
            return res;
        
        coeff = 1;
        power = 0;
    }
    
    return fsc_ok;
}


