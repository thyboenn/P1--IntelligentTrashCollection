#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <io.h>
#include <process.h>
#include <string.h>


#define MARGIN 85
#define MARGIN2 60
#define NAREA 4
#define NSUBAREA 4
#define SIZE 5
#define DAY_MAX 10
#define AREAS_PER_TRUCK 4
#define LOWER1 6
#define LOWER2 8
#define LOWER3 10
#define UPPER1 8
#define UPPER2 10
#define UPPER3 12
#define DIRECTORY "D:\\C-programmer"

typedef struct subarea
{
    int co2_cost, time, co2_used, time_used, emptied_subarea_counter, lower_random, upper_random, activity_level;
    double average, total_trash_subarea_avg, sensorData[SIZE], sensorDataTotal[SIZE];
} subarea;

typedef struct area
{
    subarea *subarea_array[NAREA + 1];
    double average, total_trash_area_avg;
    int emptied_area_counter, co2_used, time_used;
} area;

void find_garbage_truck_amount(int *garbage_truck_amount);
void create_areas(area *area_array[NAREA + 1]);
void sensor_data_start(area *area, double start_var);
void create_directories();
void run_a_day(area *area_dynamic[NAREA + 1], area *area_static[NAREA + 1]);
void simulated_days(area *area_dynamic, area *area_static);
void average_trash(area *area);
void run_empty_trash(area *area_array[NAREA + 1], area *area_array_static[NAREA + 1], int garbage_truck_amount, int *area_number, int *co2_counter, int *time_counter, int *co2_counter_static, int *time_counter_static, int day_counter);
void compare_trash_to_empty(area *area_array[NAREA + 1], int *area_number_p);
void empty_trash(area *area, int *co2_counter_p, int *time_counter_p);
void empty_trash_static(area *area, int *co2_counter_static_p, int *time_counter_static_p);
void store_data(area *area_array[NAREA + 1], int condition);
void standard_print(area *area_array[NAREA + 1], area *area_array_static[NAREA + 1], int day_counter, int area_number);
int search_function();
int area_search(int i);
int subarea_search(int i, int j);
void read_single_data(int area_number, int subarea_number, int type_choice, char file_name[50], int DynamicOrStatic);
void read_multiple_data(int area_number, int subarea_number, char file_name[50], int DynamicOrStatic);

int main(void)
{
    int co2_counter = 0, time_counter = 0;
    int co2_counter_static = 0, time_counter_static = 0;
    int day_counter = 0;
    int area_number;
    int garbage_truck_amount = 0;
    
    area *area_array[NAREA + 1];
    area *area_array_static[NAREA + 1];

    srand(time(NULL));
    
    find_garbage_truck_amount(&garbage_truck_amount);

    create_areas(area_array);
    create_areas(area_array_static);
    
    create_directories();

    int running = 1;
    while (running)
    {
        run_a_day(area_array, area_array_static);

        run_empty_trash(area_array, area_array_static, garbage_truck_amount, &area_number, &co2_counter, &time_counter, &co2_counter_static, &time_counter_static, day_counter);

        store_data(area_array, 1);
        store_data(area_array_static, 2);
    
        standard_print(area_array, area_array_static, day_counter, area_number);
        
        day_counter++;
        if(day_counter == DAY_MAX) 
        {
            running = 0;
        }
    }

    printf("TOTAL AMOUNT OF CO2 AND TIME USED FOR %d DAYS\n", DAY_MAX);
    printf("DYNAMIC\t->  Co2: %d  &  Time: %d\n", co2_counter, time_counter);
    printf("STATIC\t->  Co2: %d  &  Time: %d\n\n", co2_counter_static, time_counter_static);
    printf("------------------------------------------------------------------------------------------------------------------------------\n\n");

    running = 1;
    while (running)
    {
       running = search_function();
    }
    
}

void find_garbage_truck_amount(int *garbage_truck_amount)
{
    for (int i = NAREA; i > 0; i -= AREAS_PER_TRUCK)
    {
        (*garbage_truck_amount)++;
    }
    printf("\nGarbage trucks employed: %d\n\n", *garbage_truck_amount);
}

