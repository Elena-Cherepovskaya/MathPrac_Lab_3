//
//  main.c
//  6
//
//  Created by Лена on 16.11.2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define START_CAPACITY 32
#define MAX_BUFFER_SIZE 256

enum status_codes
{
    fsc_ok,
    fsc_overflow,
    fsc_invalid_parameter,
    fsc_memory_error_detected,
    fsc_file_is_not_found,
    fsc_track_not_added,
    fsc_track_list_is_empty,
};

enum _marker
{
    begin,
    transition,
    end,
};

typedef struct
{
    int id;
    double x;
    double y;
}T_bus_stop;

typedef struct
{
    T_bus_stop* bus_stop;
    time_t stopping_time;
    time_t departure_time;
    enum _marker marker;
}T_stop;

typedef struct
{
    T_stop stop;
    void* next_stop;
}T_track;

typedef struct
{
    int bus_num;
    T_track* begin_track;
    void* next_track;
}T_track_list;

enum status_codes create_bus_stop(T_bus_stop** current_bus_stop,
                                  int id, double x, double y)
{
    *current_bus_stop = malloc(sizeof(T_bus_stop));
    if (*current_bus_stop == NULL)
        return fsc_memory_error_detected;
    
    (*current_bus_stop)->id = id;
    (*current_bus_stop)->x = x;
    (*current_bus_stop)->y = y;
    
    return fsc_ok;
}

enum status_codes create_track_list(T_track_list** current_track_list, int bus_num)
{
    T_track_list* p = malloc(sizeof(T_track_list));
    *current_track_list = p;
    
    if (p == NULL)
        return fsc_memory_error_detected;
    
    p->bus_num = bus_num;
    p->begin_track = NULL;
    p->next_track = NULL;
    
    return fsc_ok;
}

enum status_codes create_T_stop(T_stop** current_stop, T_bus_stop* bus_stop, char* stopping_time, char* departure_time, char marker)
{
    (*current_stop) = malloc(sizeof(T_stop));
    if (*current_stop == NULL)
        return fsc_memory_error_detected;
    
    (*current_stop)->bus_stop = bus_stop;
    
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));
    strptime(stopping_time, "%d:%m:%Y %H:%M:%S", &tm);
    (*current_stop)->stopping_time = mktime(&tm);
    
    memset(&tm, 0, sizeof(struct tm));
    strptime(departure_time, "%d:%m:%Y %H:%M:%S", &tm);
    (*current_stop)->departure_time = mktime(&tm);
    
    switch (marker)
    {
        case 'b':
            (*current_stop)->marker = begin;
            break;
        case 't':
            (*current_stop)->marker = transition;
            break;
        case 'e':
            (*current_stop)->marker = end;
            break;
        default:
            break;
    }
    
    return fsc_ok;
}

enum status_codes create_track(T_track** current_track, T_stop* current_stop)
{
    (*current_track) = malloc(sizeof(T_track));
    if (*current_track == NULL)
        return fsc_memory_error_detected;
    
    (*current_track)->stop = *current_stop;
    (*current_track)->next_stop = NULL;
    
    return fsc_ok;
}


enum status_codes add_begin_to_track_list(T_track_list** track_list_first, T_stop *current_stop, int bus_num)
{
    enum status_codes res = fsc_ok;
    
    if (*track_list_first == NULL)
    {
        T_track* track = NULL;
        res = create_track(&track, current_stop);
        if (res == fsc_ok)
            res = create_track_list(track_list_first, bus_num);
        
        if (res == fsc_ok)
            (*track_list_first)->begin_track = track;
        return res;
    }
    
    T_track_list* last = *track_list_first;

    while (last->next_track != NULL)
        last = last->next_track;
    
    res = create_track_list((T_track_list**)&last->next_track, bus_num);
    if (res == fsc_ok)
    {
        last = last->next_track;
        res = create_track(&last->begin_track, current_stop);
    }
    return res;
}

