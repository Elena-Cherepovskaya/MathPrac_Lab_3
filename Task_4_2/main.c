//
//  main.c
//  4
//
//  Created by Лена on 04.11.2023.
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
    string* city;
    string* street;
    int house_number;
    string*  house_building;//может быть пустой
    int flat_number;
    string* index;
} address;

typedef struct
{
    address* parcel_address;
    double weight;
    string* mail_id;
    string* creation_time;
    string* delivery_time;
} mail;

typedef struct
{
    address* post_office_address;
    
    mail** mail_list;
    int len;
    int capacity;
} post;

typedef struct
{
    post** post_list;
    int len;
    int capacity;
} post_storage;

enum status_codes create_address(address** current_address, char* city, char* street, int house_number, char*  house_building, int flat_number, char* index)
{
    address *p = malloc(sizeof(address));
    *current_address = p;

    if (p == NULL)
        return fsc_memory_error_detected;
    
    p->city = NULL;
    p->street = NULL;
    p->house_building = NULL;
    p->index = NULL;
    
    enum status_codes function_result;
    
    function_result = create_from_char(&p->city, city);
    
    if (function_result == fsc_ok)
        function_result = create_from_char(&p->street, street);
            
    if (function_result == fsc_ok)
        function_result = create_from_char(&p->house_building, house_building);
    
    if (function_result == fsc_ok)
        function_result = create_from_char(&p->index, index);
       
    if (function_result == fsc_ok)
    {
        (*current_address)->house_number = house_number;
        (*current_address)->flat_number = flat_number;
    }
    
    if (function_result != fsc_ok)
    {
        if (p->city != NULL)
            clear_string(&p->city);
        
        if (p->street != NULL)
            clear_string(&p->street);
        
        if (p->house_building != NULL)
            clear_string(&p->house_building);
        
        if (p->index != NULL)
            clear_string(&p->index);
        
        free(p);
        (*current_address) = NULL;
    }
    
    return function_result;
}

void clear_address(address** current_address)
{
    address* p = *current_address;
    
    clear_string(&p->city);
    clear_string(&p->street);
    clear_string(&p->house_building);
    clear_string(&p->index);
    
    free(p);
    (*current_address) = NULL;
}


enum status_codes create_mail(mail** current_mail, address** parcel_address, double weight, char* mail_id, char* creation_time, char* delivery_time)
{
    mail *p = malloc(sizeof(mail));
    (*current_mail) = p;
    if (p == NULL)
    {
        clear_address(parcel_address);
        return fsc_memory_error_detected;
    }
    
    p->mail_id = NULL;
    p->creation_time = NULL;
    p->delivery_time = NULL;
    
    enum status_codes function_result;
    
    p->parcel_address = *parcel_address;
    p->weight = weight;
    
    function_result = create_from_char(&p->mail_id, mail_id);
    
    if (function_result == fsc_ok)
        function_result = create_from_char(&p->creation_time, creation_time);
    
    if (function_result == fsc_ok)
        function_result = create_from_char(&p->delivery_time, delivery_time);
    
    if (function_result != fsc_ok)
    {
        clear_address(parcel_address);
        
        if (p->mail_id != NULL)
            clear_string(&p->mail_id);
        
        if (p->creation_time != NULL)
            clear_string(&p->creation_time);
        
        if (p->delivery_time != NULL)
            clear_string(&p->delivery_time);
        
        free(p);
        *current_mail = NULL;
    }
    
    return function_result;
}


enum status_codes create_post(post** current_post, address** post_address)
{
    post *p = malloc(sizeof(post));
    *current_post = p;
    if (p == NULL)
    {
        clear_address(post_address);
        return fsc_memory_error_detected;
    }
    
    p->post_office_address = *post_address;
    p->mail_list = NULL;
    p->len = 0;
    p->capacity = 0;
    
    return fsc_ok;
}

enum status_codes create_post_storage(post_storage** post_list)
{
    post_storage *p = malloc(sizeof(post_storage));
    *post_list = p;
    if (p == NULL)
        return fsc_memory_error_detected;
    
    p->post_list = NULL;
    p->capacity = 0;
    p->len = 0;
    
    return fsc_ok;
}