void create_areas(area *area_array[NAREA + 1]) 
{
    int percentage_added[3][2] = {{LOWER1, UPPER1}, {LOWER2, UPPER2}, {LOWER3, UPPER3}};
    int activity_level;

    for (int i = 0; i < NAREA + 1; i++)
    {
        area_array[i] = malloc(sizeof(area));

        for (int j = 0; j < NSUBAREA; j++)
        {
            activity_level = (rand() % 3);
            area_array[i]->subarea_array[j] = malloc(sizeof(subarea));
            area_array[i]->subarea_array[j]->co2_cost = 50;
            area_array[i]->subarea_array[j]->time = 50;
            area_array[i]->subarea_array[j]->co2_used = 0;
            area_array[i]->subarea_array[j]->time_used = 0;
            area_array[i]->subarea_array[j]->average = 0;
            area_array[i]->subarea_array[j]->emptied_subarea_counter = 0;
            area_array[i]->subarea_array[j]->total_trash_subarea_avg = 0;
            area_array[i]->subarea_array[j]->lower_random = percentage_added[activity_level][0];
            area_array[i]->subarea_array[j]->upper_random = percentage_added[activity_level][1];
            area_array[i]->subarea_array[j]->activity_level = activity_level;

        }
        sensor_data_start(area_array[i], 0);
        area_array[i]->average = 0;
        area_array[i]->total_trash_area_avg = 0;
        area_array[i]->emptied_area_counter = 0;
        area_array[i]->co2_used = 0;
        area_array[i]->time_used = 0;
    }
}

void sensor_data_start(area *area, double start_var)
{
    for (int i = 0; i < NSUBAREA; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            area->subarea_array[i]->sensorData[j] = start_var;
            area->subarea_array[i]->sensorDataTotal[j] = start_var;
        }
    }
}

void create_directories() 
{
    char dirname[150];
    char dirname2[150];
    snprintf(dirname, 150, "%s\\data", DIRECTORY);
    snprintf(dirname2, 150, "%s\\area", dirname);
    mkdir(dirname);

    for (int i = 1; i < NAREA + 1; i++)
    {
        char newdirname[100];
        char newdirname2[100];
        snprintf(newdirname, 100, "%s%d", dirname2, i);
        mkdir(newdirname);
        
        for (int j = 1; j < NSUBAREA + 1; j++)
        {
            snprintf(newdirname2, 100, "%s%d%s%d", dirname2, i, "\\subarea", j);
            mkdir(newdirname2);
        }
        
    }
}

void run_a_day(area *area_dynamic[NAREA + 1], area *area_static[NAREA + 1])
{
    for (int i = 1; i < NAREA + 1; i++)
        {
            simulated_days(area_dynamic[i], area_static[i]);
            average_trash(area_dynamic[i]);
            average_trash(area_static[i]);
        }
}



void simulated_days(area *area_dynamic, area *area_static)
{
    for (int i = 0; i < NSUBAREA; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            int x = ((rand() % (area_dynamic->subarea_array[i]->upper_random - area_dynamic->subarea_array[i]->lower_random + 1)) + area_dynamic->subarea_array[i]->lower_random);
            area_dynamic->subarea_array[i]->sensorData[j] += x;
            area_dynamic->subarea_array[i]->sensorDataTotal[j] += x;

            area_static->subarea_array[i]->sensorData[j] += x;
            area_static->subarea_array[i]->sensorDataTotal[j] += x;
        }
    }
}


void average_trash(area *area)
{   
    double sum[NSUBAREA];
    double sum_total[NSUBAREA];
    double area_trash_sum = 0;
    double area_trash_sum_total = 0;

    for (int i = 0; i < NSUBAREA; i++)
    {
        sum[i] = 0;
        sum_total[i] = 0;
    }


    for (int i = 0; i < NSUBAREA; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            sum[i] += area->subarea_array[i]->sensorData[j];
            sum_total[i] += area->subarea_array[i]->sensorDataTotal[j];
        }
        area->subarea_array[i]->average = sum[i] / SIZE;
        area->subarea_array[i]->total_trash_subarea_avg = sum_total[i] / SIZE;
    }

    for (int i = 0; i < NSUBAREA; i++)
    {
        area_trash_sum += area->subarea_array[i]->average;
        area_trash_sum_total += area->subarea_array[i]->total_trash_subarea_avg;
    }

    area->average = area_trash_sum / NSUBAREA;
    area->total_trash_area_avg = area_trash_sum_total / NSUBAREA;
}