enum status_codes add_end_to_track_list(T_track_list** track_list_first, T_stop *current_stop, int bus_num)
{
    T_track_list* last = *track_list_first;

    T_track* found_track = NULL;
    while (last != NULL)
    {
        if (last->bus_num == bus_num)
        {
            if (found_track == NULL)
                found_track = last->begin_track;
            else
            {
                int test_time = difftime(current_stop->stopping_time, last->begin_track->stop.departure_time);
                int test_time_found_track = difftime(current_stop->stopping_time, found_track->stop.departure_time);
                
                // Иначе ищем ближайший к началу
                if ((test_time > 0) && (test_time < test_time_found_track))
                {
                    found_track = last->begin_track;
                }
            }
        }
        last = last->next_track;
    }
    
    return create_track((T_track**)&found_track->next_stop, current_stop);
}
 
enum status_codes add_transition_to_track_list(T_track_list * track_first, T_stop *current_stop, int bus_num)
{
    T_track_list* current_track_list = track_first;
    while (current_track_list != NULL)
    {
        if (current_track_list->bus_num == bus_num)
        {
            if (current_track_list->begin_track->stop.departure_time < current_stop->stopping_time)
            {
                T_track *test_track = current_track_list->begin_track;
                while (test_track->next_stop != NULL)
                {
                    T_track *next_test_track = test_track->next_stop;
                    if ((current_stop->stopping_time > test_track->stop.stopping_time) &&
                        (current_stop->departure_time < next_test_track->stop.departure_time))
                    {
                        T_track *track;
                        if (create_track(&track, current_stop) == fsc_ok)
                        {
                            track->next_stop = next_test_track;
                            test_track->next_stop = track;
                            return fsc_ok;
                        }
                        return fsc_memory_error_detected;
                    }
                    test_track = next_test_track;
                }
            }
        }
        current_track_list = current_track_list->next_track;
    }
    return fsc_track_not_added;
}


void track_list_print(T_track_list* track_list_first)
{
    T_track_list* track_list_last = track_list_first;
    
    while (true)
    {
        printf("Номер автобуса: %d\n", track_list_last->bus_num);
        
        T_track* track_last = track_list_last->begin_track;
        while (true)
        {
            T_stop* current_stop = &track_last->stop;
            printf("Номер остановки: %d\n", current_stop->bus_stop->id);
            printf("Координаты остановки: %lf %lf\n", current_stop->bus_stop->x, current_stop->bus_stop->y);
            
            struct tm * tm = localtime(&(current_stop->stopping_time));
            printf("Время прибытия: %d:%d:%d %d:%d:%d\n",tm->tm_mday, tm->tm_mon, tm->tm_year, tm->tm_hour, tm->tm_min, tm->tm_sec);
            
            tm = localtime(&(current_stop->departure_time));
            printf("Время отбытия: %d:%d:%d %d:%d:%d\n",tm->tm_mday, tm->tm_mon, tm->tm_year, tm->tm_hour, tm->tm_min, tm->tm_sec);
            
            printf("Маркер %d\n\n", current_stop->marker);
            
            if (track_last->next_stop == NULL)
                break;
            track_last = track_last->next_stop;
        }
        
        if (track_list_last->next_track == NULL)
            break;
        track_list_last = track_list_last->next_track;
    }
}

typedef struct
{
    int bus_num;
    int counter;
    double track_length;
    int min_stop;
    int max_stop;
    int full_stop_time;
}T_bus_and_counter;

typedef struct
{
    T_bus_and_counter* bus_list;
    int capasity;
    int len;
}T_bus_and_counter_list;

enum status_codes create_bus_and_counter_list(T_bus_and_counter_list** list)
{
    if (*list != NULL)
        return fsc_invalid_parameter;
    
    *list = malloc(sizeof(T_bus_and_counter_list));
    if (*list == NULL)
        return fsc_memory_error_detected;
    