void clear_mail(mail** current_mail)
{
    mail *p = *current_mail;
    
    clear_address(&p->parcel_address);
    clear_string(&p->mail_id);
    clear_string(&p->creation_time);
    clear_string(&p->delivery_time);
    
    free(p);
    (*current_mail) = NULL;
}

void delete_data(post_storage** post_list)
{
    for (int i = 0; i < (*post_list)->len; ++i)
    {
        post* current_post = ((*post_list)->post_list)[i];
        
        for (int j = 0; j < current_post->len; ++j)
            clear_mail(&(current_post->mail_list[j]));

        clear_address(&(current_post->post_office_address));
        
        free(current_post->mail_list);
        current_post->mail_list = NULL;
    }
    free(*post_list);
    *post_list = NULL;
}

void clear_post(post** current_post)
{
    clear_address(&((*current_post)->post_office_address));
    
    for (int i = 0; i < (*current_post)->len; ++i)
        clear_mail(&(((*current_post)->mail_list)[i]));
    
    free(*current_post);
    (*current_post) = NULL;
}

enum status_codes add_post(post_storage * post_list, post** current_post)
{
    if (post_list->capacity == 0)
    {
        const int start_capacity = 32;
        post_list->post_list = malloc(sizeof(post*) * start_capacity);
        if (post_list == NULL)
        {
            clear_post(current_post);
            return fsc_memory_error_detected;
        }
        post_list->capacity = start_capacity;
    }
    
    if (post_list->len == post_list->capacity)
    {
        const int capacity = post_list->capacity * 2;
        post** tmp_p = realloc(post_list->post_list, sizeof(post *) * capacity);
        if (tmp_p == NULL)
        {
            clear_post(current_post);
            return fsc_memory_error_detected;
        }
        post_list->capacity = capacity;
        post_list->post_list = tmp_p;
    }
    
    (post_list->post_list)[post_list->len] = *current_post;
    ++(post_list->len);
    
    return fsc_ok;
}


int mail_comp (const void * a, const void * b)
{
    mail* mail_a = *(mail**)a;
    mail* mail_b = *(mail**)b;
    
    int res = comp(mail_a->parcel_address->index, mail_b->parcel_address->index);
    if (res != 0)
        return res;
    
    res = comp(mail_a->mail_id, mail_b->mail_id);
    if (res != 0)
        return res;
    
    return 0;
}

enum status_codes add_mail(post * const post, mail** current_mail)
{
    if (post->capacity == 0)
    {
        const int start_capacity = 32;
        post->mail_list = malloc(sizeof(mail*) * start_capacity);
        if (post->mail_list == NULL)
        {
            clear_mail(current_mail);
            return fsc_memory_error_detected;
        }
        post->capacity = start_capacity;
    }
    
    if (post->len == post->capacity)
    {
        const int capacity = post->capacity * 2;
        mail** tmp_p = realloc(post->mail_list, capacity);
        if (tmp_p == NULL)
        {
            clear_mail(current_mail);
            return fsc_memory_error_detected;
        }
        post->capacity = capacity;
        post->mail_list = tmp_p;
    }
    
    (post->mail_list)[post->len] = *current_mail;
    ++(post->len);
    
    qsort(post->mail_list, post->len, sizeof(mail*), mail_comp);
    
    return fsc_ok;
}


enum status_codes delete_mail(post_storage * const post_list, int post_ind, char * mail_id)
{
    //Введен индекс несуществующей почты
    if (post_ind < 0 || post_ind >= post_list->len)
        return fsc_invalid_parameter;
    
    //p_post - указатель на нужную почту
    post* p_post = (post_list->post_list)[post_ind];
    
    string* mail_id_string;
    create_from_char(&mail_id_string, mail_id);
    
    //Поиск письма по id
    bool mail_detected = false;
    int i = 0;
    for (int i = 0; i < p_post->len; ++i)
    {
        if (! is_equivalents(((p_post->mail_list)[i])->mail_id,mail_id_string))
            ++i;
        else
        {
            mail_detected = true;
            break;
        }
    }
    
    if (mail_detected)
    {
        clear_mail(&p_post->mail_list[i]);
        (p_post->mail_list)[i] = (p_post->mail_list)[p_post->len - 1];
        --(p_post->len);
        qsort(p_post->mail_list, p_post->len, sizeof(mail*), mail_comp);
    }
    else
        return fsc_invalid_parameter;
    