void run_empty_trash(area *area_array[NAREA + 1], area *area_array_static[NAREA + 1], int garbage_truck_amount, int *area_number, int *co2_counter, int *time_counter, int *co2_counter_static, int *time_counter_static, int day_counter)
{
    for (int i = 0; i < garbage_truck_amount; i++)
        {
            compare_trash_to_empty(area_array, area_number);
            empty_trash(area_array[(*area_number)], co2_counter, time_counter);
            
            for (int j = 1; j < NAREA + 1; j++)
            {
                average_trash(area_array[j]); 
            }
        }

    if (day_counter > 1 && day_counter % 14 == 0)
    {
        for (int i = 1; i < NAREA + 1; i++)
        {
            empty_trash_static(area_array_static[i], co2_counter_static, time_counter_static);
            average_trash(area_array_static[i]);
        }
    }
}


void compare_trash_to_empty(area *area_array[NAREA + 1], int *area_number_p)
{
    int highBlock = 0;

    for (int i = 1; i < NAREA + 1; i++)
    {
        if (area_array[i]->average >= MARGIN)
        {
            if (area_array[i]->average > area_array[highBlock]->average)
            {
                highBlock = i;
            }
        }
    }
    *area_number_p = highBlock;
}

void empty_trash(area *area, int *co2_counter_p, int *time_counter_p)
{
    for (int i = 0; i < NSUBAREA; i++)
    {
        if (area->subarea_array[i]->average > MARGIN2)
        {
            *co2_counter_p  += area->subarea_array[i]->co2_cost;
            *time_counter_p += area->subarea_array[i]->time;
            area->subarea_array[i]->co2_used += area->subarea_array[i]->co2_cost;
            area->subarea_array[i]->time_used += area->subarea_array[i]->time;
            area->co2_used += area->subarea_array[i]->co2_cost;
            area->time_used += area->subarea_array[i]->time;
            area->subarea_array[i]->average = 0;
            area->subarea_array[i]->emptied_subarea_counter++;
            for (int j = 0; j < SIZE; j++)
            { 
                if (area->subarea_array[i]->sensorData[j] <= 100) 
                {
                    area->subarea_array[i]->sensorData[j] = 0;
                }
                else if (area->subarea_array[i]->sensorData[j] > 100) 
                {
                    area->subarea_array[i]->sensorData[j] -= 100;
                }
            }
        }
    }
    area->emptied_area_counter++;
}

void empty_trash_static(area *area, int *co2_counter_static_p, int *time_counter_static_p)
{
    for (int i = 0; i < NSUBAREA; i++)
    {
        *co2_counter_static_p  += area->subarea_array[i]->co2_cost;
        *time_counter_static_p += area->subarea_array[i]->time;
        area->subarea_array[i]->co2_used += area->subarea_array[i]->co2_cost;
        area->subarea_array[i]->time_used += area->subarea_array[i]->time;
        area->co2_used += area->subarea_array[i]->co2_used;
        area->time_used += area->subarea_array[i]->time_used;
        area->subarea_array[i]->average = 0;
        area->subarea_array[i]->emptied_subarea_counter++;
        for (int j = 0; j < SIZE; j++)
        { 
            if (area->subarea_array[i]->sensorData[j] <= 100) 
            {
                area->subarea_array[i]->sensorData[j] = 0;
            }
            else if (area->subarea_array[i]->sensorData[j] > 100) 
            {
                area->subarea_array[i]->sensorData[j] -= 100;
            }
        }
    }
    area->emptied_area_counter++;
}