    (*list)->bus_list = malloc(sizeof(T_bus_and_counter) * START_CAPACITY);
    if ((*list)->bus_list == NULL)
    {
        free(*list);
        *list = NULL;
        return fsc_memory_error_detected;
    }
    
    (*list)->len = 0;
    (*list)->capasity = START_CAPACITY;
    
    return fsc_ok;
}

int min(int a, int b)
{
    return (a < b) ? a : b;
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

double pow_2 (double a)
{
    return a * a;
}


void print_bus_with_count(T_bus_and_counter_list* list)
{
    for (int i = 0; i < list->len; ++i)
    {
        printf("Bus num %d\n",(list->bus_list)[i].bus_num);
        printf("counter %d\n", (list->bus_list)[i].counter);
        printf("track_lenth %lf\n", (list->bus_list)[i].track_length);
        printf("min_stop %d\n", (list->bus_list)[i].min_stop);
        printf("max stop %d\n", (list->bus_list)[i].max_stop);
        printf("full stopping time %d\n\n", (list->bus_list)[i].full_stop_time);
    }
}

enum status_codes add_to_bus_and_counter_list(T_bus_and_counter_list** list, int bus_num, T_track* first_stop_in_track)
{
    if (*list == NULL)
        return fsc_invalid_parameter;
    
    bool bus_is_found = false;
    for (int i = 0; i < (*list)->len; ++i)
    {
        if ((*list)->bus_list[i].bus_num == bus_num)
        {
            bus_is_found = true;
            (*list)->bus_list[i].counter += 1;
            break;
        }
    }
    
    if (!(bus_is_found))
    {
        if ((*list)->len == (*list)->capasity)
        {
            (*list)->capasity *= 2;
            T_bus_and_counter* tmp_p = realloc((*list)->bus_list, sizeof(T_bus_and_counter) * (*list)->capasity);
            if (tmp_p == NULL)
            {
                free((*list)->bus_list);
                free(*list);
                *list = NULL;
                return fsc_memory_error_detected;
            }
            (*list)->bus_list = tmp_p;
        }
        
        int index = (*list)->len;
        (*list)->bus_list[index].bus_num = bus_num;
        (*list)->bus_list[index].counter = 1;
        (*list)->len += 1;
        
        T_track* current_stop = first_stop_in_track;
        T_track* next_stop = first_stop_in_track->next_stop;
        double track_len = 0.0;
        int min_stop = 86400;
        int max_stop = 0;
        int full_stop_time = 0;
        while(true)
        {
            if (next_stop != NULL)
                track_len += sqrt(pow_2(current_stop->stop.bus_stop->x - next_stop->stop.bus_stop->x) + pow_2(current_stop->stop.bus_stop->y - next_stop->stop.bus_stop->y));
            
            min_stop = min(abs(difftime(current_stop->stop.departure_time, current_stop->stop.stopping_time)), min_stop);
            
            max_stop = max(abs(difftime(current_stop->stop.departure_time, current_stop->stop.stopping_time)), max_stop);
            
            full_stop_time += abs(difftime(current_stop->stop.departure_time, current_stop->stop.stopping_time));
            current_stop = next_stop;
            if (current_stop == NULL)
                break;
            next_stop = current_stop->next_stop;
        }
        
        (*list)->bus_list[index].track_length = track_len;
        (*list)->bus_list[index].min_stop = min_stop;
        (*list)->bus_list[index].max_stop = max_stop;
        (*list)->bus_list[index].full_stop_time = full_stop_time;
        // todo добавить расчеты
    }

    return fsc_ok;
}

enum status_codes clear_bus_and_counter_list(T_bus_and_counter_list** list)
{
    if (*list == NULL)
        return fsc_invalid_parameter;
    
    free(*list);
    *list = NULL;
    
    return fsc_ok;
}

//Собирает данные о треках автобусов
enum status_codes make_base_data(T_track_list* track_list, T_bus_and_counter_list** list)
{
    enum status_codes res = fsc_ok;
    
    res = create_bus_and_counter_list(list);
    if (res != fsc_ok)
        return res;

    while(track_list != NULL)
    {
        res = add_to_bus_and_counter_list(list, track_list->bus_num, track_list->begin_track);
        if (res != fsc_ok)
            return res;
        track_list = track_list->next_track;
    }
    
    //print_bus_with_count(list);
    return fsc_ok;
}

enum status_codes bus_with_max_track_count(T_bus_and_counter_list* list, int* bus_num)
{
    int max_count = 0;
    
    if (list == NULL)
        return fsc_invalid_parameter;
    
    for (int i = 0; i < list->len; ++i)
    {
        if (list->bus_list[i].counter > max_count)
        {
            max_count = list->bus_list[i].counter;
            *bus_num = list->bus_list[i].bus_num;
        }
    }
    
    return fsc_ok;
}

enum status_codes bus_with_min_track_count(T_bus_and_counter_list* list, int* bus_num)
{
    if (list == NULL)
        return fsc_invalid_parameter;
    
    int min_count = list->bus_list[0].counter;
    *bus_num = list->bus_list[0].bus_num;
    
    for (int i = 1; i < list->len; ++i)
    {
        if (list->bus_list[i].counter < min_count)
        {
            min_count = list->bus_list[i].counter;
            *bus_num = list->bus_list[i].bus_num;
        }
    }
    
    return fsc_ok;
}

enum status_codes bus_with_max_way(T_bus_and_counter_list* list, int* bus_num)
{
    if (list == NULL)
        return fsc_invalid_parameter;
    
    int max_way = 0;
    for (int i = 0; i < list->len; ++i)
    {
        int current_way = list->bus_list[i].counter * list->bus_list[i].track_length;
        if (current_way > max_way)
        {
            max_way = current_way;
            *bus_num = list->bus_list[i].bus_num;
        }
    }
    
    return fsc_ok;
}

enum status_codes bus_with_min_way(T_bus_and_counter_list* list, int* bus_num)
{
    if (list == NULL)
        return fsc_invalid_parameter;
    
    int min_way = list->bus_list[0].counter * list->bus_list[0].track_length;
    *bus_num = list->bus_list[0].bus_num;
    
    for (int i = 1; i < list->len; ++i)
    {
        int current_way = list->bus_list[i].counter * list->bus_list[i].track_length;
        if (current_way < min_way)
        {
            min_way = current_way;
            *bus_num = list->bus_list[i].bus_num;
        }
    }
    return fsc_ok;
}

enum status_codes bus_with_max_track(T_bus_and_counter_list* list, int* bus_num)
{
    if (list == NULL)
        return fsc_invalid_parameter;
    int max_track = 0;
    
    for (int i = 0; i < list->len; ++i)
    {
        if (list->bus_list[i].track_length > max_track)
        {
            max_track = list->bus_list[i].track_length;
            *bus_num = list->bus_list[i].bus_num;
        }
    }
    return fsc_ok;
}

enum status_codes bus_with_min_track(T_bus_and_counter_list* list, int* bus_num)
{
    if (list == NULL)
        return fsc_invalid_parameter;
    int min_track = list->bus_list[0].track_length;
    *bus_num = list->bus_list[0].bus_num;
    
    for (int i = 1; i < list->len; ++i)
    {
        if (list->bus_list[i].track_length < min_track)
        {
            min_track = list->bus_list[i].track_length;
            *bus_num = list->bus_list[i].bus_num;
        }
    }
    return fsc_ok;
}

enum status_codes bus_with_max_stop(T_bus_and_counter_list* list, int* bus_num)
{
    if (list == NULL)
        return fsc_invalid_parameter;
    
    int max_stop = 0;
    for (int i = 0; i < list->len; ++i)
    {
        if (list->bus_list[i].max_stop > max_stop)
        {
            max_stop = list->bus_list[i].max_stop;
            *bus_num = list->bus_list[i].bus_num;
        }
    }
    return fsc_ok;
}

enum status_codes bus_with_min_stop(T_bus_and_counter_list* list, int* bus_num)
{
    if (list == NULL)
        return fsc_invalid_parameter;
    int min_stop = list->bus_list[0].min_stop;
    *bus_num = list->bus_list[0].bus_num;
    
    for (int i = 1; i < list->len; ++i)
    {
        if (list->bus_list[i].min_stop < min_stop)
        {
            min_stop = list->bus_list[i].min_stop;
            *bus_num = list->bus_list[i].bus_num;
        }
    }
    return fsc_ok;
}

enum status_codes bus_with_max_full_stop_time(T_bus_and_counter_list* list, int* bus_num)
{
    if (list == NULL)
        return fsc_invalid_parameter;
    int max_full_stop = 0;
    
    for (int i = 0; i < list->len; ++i)
    {
        if (list->bus_list[i].full_stop_time > max_full_stop)
        {
            max_full_stop = list->bus_list[i].full_stop_time;
            *bus_num = list->bus_list[i].bus_num;
        }
    }
    return fsc_ok;
}

/*
 typedef struct
 {
     int bus_num;
     T_track* begin_track;
     void* next_track;
 }T_track_list;
 */

enum status_codes clear_track_list(T_track_list** track_list)
{
    if (*track_list == NULL)
        return fsc_invalid_parameter;
    
    while(*track_list != NULL)
    {
        T_track* current_track = (*track_list)->begin_track;
        while (current_track != NULL)
        {
            T_track* tmp_p = current_track;
            current_track = current_track->next_stop;
            free(tmp_p);
        }
        
        T_track_list* tmp_p = *track_list;
        (*track_list) = (*track_list)->next_track;
        free(tmp_p);
        
        tmp_p = NULL;
    }
    return fsc_ok;
}

int main(int argc, const char * argv[])
{
    enum status_codes res = fsc_ok;
    
    T_track_list* track_list_first = NULL;
    
    for (int k = 0; k < 3; ++k)
    {
        for (int i = 1; i < argc; ++i)
        {
            FILE* file_input = fopen(argv[i], "rb");
            res = (file_input == NULL) ? fsc_file_is_not_found : fsc_ok;
            
            if (res != fsc_ok)
                break;
            
            int stop_num;
            fscanf(file_input, "%d", &stop_num);
            
            double x;
            double y;
            fscanf(file_input, "%lf %lf", &x, &y);
            
            T_bus_stop* current_bus_stop;
            res = create_bus_stop(&current_bus_stop, stop_num, x, y);
            
            if (res != fsc_ok)
                break;
            
            int bus_num;
            char stopping_data[MAX_BUFFER_SIZE];
            char stopping_time[MAX_BUFFER_SIZE];
            char stopping_data_time[MAX_BUFFER_SIZE];
            
            char departure_data[MAX_BUFFER_SIZE];
            char departure_time[MAX_BUFFER_SIZE];
            char departure_data_time[MAX_BUFFER_SIZE];
            char marker;
            
            while (true)
            {
                int param_num = fscanf(file_input, "%d %s %s %s %s %c", &bus_num, stopping_data, stopping_time, departure_data, departure_time, &marker);
                
                if (param_num < 0)
                    break;
                
                strcpy(stopping_data_time, stopping_data);
                strcat(stopping_data_time, " ");
                strcat(stopping_data_time, stopping_time);
                
                strcpy(departure_data_time, departure_data);
                strcat(departure_data_time, " ");
                strcat(departure_data_time, departure_time);
                
                if ((k == 0) && (marker != 'b'))
                    continue;
                
                if ((k == 1) && (marker != 'e'))
                    continue;
                
                if ((k == 2) && (marker != 't'))
                    continue;
                
                T_stop* current_stop;
                res = create_T_stop(&current_stop, current_bus_stop, stopping_data_time, departure_data_time, marker);
                if (res != fsc_ok)
                    break;
                
                if (current_stop->marker == begin)
                {
                    res = add_begin_to_track_list(&track_list_first, current_stop, bus_num);
                    if (res != fsc_ok)
                        break;
                    continue;
                }
                
                if (current_stop->marker == end)
                {
                    res = add_end_to_track_list(&track_list_first, current_stop, bus_num);
                    if (res != fsc_ok)
                        break;
                    continue;
                }
                
                add_transition_to_track_list(track_list_first, current_stop, bus_num);
            }
                    
            fclose(file_input);
        }
    }

    T_bus_and_counter_list* list = NULL;
    if (res == fsc_ok)
    {
        res = make_base_data(track_list_first, &list);
        print_bus_with_count(list);
    }
    
    if (res == fsc_ok)
    {
        int p;
        bool flag = false;
        printf("Выберите пункт меню: \n");
        while (true)
        {
            printf("1 -  Выведи на экран все маршруты\n2 - Автобус, который проехал больше всех маршрутов\n3 - Автобус, который проехал  меньше всех маршрутов\n4 - Автобус, который проехал самый длинный путь\n5 - Автобус, который проехад самый короткий путь\n6 - Автобус, который проехал самый длинный маршрут\n7 - Автобус, который проехал самый короткий маршрут\n8 - Автобус с самой длинной остановкой\n9 -  Автобус с сомй короткой остаовкой\n10 - Автобус с самым большим временем простоя\n11 - Завершить\n");
            scanf("%d", &p);
            
            switch (p)
            {
                case 1:
                    track_list_print(track_list_first);
                    break;
                //Автобус, который проехал больше всех маршрутов
                case 2:
                {
                    int res_bus_num = 0;
                    bus_with_max_track_count(list, &res_bus_num);
                    printf("%d\n", res_bus_num);
                    break;
                }
                //Автобус, который проехал меньше всех маршрутов
                case 3:
                {
                    int res_bus_num = 0;
                    bus_with_min_track_count(list, &res_bus_num);
                    printf("%d\n", res_bus_num);
                    break;
                }
                //Автобус, который проехал самый длинный путь
                case 4:
                {
                    int res_bus_num = 0;
                    bus_with_max_way(list, &res_bus_num);
                    printf("%d\n", res_bus_num);
                    break;
                }
                //Автобус, который прехал самый короткий путь
                case 5:
                {
                    int res_bus_num = 0;
                    bus_with_min_track_count(list, &res_bus_num);
                    printf("%d\n", res_bus_num);
                    break;
                }
                //Автобус с самым длинным маршрутом
                case 6:
                {
                    int res_bus_num = 0;
                    bus_with_max_track(list, &res_bus_num);
                    printf("%d\n", res_bus_num);
                    break;
                }
                //Автобус с самым коротким маршрутом
                case 7:
                {
                    int res_bus_num = 0;
                    bus_with_min_track(list, &res_bus_num);
                    printf("%d\n", res_bus_num);
                    break;
                }
                //Автобус с самой длинной остановкой
                case 8:
                {
                    int res_bus_num = 0;
                    bus_with_max_stop(list, &res_bus_num);
                    printf("%d\n", res_bus_num);
                    break;
                }
                //Автобус с самой короткой остановкой
                case 9:
                {
                    int res_bus_num = 0;
                    bus_with_min_stop(list, &res_bus_num);
                    printf("%d\n", res_bus_num);
                    break;
                }
                //Автобус с самый большим временем простоя
                case 10:
                {
                    int res_bus_num = 0;
                    bus_with_max_full_stop_time(list, &res_bus_num);
                    printf("%d\n", res_bus_num);
                    break;
                }
                case 11:
                    flag = true;
                    break;
                default:
                    break;
            }
            if (flag)
                break;
        }
    }
    
    if (list != NULL)
        clear_bus_and_counter_list(&list);
    
    if (track_list_first != NULL)
        clear_track_list(&track_list_first);
    
    return res == fsc_ok ? 0 : 1;
}