    return fsc_ok;
}

void address_printf(address* current_address)
{
    printf("Город: %s\n", (current_address->city)->str);
    printf("Улица: %s\n", (current_address->street)->str);
    printf("Номер дома: %d\n", current_address->house_number);
    printf("Строение: %s\n", (current_address->house_building)->str);
    printf("Номер квартиры: %d\n", current_address->flat_number);
    printf("Индекс: %s\n", (current_address->index)->str);
}

enum status_codes post_printf(post_storage* post_list, int ind)
{
    if (ind < 0 || ind >= post_list->len)
    {
        printf("Post witn such index is not detected\n");
        return fsc_invalid_parameter;
    }
    
    post* current_post = (post_list->post_list)[ind];
    address_printf(current_post->post_office_address);
    
    return fsc_ok;
}

void _mail_printf(mail* current_mail)
{
    address_printf(current_mail->parcel_address);
    printf("Вес письма: %lf\n", current_mail->weight);
    printf("Почтовый идентификатор: %s\n", (current_mail->mail_id)->str);
    printf("Время создания: %s\n", (current_mail->creation_time)->str);
    printf("Время получения: %s\n", (current_mail->delivery_time)->str);
    printf("\n");
}

enum status_codes mail_printf(post_storage* post_list,char* mail_id)//Вывод информации о письме по id
{
    string* mail_id_string;
    create_from_char(&mail_id_string, mail_id);
    
    mail* current_mail = NULL;
    bool mail_detected = false;
    for (int i = 0; i < post_list->len; ++i)
    {
        //Поиск письма по id
        post* p_post = (post_list->post_list)[i];
        
        for (int j = 0; j < p_post->len; ++j)
        {
            if(!is_equivalents(((p_post->mail_list)[j])->mail_id,mail_id_string))
                ++i;
            else
            {
                current_mail = (p_post->mail_list)[j];
                mail_detected = true;
                break;
            }
        }
    }
    
    if (mail_detected)
    {
        _mail_printf(current_mail);
    }
    else
    {
        printf("Letter with such id is not detected\n");
        return fsc_invalid_parameter;
    }
    return fsc_ok;
}

//Вывод списка писем выбранной почты
enum status_codes mail_list_printf(post_storage* post_list, int ind)
{
    if (ind < 0 || ind >= post_list->len)
    {
        printf("Post witn such index is not detected\n");
        return fsc_invalid_parameter;
    }
    
    post* current_post = (post_list->post_list)[ind];
        
    for (int i = 0; i < current_post->len; ++i)
    {
        mail* current_mail = (current_post->mail_list)[i];
        
        _mail_printf(current_mail);
    }
    
    return fsc_ok;
}

void post_and_mail_printf (post_storage* post_list)
{
    if (post_list->len == 0)
    {
        printf("Post storage is empty\n");
        return;
    }
    
    for (int i = 0; i < (post_list->len); ++i)
    {
        printf("Почта:\n");
        post* current_post = (post_list->post_list)[i];
        address_printf(current_post->post_office_address);
        printf("\nСписок писем:\n");
        
        mail_list_printf(post_list, i);
    }
    return;
}


enum status_codes overdue_mail_printf(post_storage* post_list)
{
    if (post_list->len == 0)
    {
        printf("Post storage is empty\n");
        return fsc_ok;
    }
    
    time_t real_time = time(NULL);
    
    for (int i = 0; i < post_list->len; ++i)
    {
        post* current_post = (post_list->post_list)[i];
        
        for (int j = 0; j < current_post->len; ++j)
        {
            mail* current_mail = (current_post->mail_list)[j];
            if (strcmp((current_mail->delivery_time)->str, "") == 0)
            {
                struct tm current_creation_time;
                
                strptime((current_mail->creation_time)->str, "%d:%m:%Y %H:%M:%S", &current_creation_time);
                
                time_t time_t_current_creation_time = mktime(&current_creation_time);
                double time_difference = difftime(time_t_current_creation_time, real_time);
                
                if (fabs(time_difference) > 86400.0)
                    _mail_printf(current_mail);
            }
        }
    }
    return fsc_ok;
}

