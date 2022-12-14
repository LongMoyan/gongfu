#include <stdlib.h>
#include "scheduler.h"
#include "task.h"

static scheduler_state_e scheduler_state = UNINITIALIZED;

void scheduler_init(void)
{
    /* initialize registry and task pool */
    if (EXE_PASS == registry_init() && EXE_PASS == task_pool_init())
    {
        scheduler_state = INITIALIZED;
    }
}

void scheduler_execute_task(void* ready_list)
{
    list_item_t* cur_task_state_item = NULL;
    tick_type_t current_ticks = 0;

    /* Get the first ready task. */
    cur_task_state_item = list_get_first_item(ready_list);

    /* Remove it from the ready list and set the task state to RUNNING. */
    list_remove(cur_task_state_item);//Affect task pool
    task_state_set(cur_task_state_item->item_owner, RUNNING);

    /* Set current RUNNING task control block. */
    task_current_running_tcb_set(cur_task_state_item->item_owner);

    /* Execute task processing function. */
    (((task_tcb_t *)(cur_task_state_item->item_owner))->task_handler)(cur_task_state_item->item_owner);

    /* Reduce the number of times it needs to be scheduled. */
    task_need_to_be_scheduled_number_reduction((task_tcb_t*)(cur_task_state_item->item_owner));

    /* When the task staus is not changed(no task deletion, blocking, etc.) during execution task processing 
     * function, check whether the task still needs to be scheduled. If necessary, add it to the ready list.
     * Otherwize, check whether it is periodic task. If yes, add it to block list. */
    if(RUNNING == task_state_get(cur_task_state_item->item_owner))
    {
        if (LOGIC_FALSE == task_does_not_need_to_be_scheduled((task_tcb_t *)(cur_task_state_item->item_owner)))
        {
            task_add_to_ready_list(cur_task_state_item);//Affect task pool
        }
        else if(TASK_DELAY_ONGOING == ((task_tcb_t *)(cur_task_state_item->item_owner))->task_delay_flag)
        {
            current_ticks = task_tick_count_get();
            if(((task_tcb_t *)(cur_task_state_item->item_owner))->task_start_delay_tick_overflow_count == task_tick_overflow_count_get())
            {
                if(((task_tcb_t *)(cur_task_state_item->item_owner))->task_wake_up_tick > current_ticks)
                {
                    cur_task_state_item->item_value = ((task_tcb_t *)(cur_task_state_item->item_owner))->task_wake_up_tick;
                    task_add_to_block_list(cur_task_state_item); // Affect task pool
                }
                else
                {
                    ((task_tcb_t *)(cur_task_state_item->item_owner))->task_delay_flag = TASK_DELAY_ENDING;
                    task_add_to_ready_list(cur_task_state_item); // Affect task pool
                }
            }
            else
            {
                if(((task_tcb_t *)(cur_task_state_item->item_owner))->task_wake_up_tick > current_ticks)
                {
                    if((((task_tcb_t *)(cur_task_state_item->item_owner))->last_block_list == task_list_block_point_get()))
                    {
                        ((task_tcb_t *)(cur_task_state_item->item_owner))->task_delay_flag = TASK_DELAY_ENDING;
                        task_add_to_ready_list(cur_task_state_item); // Affect task pool
                    }
                    else
                    {
                        cur_task_state_item->item_value = ((task_tcb_t *)(cur_task_state_item->item_owner))->task_wake_up_tick;
                        task_add_to_block_list(cur_task_state_item); // Affect task pool
                    }
                }
                else
                {
                    ((task_tcb_t *)(cur_task_state_item->item_owner))->task_delay_flag = TASK_DELAY_ENDING;
                    task_add_to_ready_list(cur_task_state_item); // Affect task pool
                }
            }
        }
        else if (APERIODIC_TASK != ((task_tcb_t *)(cur_task_state_item->item_owner))->run_period)
        {
            /* Periodic tasks need to insert bolck list. If its wake-up time overflows, insert it into the block
             * overflow list. Otherwise, insert it into the block list. */
            current_ticks = task_tick_count_get();
            cur_task_state_item->item_value = current_ticks + ((task_tcb_t *)(cur_task_state_item->item_owner))->run_period;
            if(current_ticks < cur_task_state_item->item_value)
            {
                task_add_to_block_list(cur_task_state_item);//Affect task pool
            }
            else
            {
                task_add_to_block_list_overflow(cur_task_state_item);//Affect task pool
            }
        }
        else
        {
            /* Set the task state to IDLE, which means that the task is not in the task pool. */
            task_state_set(cur_task_state_item->item_owner, IDLE);
        }
    }
}

void scheduler_start(void)
{
    ubase_type_t ready_number[TASK_PRIORITY_NUMBER] = {0};
    list_t* ready_list = NULL;

    if (INITIALIZED == scheduler_state)
    {
        scheduler_state = DISPATCHING;
    }

    while (DISPATCHING == scheduler_state)
    {
        for(int task_priority = 0; task_priority < TASK_PRIORITY_NUMBER; task_priority++)
        {
            ready_list = task_list_ready_point_get(task_priority);
            
            /* TASK_PRIORITY_HIGH task must be handled first.  */
            if (TASK_PRIORITY_HIGH == task_priority)
            {
                while (LOGIC_FALSE == list_is_empty(ready_list))
                {
                    scheduler_execute_task(ready_list);//Affect task pool
                }
            }
            /* Other priority tasks are processed when there are no TASK_PRIORITY_HIGH ready tasks. */
            else
            {
                if(LOGIC_TRUE == list_is_empty(ready_list))
                {
                    continue;
                }
                scheduler_execute_task(ready_list);//Affect task pool
                break;
            }
        }
    }
}

void scheduler_end()
{
    scheduler_state = STOPPED;
}