void store_data(area *area_array[NAREA + 1], int condition)
{
    FILE *output_filepointer;

    char dirname[150]; 
    char total_dirname[150];
    char total_dirname2[150];
    char total_dirname3[150];
    char avg_dirname[150];
    char avg_dirname2[150];
    char avg_dirname3[150];
    char co2_dirname[150];
    char co2_dirname2[150];
    char time_dirname[150];
    char time_dirname2[150];
    char emptied_dirname[150];
    char emptied_dirname2[150];
    snprintf(dirname, 150, "%s\\data\\area", DIRECTORY);

    for (int i = 1; i < NAREA + 1; i++)
    {
        if (condition == 1)
        {
            snprintf(total_dirname, 150, "%s%d\\area_average_total.txt", dirname, i);
            snprintf(avg_dirname, 150, "%s%d\\area_average.txt", dirname, i);
            snprintf(co2_dirname, 150, "%s%d\\area_co2_total.txt", dirname, i);
            snprintf(time_dirname, 150, "%s%d\\area_time_total.txt", dirname, i);
            snprintf(emptied_dirname, 150, "%s%d\\area_emptied.txt", dirname, i);
        }
        
        else if (condition == 2)
        {
            snprintf(total_dirname, 150, "%s%d\\area_static_average_total.txt", dirname, i);
            snprintf(avg_dirname, 150, "%s%d\\area_static_average.txt", dirname, i);
            snprintf(co2_dirname, 150, "%s%d\\area_static_co2_total.txt", dirname, i);
            snprintf(time_dirname, 150, "%s%d\\area_static_time_total.txt", dirname, i);
            snprintf(emptied_dirname, 150, "%s%d\\area_static_emptied.txt", dirname, i);
        }

        output_filepointer = fopen(total_dirname, "a");
        fprintf(output_filepointer, "%lf ", area_array[i]->total_trash_area_avg);
        
        fclose(output_filepointer);
        output_filepointer = fopen(avg_dirname, "a");
        fprintf(output_filepointer, "%lf ", area_array[i]->average);
        
        fclose(output_filepointer);
        output_filepointer = fopen(co2_dirname, "a");
        fprintf(output_filepointer, "%d ", area_array[i]->co2_used);
      
        fclose(output_filepointer);
        output_filepointer = fopen(time_dirname, "a");
        fprintf(output_filepointer, "%d ", area_array[i]->time_used);

        fclose(output_filepointer);
        output_filepointer = fopen(emptied_dirname, "a");
        fprintf(output_filepointer, "%d ", area_array[i]->emptied_area_counter);

        for (int j = 0; j < NSUBAREA; j++)
        {
            if (condition == 1)
            {
                snprintf(total_dirname2, 150, "%s%d%s%d\\subarea_average_total.txt", dirname, i, "\\subarea", j + 1);
                snprintf(avg_dirname2, 150, "%s%d%s%d\\subarea_average.txt", dirname, i, "\\subarea", j + 1);
                snprintf(co2_dirname2, 150, "%s%d%s%d\\subarea_co2_total.txt", dirname, i, "\\subarea", j + 1);
                snprintf(time_dirname2, 150, "%s%d%s%d\\subarea_time_total.txt", dirname, i, "\\subarea", j + 1);
                snprintf(emptied_dirname2, 150, "%s%d%s%d\\subarea_emptied.txt", dirname, i, "\\subarea", j + 1);
            }
        
            else if (condition == 2)
            {
                snprintf(total_dirname2, 150, "%s%d%s%d\\subarea_static_average_total.txt", dirname, i, "\\subarea", j + 1);
                snprintf(avg_dirname2, 150, "%s%d%s%d\\subarea_static_average.txt", dirname, i, "\\subarea", j + 1);
                snprintf(co2_dirname2, 150, "%s%d%s%d\\subarea_static_co2_total.txt", dirname, i, "\\subarea", j + 1);
                snprintf(time_dirname2, 150, "%s%d%s%d\\subarea_static_time_total.txt", dirname, i, "\\subarea", j + 1);
                snprintf(emptied_dirname2, 150, "%s%d%s%d\\subarea_static_emptied.txt", dirname, i, "\\subarea", j + 1);

            }
            
            fclose(output_filepointer);
            output_filepointer = fopen(total_dirname2, "a");
            fprintf(output_filepointer, "%lf ", area_array[i]->subarea_array[j]->total_trash_subarea_avg);
            
            fclose(output_filepointer);
            output_filepointer = fopen(avg_dirname2, "a");
            fprintf(output_filepointer, "%lf ", area_array[i]->subarea_array[j]->average);

            fclose(output_filepointer);
            output_filepointer = fopen(co2_dirname2, "a");
            fprintf(output_filepointer, "%d ", area_array[i]->subarea_array[j]->co2_used);

            fclose(output_filepointer);
            output_filepointer = fopen(time_dirname2, "a");
            fprintf(output_filepointer, "%d ", area_array[i]->subarea_array[j]->time_used);

            fclose(output_filepointer);
            output_filepointer = fopen(emptied_dirname2, "a");
            fprintf(output_filepointer, "%d ", area_array[i]->subarea_array[j]->emptied_subarea_counter);

            for (int z = 0; z < SIZE; z++)
            {
                if (condition == 1)
                {
                    snprintf(total_dirname3, 150, "%s%d%s%d\\trashcan_total_trash.txt", dirname, i, "\\subarea", j + 1);
                    snprintf(avg_dirname3, 150, "%s%d%s%d\\trashcan_trash.txt", dirname, i, "\\subarea", j + 1);
                }

                else if (condition == 2)
                {
                    snprintf(total_dirname3, 150, "%s%d%s%d\\trashcan_static_total_trash.txt", dirname, i, "\\subarea", j + 1);
                    snprintf(avg_dirname3, 150, "%s%d%s%d\\trashcan_static_trash.txt", dirname, i, "\\subarea", j + 1);
                }
                
                fclose(output_filepointer);
                output_filepointer = fopen(total_dirname3, "a");
                fprintf(output_filepointer, "%lf ", area_array[i]->subarea_array[j]->sensorDataTotal[z]);

                fclose(output_filepointer);
                output_filepointer = fopen(avg_dirname3, "a");
                fprintf(output_filepointer, "%lf ", area_array[i]->subarea_array[j]->sensorData[z]);
            }

            fprintf(output_filepointer, "\n");
            fclose(output_filepointer);
            output_filepointer = fopen(total_dirname3, "a");
            fprintf(output_filepointer, "\n");

            fclose(output_filepointer);
            output_filepointer = fopen(total_dirname2, "a");
            fprintf(output_filepointer, "\n");

            fclose(output_filepointer);
            output_filepointer = fopen(avg_dirname2, "a");
            fprintf(output_filepointer, "\n");

            fclose(output_filepointer);
            output_filepointer = fopen(co2_dirname2, "a");
            fprintf(output_filepointer, "\n");

            fclose(output_filepointer);
            output_filepointer = fopen(time_dirname2, "a");
            fprintf(output_filepointer, "\n");

            fclose(output_filepointer);
            output_filepointer = fopen(emptied_dirname2, "a");
            fprintf(output_filepointer, "\n");

        }

        fclose(output_filepointer);
        output_filepointer = fopen(total_dirname, "a");
        fprintf(output_filepointer, "\n");
        fclose(output_filepointer);
        
        output_filepointer = fopen(avg_dirname, "a");
        fprintf(output_filepointer, "\n");
        fclose(output_filepointer);
        
        output_filepointer = fopen(co2_dirname, "a");
        fprintf(output_filepointer, "\n");
        fclose(output_filepointer);

        output_filepointer = fopen(time_dirname, "a");
        fprintf(output_filepointer, "\n");
        fclose(output_filepointer);
        
        output_filepointer = fopen(emptied_dirname, "a");
        fprintf(output_filepointer, "\n");
        fclose(output_filepointer);
    }   
}