//Если адрес доставки указан, то письмо доставлено
void printf_delivered_emails (post_storage* post_list)
{
    if (post_list->len == 0)
    {
        printf("Post storage is empty\n");
        return;
    }
    
    for (int i = 0; i < post_list->len; ++i)
    {
        post* current_post = (post_list->post_list)[i];
        for (int j = 0; j < current_post->len; ++j)
        {
            mail* current_mail = (current_post->mail_list)[j];
            if (!(is_empty(current_mail->delivery_time)))
                _mail_printf(current_mail);
        }
    }
}


int main(int argc, const char * argv[])
{
    enum status_codes res = fsc_ok;
        
    post_storage* post_list = NULL;
    res = create_post_storage(&post_list);
    
    if (res == fsc_ok)
    {
        char city[MAX_BUFFER_SIZE];
        char street[MAX_BUFFER_SIZE];
        int house_number = 0;
        char house_building[MAX_BUFFER_SIZE];
        int flat_number = 0;
        char index[MAX_BUFFER_SIZE];
        
        double weight;
        char mail_id[MAX_BUFFER_SIZE];
        char creation_time[MAX_BUFFER_SIZE];
        char delivery_time[MAX_BUFFER_SIZE];
        
        
        //Тестовый ввод

        //1я почта
        address* post_address;
        create_address(&post_address, "a_city", "a_street", 10, "a", 39, "999999");
        
        post* current_post;
        create_post(&current_post, &post_address);
        
        add_post(post_list, &current_post);
                        
        //1е письмо 1я почта
        address* parsel_address;
        create_address(&parsel_address, "a_city", "a_street", 2, "b", 40, "777777");
        
        int post_ind = 0;
        
        mail* current_mail;
        create_mail(&current_mail, &parsel_address, 12, "1234", "12:10:2022 10:10:12", "");
        add_mail((post_list->post_list)[post_ind], &current_mail);
           
            
        //2е письмо 1я почта
        create_address(&parsel_address, "b_city", "b_street", 7, "b", 34, "111111");
        
        create_mail(&current_mail, &parsel_address, 54, "7834", "12:09:2022 10:10:12", "12:10:2022 10:10:12");
        add_mail((post_list->post_list)[post_ind], &current_mail);
        
        //3е письмо 1я почта
        create_address(&parsel_address, "c_city", "c_street", 7, "b", 34, "999999");
        
        create_mail(&current_mail, &parsel_address, 54, "7904", "10:10:2022 10:10:12", "12:11:2022 10:10:12");
        add_mail((post_list->post_list)[post_ind], &current_mail);
                
        //4е письмо 1я почта
        create_address(&parsel_address, "d_city", "d_street", 9, "b", 30, "333333");
        
        create_mail(&current_mail, &parsel_address, 88, "1104", "11:10:2022 10:10:12", "12:11:2022 10:10:12");
        add_mail((post_list->post_list)[post_ind], &current_mail);
                            
        //5е письмо 1я почта
        create_address(&parsel_address, "b_city", "b_street", 2, "b", 40, "999999");
        
        create_mail(&current_mail, &parsel_address, 12, "9034", "12:10:2022 10:10:12", "");
        //post_ind = 1;
        add_mail((post_list->post_list)[post_ind], &current_mail);
                       
        //6е письмо 1я почта
        create_address(&parsel_address, "b_city", "b_street", 7, "b", 34, "222222");
        
        create_mail(&current_mail, &parsel_address, 54, "7834", "12:09:2022 10:10:12", "12:10:2022 10:10:12");
        add_mail((post_list->post_list)[post_ind], &current_mail);
        
            
        //7е письмо 1я почта
        create_address(&parsel_address, "c_city", "c_street", 7, "b", 34, "888888");
        
        create_mail(&current_mail, &parsel_address, 54, "7904", "10:10:2022 10:10:12", "12:11:2022 10:10:12");
        add_mail((post_list->post_list)[post_ind], &current_mail);
                
        bool finish_flag = false;
        while (true)
        {
            printf("Меню:\n1 - Добавить почту\n2 - Добавить письмо\n3 - Удалить письмо\n4 - Показать список почт\n5 - Показать список писем выбранной почты\n6 - Показать список всех почт и писем\n7 - информативный вывод данных об отправлении при поиске объекта типа Mail по идентификатору\n8 - поиск всех доставленных отправлений\n9 - Показать отправления срок достаки, которых на данный момент истек\n10 - Завершить работу\n");

            printf("Выберите пункт меню:\n");
            
            int p = 0;
            scanf("%d", &p);
            
            switch (p)
            {
                case 1: // Добавление почты
                {
                    printf("Введите адресс почты в формате:город улица номер_дома строение номер_квартиры индекс\n");
                    scanf("%s %s %d %s %d %s",city, street, &house_number, house_building, &flat_number, index);
            
                    address* post_address;
                    res = create_address(&post_address, city, street, house_number, house_building, flat_number, index);
                    
                    if (res == fsc_ok)
                    {
                        post* current_post;
                        res = create_post(&current_post, &post_address);
                    }
                    
                    if (res == fsc_ok)
                    {
                        res = add_post(post_list, &current_post);
                    }
                    break;
                }
                case 2: // Добавление письма
                {
                    printf("адрес полyчателя(город улица номер_дома строение номер_квартиры индекс) вес почтовый_идентификатор время_создания(dd:mm:yyyy hh:mm:ss) время_получения(dd:mm:yyyy hh:mm:ss)\n");
                    
                    scanf("%s %s %d %s %d %s %lf %s %s %s",city, street, &house_number, house_building, &flat_number, index, &weight, mail_id, creation_time, delivery_time);
                    
                    address* parsel_address;
                    res = create_address(&parsel_address, city, street, house_number, house_building, flat_number, index);
                    
                    mail* current_mail;
                    if (res == fsc_ok)
                        res = create_mail(&current_mail, &parsel_address, weight, mail_id, creation_time, delivery_time);
                    
                    if (res == fsc_ok)
                    {
                        printf("Выберите индекс почты (0 : %d)\n", post_list->len - 1);
                        
                        int post_ind = 0;
                        scanf("%d", &post_ind);
                        
                        if (post_ind < 0 || post_ind >= post_list->len)
                        {
                            printf("Введен индекс несуществующей почты\n");
                            res = fsc_invalid_parameter;
                            break;
                        }
                        
                        res = add_mail((post_list->post_list)[post_ind], &current_mail);
                    }
                
                    break;
                }
                case 3: // Удаление письма
                {
                    printf("Выберите индекс почты (0 : %d)\n", post_list->len - 1);
                    
                    int post_ind = 0;
                    scanf("%d", &post_ind);
                    
                    printf("Введите id письма:\n");
                    char mail_id[MAX_BUFFER_SIZE];
                    scanf("%s", mail_id);
                    
                    res = delete_mail(post_list, post_ind, mail_id);
                    
                    break;
                }

                case 4: // Вывод на экран списка почт
                {
                    for (int i = 0; i < post_list->len; ++i)
                        post_printf(post_list, i);
                    break;
                }

                case 5: // Вывод списка писем выбранной почты
                {
                    int ind = 0;
                    printf("Введите индекс почты:\n");
                    scanf("%d", &ind);
                    
                    mail_list_printf(post_list, ind);
                    break;
                }
                
                case 6: // Вывод списка почт и писем
                {
                    post_and_mail_printf(post_list);
                    break;
                }
                    
                case 7: // информативный вывод данных об отправлении при поиске объекта типа Mail по идентификатору
                {
                    char mail_id[MAX_BUFFER_SIZE];
                    printf("Введите id письма:\n");
                    scanf("%s", mail_id);
                    mail_printf(post_list, mail_id);
                    break;
                }

                case 8: // поиска всех доставленных отправлений
                {
                    //  Если время вручения указано, то считается, что письмо доставлено

                    printf_delivered_emails(post_list);
                    break;
                }
                    
                case 9: // Отправления срок достаки, которых на данный момент истек
                {
                    //Если время вручения не указано и прошло более 1 суток с момента отправки, считается,  что срок доставки на текущий момент времени истек

                    overdue_mail_printf(post_list);
                    break;
                }
                case 10:
                    finish_flag = true;
                    break;
                default:
                    res = fsc_invalid_parameter;
                    break;
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
                default:
                    break;
            }
        
             if (finish_flag)
                 break;
        }
                
        if (post_list != NULL)
            delete_data(&post_list);
    }
    
    return res == fsc_ok ? 0 : 1;
}
