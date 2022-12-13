#include <stdio.h>
#include <time.h>
#include "scheduler.h"
#include "list.h"
#include "task.h"
#include "my_timer.h"

void systick_handler(void);

void task1(void* task_tcb);
task_tcb_t task1_tcb;
void task2(void* task_tcb);
task_tcb_t task2_tcb;
void task3(void* task_tcb);
task_tcb_t task3_tcb;
void task4(void* task_tcb);
task_tcb_t task4_tcb;
void task5(void* task_tcb);
task_tcb_t task5_tcb;
void task6(void* task_tcb);
task_tcb_t task6_tcb;
void task7(void* task_tcb);
task_tcb_t task7_tcb;
void task8(void* task_tcb);
task_tcb_t task8_tcb;
void task9(void* task_tcb);
task_tcb_t task9_tcb;
void task10(void* task_tcb);
task_tcb_t task10_tcb;

extern list_t* task_list_ready[TASK_PRIORITY_NUMBER];
extern list_t* task_list_suspend;
extern list_t* task_list_block;
extern list_t * task_list_ready[3];
int main()
{
    UserTimerSet(1, systick_handler);
    scheduler_init();

    task_create(task1, "task1", 0, TASK_PRIORITY_HIGH, 10, &task1_tcb);
    task_create(task2, "task2", 0, TASK_PRIORITY_HIGH, 20, &task2_tcb);
    task_create(task3, "task3", 0, TASK_PRIORITY_HIGH, 30, &task3_tcb);
    task_create(task4, "task4", 0, TASK_PRIORITY_HIGH, 40, &task4_tcb);
    task_create(task5, "task5", 0, TASK_PRIORITY_MID, 50, &task5_tcb);
    task_create(task6, "task6", 0, TASK_PRIORITY_MID, 60, &task6_tcb);
    task_create(task7, "task7", 0, TASK_PRIORITY_MID, 70, &task7_tcb);
    task_create(task8, "task8", 0, TASK_PRIORITY_LOW, 80, &task8_tcb);
    task_create(task9, "task9", 0, TASK_PRIORITY_LOW, 90, &task9_tcb);
    task_create(task10, "task10", 0, TASK_PRIORITY_LOW, 100, &task10_tcb);

    scheduler_start();

    return 0;
}


void task1(void* task_tcb)
{
    base_type_t cur_tick = task_tick_count_get();
    printf("%lu:task1 is working!\n", cur_tick);
    return;
}
void task2(void* task_tcb)
{
    base_type_t cur_tick = task_tick_count_get();
    printf("%lu:task2 is working!\n", cur_tick);
    return;
}
void task3(void* task_tcb)
{
    base_type_t cur_tick = task_tick_count_get();
    printf("%lu:task3 is working!\n", cur_tick);
    return;
}
void task4(void* task_tcb)
{
    base_type_t cur_tick = task_tick_count_get();
    printf("%lu:task4 is working!\n", cur_tick);
    return;
}
void task5(void* task_tcb)
{
    base_type_t cur_tick = task_tick_count_get();
    printf("%lu:task5 is working!\n", cur_tick);
    return;
}
void task6(void* task_tcb)
{
    base_type_t cur_tick = task_tick_count_get();
    printf("%lu:task6 is working!\n", cur_tick);
    return;
}
void task7(void* task_tcb)
{
    base_type_t cur_tick = task_tick_count_get();
    printf("%lu:task7 is working!\n", cur_tick);
    return;
}
void task8(void* task_tcb)
{
    base_type_t cur_tick = task_tick_count_get();
    printf("%lu:task8 is working!\n", cur_tick);
    return;
}
void task9(void* task_tcb)
{
    base_type_t cur_tick = task_tick_count_get();
    printf("%lu:task9 is working!\n", cur_tick);
    return;
}
void task10(void* task_tcb)
{
    base_type_t cur_tick = task_tick_count_get();
    printf("%lu:task10 is working!\n", cur_tick);
    return;
}

void systick_handler(void)
{
    task_tick_increment();//Affect task pool
    //if (task_tick_count_get() == 500)
    //{
    //    task_delete(&task1_tcb);
    //}
}