void standard_print(area *area_array[NAREA + 1], area *area_array_static[NAREA + 1], int day_counter, int area_number)
{
    printf("DAY: %d\n", day_counter + 1);
    printf("Dynamic: \n");
    for (int i = 0; i < NSUBAREA; i++)
    {
        for (int j = 1; j < NAREA + 1; j++)
        {
            printf("%lf\t", area_array[j]->subarea_array[i]->average);
        }
        printf("\n");
    }

    printf("\n");
    if (area_number > 0)
    {
        printf("Area[%d] emptied\n-----------------\n\n", area_number);
    }
        
    printf("Static: \n");
    for (int i = 0; i < NSUBAREA; i++)
    {
        for (int j = 1; j < NAREA + 1; j++)
        {
            printf("%lf\t", area_array_static[j]->subarea_array[i]->average);
        }
        printf("\n");
    }
    printf("\n");

    if (day_counter > 1 && day_counter % 14 == 0)
    {
        printf("All areas emptied\n\n");
    }
    printf("------------------------------------------------------------------------------------------------------------------------------\n\n");
}

int search_function()
{
    char user_input[20];
    char buffer[20];
    int int_user_input;
    printf("NOTICE: 'q' will quit the program & 'r' will reset to the beginning of searching");
    printf("\nEnter which area you want data for (area1 = '1', area2 = '2' etc. - POSSIBILITIES: 1-%d): ", NAREA);
    scanf("%s", &user_input);
    printf("\n");

    if (strcmp(user_input, "q") == 0)
    {
        return 0;
    }

    for (int i = 1; i < NAREA + 1; i++)
    {
        int_user_input = atoi(user_input);

        if (i == int_user_input)
        {
           return area_search(i);
        }
        
    }
}


