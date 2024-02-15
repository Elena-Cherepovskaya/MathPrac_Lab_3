//
//  polinome.h
//  8
//
//  Created by Лена on 24.01.2024.
//

#ifndef polinome_h
#define polinome_h

#include "status_code.h"

typedef struct
{
    int coeff;
    int pow;
    void* p_next;
}T_node;

enum status_codes add_to_polinom(T_node** head, int coeff, int pow);
enum status_codes read_polinom(T_node** res_head, char* str);
enum status_codes add_polinoms(T_node** res_head, T_node* head_1, T_node* head_2);
enum status_codes sub_polinoms(T_node** res_head, T_node* head_1, T_node* head_2);
enum status_codes mult_polinoms(T_node** res_head, T_node* head_1, T_node* head_2);
enum status_codes div_polinoms(T_node** res_head, T_node* head_1, T_node* head_2);
enum status_codes mod_polinoms(T_node** res_head, T_node* head_1, T_node* head_2);
enum status_codes eval_polinom(int* res, T_node* head, int point);
enum status_codes diff_polinom(T_node** res_head, T_node* head);
enum status_codes cmps_polinoms(T_node** res_head, T_node* head_1, T_node* head_2);

void print_polinom(T_node* head);
enum status_codes clear_polinom(T_node** head);
enum status_codes copy_polinom(T_node** new_head, T_node* head);
#endif /* polinome_h */