int area_search(int i)
{
    char user_input[20];
    char buffer[10];
    int j = 0;
    int int_user_input;
     printf("\nChoose which data you want from Area[%d]:\n"
            "DATA:     \t\tINPUT:\n"
            "Area total\t->\t'to'\n"
            "Area average\t->\t'a'\n"
            "Area CO2\t->\t'c'\n"
            "Area Emptied\t->\t'e'\n"
            "Area Time\t->\t'ti'\n"
            "Subarea \t->\t'1-%d'\n\n", i, NSUBAREA);

    scanf("%s", user_input);
    printf("\n");
    
    if (strcmp(user_input, "q") == 0)
    {
        return 0;
    }

    else if (strcmp(user_input, "r") == 0)
    {
        return 1;
    }
    
    else if (strcmp(user_input, "to") == 0)
    {
        read_single_data(i, j, 1, "area_average_total", 1);
        read_single_data(i, j, 1, "area_static_average_total", 2);
        return 1;
    }
    else if (strcmp(user_input, "a") == 0)
    {
        read_single_data(i, j, 1, "area_average", 1);
        read_single_data(i, j, 1, "area_static_average", 2);
        return 1;
    }
    else if (strcmp(user_input, "c") == 0)
    {
        read_single_data(i, j, 2, "area_co2_total", 1);
        read_single_data(i, j, 2, "area_static_co2_total", 2);
        return 1;
    }
    else if (strcmp(user_input, "e") == 0)
    {
        read_single_data(i, j, 2, "area_emptied", 1);
        read_single_data(i, j, 2, "area_static_emptied", 2);
        return 1;
    }
    else if (strcmp(user_input, "ti") == 0)
    {
        read_single_data(i, j, 2, "area_time_total", 1);
        read_single_data(i, j, 2, "area_static_time_total", 2);
        return 1;
    }

    for (int j = 1; j < NSUBAREA + 1; j++)
    {
        int_user_input = atoi(user_input);
        if (j == int_user_input)
        {
           return subarea_search(i, j);
        }
        
    }
}

int subarea_search(int i, int j)
{
    char user_input[20];
    char buffer[10];
    printf("\nChoose which data you want from Area[%d]->Subarea[%d]:\n"
            "DATA:     \t\t\tINPUT:\n"
            "Subarea total\t\t->\t'to'\n"
            "Subarea average\t\t->\t'a'\n"
            "Subarea CO2\t\t->\t'c'\n"
            "Subarea Emptied\t\t->\t'e'\n"
            "Subarea Time\t\t->\t'ti'\n"
            "Subarea Trashcan\t->\t'tt'\n"
            "Subarea Trashcan Total\t->\t'ttt'\n\n", i, j);
    scanf("%s", user_input);
    printf("\n");

    if (strcmp(user_input, "q") == 0)
    {
        return 0;
    }
    else if (strcmp(user_input, "r") == 0)
    {
        return 1;
    }

    else if (strcmp(user_input, "to") == 0)
    {
        read_single_data(i, j, 1, "subarea_average_total", 1);
        read_single_data(i, j, 1, "subarea_static_average_total", 2);

        return 1;
    }
    else if (strcmp(user_input, "a") == 0)
    {
        read_single_data(i, j, 1, "subarea_average", 1);
        read_single_data(i, j, 1, "subarea_static_average", 2);
        return 1;
    }
    else if (strcmp(user_input, "c") == 0)
    {
        read_single_data(i, j, 2, "subarea_co2_total", 1);
        read_single_data(i, j, 2, "subarea_static_co2_total", 2);
        return 1;
    }
    else if (strcmp(user_input, "e") == 0)
    {
        read_single_data(i, j, 2, "subarea_emptied", 1);
        read_single_data(i, j, 2, "subarea_static_emptied", 2);
        return 1;
    }
    else if (strcmp(user_input, "ti") == 0)
    {
        read_single_data(i, j, 2, "subarea_time_total", 1);
        read_single_data(i, j, 2, "subarea_static_time_total", 2);
        return 1;
    }
     else if (strcmp(user_input, "ttt") == 0)
    {
        read_multiple_data(i, j, "trashcan_total_trash", 1);
        read_multiple_data(i, j, "trashcan_static_total_trash", 2);
        return 1;
    }
     else if (strcmp(user_input, "tt") == 0)
    {
        read_multiple_data(i, j, "trashcan_trash", 1);
        read_multiple_data(i, j, "trashcan_static_trash", 2);
        return 1;
    }
       
}

void read_single_data(int area_number, int subarea_number, int type_choice, char file_name[50], int DynamicOrStatic)
{
    FILE *output_filepointer;

    char dirname[150];
    char newdirname[150];
    double double_holder;
    int int_holder;
    
    snprintf(dirname, 150, "%s\\data\\area", DIRECTORY);

    if (subarea_number > 0)
    {
        snprintf(newdirname, 150, "%s%d\\subarea%d\\%s.txt", dirname, area_number, subarea_number, file_name);
    }
    
    else if (area_number > 0)
    {
        snprintf(newdirname, 150, "%s%d\\%s.txt", dirname, area_number, file_name);
    }
    

    output_filepointer = fopen(newdirname, "r");

    if (DynamicOrStatic == 1) {printf("Dynamic\n");}
    else if (DynamicOrStatic == 2) {printf("Static\n");}

    if (type_choice == 1)
    {
       for (int i = 0; i < DAY_MAX; i++)
        {
            fscanf(output_filepointer, "%lf ", &double_holder);
            printf("Day %d: %5.2lf\n", i + 1, double_holder);
        }
    }

    else if (type_choice == 2)
    {
        for (int i = 0; i < DAY_MAX; i++)
        {
            fscanf(output_filepointer, "%d ", &int_holder);
            printf("Day %d: %d\n", i + 1, int_holder);
        }
    }
    printf("\n");
    if (DynamicOrStatic == 2)
    {
        printf("------------------------------------------------------------------------------------------------------------------------------\n\n");
    }
    

    fclose(output_filepointer);
}

void read_multiple_data(int area_number, int subarea_number, char file_name[50], int DynamicOrStatic)
{
    FILE *output_filepointer;

    char dirname[150];
    char newdirname[150];
    double double_holder;

    snprintf(dirname, 150, "%s\\data\\area", DIRECTORY);
    
    if (subarea_number > 0)
    {
        snprintf(newdirname, 150, "%s%d\\subarea%d\\%s.txt", dirname, area_number, subarea_number, file_name);
    }
    
    else if (area_number > 0)
    {
        snprintf(newdirname, 150, "%s%d\\%s.txt", dirname, area_number, file_name);
    }
    
    output_filepointer = fopen(newdirname, "r");
    
    if (DynamicOrStatic == 1) {printf("Dynamic\n");}
    else if (DynamicOrStatic == 2) {printf("Static\n");}

    for (int i = 0; i < DAY_MAX; i++)
    {
        printf("Day %d:\t", i + 1);
        for (int j = 0; j < SIZE; j++)
        {
            fscanf(output_filepointer, "%lf ", &double_holder);
            printf("[%d]: %5.2lf\t", j, double_holder);
        }
        printf("\n");
    }
    printf("\n");

    if (DynamicOrStatic == 2)
    {
        printf("------------------------------------------------------------------------------------------------------------------------------\n\n");
    }
    
    fclose(output_filepointer);
}