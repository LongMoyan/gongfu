#include <stdlib.h>
#include "task.h"
#include "list.h"

/* current runnig task_tcb */
static task_tcb_t* task_current_running_tcb = NULL;

/* schedule tick */
tick_type_t task_tick = 0;
tick_type_t next_task_unblock_tick = 0;
static tick_type_t task_tick_overflow_count = 0;

/* task pool */
base_type_t task_pool_creat_success = LOGIC_FALSE;
list_t * task_list_ready[TASK_PRIORITY_NUMBER] = {NULL};
list_t * task_list_suspend = NULL;
list_t * task_list_block = NULL;
list_t * task_list_block_overflow = NULL;

/* task registry, all task control blocks are stored in the 
 * registry as a a double linked list. 
 */
task_registry_t * registry;


/*
 * Function name: task_set_wake_up_tick().
 *
 * Set the wake-up ticks for the task blocking.
 *
 * @param list_item The task state list item.
 *
 * @param wake_up_tick Task wake-up ticks.
 *
 * @return The number of current task ticks.
 *
 * \page task_set_wake_up_tick task_set_wake_up_tick
 * \ingroup task
 */
static void task_set_wake_up_tick(list_item_t* list_item, tick_type_t wake_up_tick)
{
    if (NULL == list_item)
    {
        return ;
    }

    list_item->item_value = wake_up_tick;
}

/*
 * Function name: add_current_task_to_block_list().
 *
 * Add the currently runnig task to the block list.
 *
 * @param tick_to_delay The number of ticks for the task needs
 * to be blocked.
 *
 * @param task_can_block_indefinitely Whether the task can be blocked 
 * indefinitely. If LOGIC_TRUE, task can be added to suspend list.
 *
 * \page add_current_task_to_block_list add_current_task_to_block_list
 * \ingroup task
 */
static void add_current_task_to_block_list(tick_type_t tick_to_delay, base_type_t task_can_block_indefinitely)
{
    tick_type_t task_tick_to_wake;
    tick_type_t current_tick = task_tick;

    /* When an indefinite delay is allowed and the delay ticks is equal to MAX_BLOCK_TICK.
     * Add it to the suspend list. 
     */
    if ((MAX_BLOCK_TICK == tick_to_delay) && (LOGIC_FALSE != task_can_block_indefinitely))
    {
        task_add_to_suspend_list(&(task_current_running_tcb->task_state_list_item));
    }
    /* When wake-up ticks does not overflow, add it to block list. Otherwise, add it to overflow block list. */
    else
    {
        task_tick_to_wake = current_tick + tick_to_delay;

        /* The list item will be inserted in wake time order. */
        task_set_wake_up_tick(&(task_current_running_tcb->task_state_list_item), task_tick_to_wake);
        
        task_current_running_tcb->task_delay_flag = TASK_DELAY_ONGOING;
        task_current_running_tcb->task_start_delay_tick_overflow_count = task_tick_overflow_count;
        task_current_running_tcb->task_wake_up_tick = task_tick_to_wake;

        if (task_tick_to_wake < current_tick)
        {
            /* Wake time has overflowed.  Place this item in the overflow list. */
            task_add_to_block_list_overflow(&(task_current_running_tcb->task_state_list_item));
            task_current_running_tcb->last_block_list = task_list_block_overflow_point_get();
        }
        else
        {
            /* The wake time has not overflowed, so the current block list is used. */
            task_add_to_block_list(&(task_current_running_tcb->task_state_list_item));
            task_current_running_tcb->last_block_list = task_list_block_point_get();
        }
    }
}

/*
 * Function name: reset_next_task_unblock_tick().
 *
 * Reset next task unblock time. It must be called when
 * adding or removing items to block list.
 *
 * @param task_tcb The task control block of the task.
 *
 * @return The number of current task ticks.
 *
 * \page reset_next_task_unblock_tick reset_next_task_unblock_tick
 * \ingroup task
 */
static void reset_next_task_unblock_tick(void)
{
    task_tcb_t * task_tcb;
    
    if (LOGIC_TRUE == list_is_empty(task_list_block))
    {
        next_task_unblock_tick = MAX_BLOCK_TICK;
    }
    else
    {
        task_tcb = (task_tcb_t*)list_get_owner_of_first_item(task_list_block);
        next_task_unblock_tick = (&(task_tcb->task_state_list_item))->item_value;
    }
}


base_type_t registry_init()
{
    registry = (task_registry_t*)malloc(sizeof(task_registry_t));
    if (NULL == registry)
    {
        return EXE_FAIL;
    }

    registry->registry_head = NULL;
    registry->task_num = 0;

    return EXE_PASS;
}

base_type_t registry_login(task_tcb_t* task_tcb)
{
    if (NULL == registry)
    {
        return EXE_FAIL;
    }

    /* Insert an empty registry. */
    if (NULL == registry->registry_head)
    {
        registry->registry_head = task_tcb;
        task_tcb->pre_tcb = NULL;
        task_tcb->next_tcb = NULL;
        registry->task_num = 1;
    }
    /* Registry is not empty, and the item will be inserted into the header of the reigstry. */
    else
    {
        registry->registry_head->pre_tcb = task_tcb;
        task_tcb->next_tcb = registry->registry_head;
        registry->registry_head = task_tcb;
        registry->task_num++;
    }

    return EXE_PASS;
}

base_type_t registry_logout(task_tcb_t* task_tcb)
{
    ubase_type_t task_number = registry->task_num;
    if (NULL == registry)
    {
        return EXE_FAIL;
    }

    /* Remove the item in the middle of the registry. */
    if (NULL != task_tcb->pre_tcb && NULL != task_tcb->next_tcb)
    {
        task_tcb->pre_tcb->next_tcb = task_tcb->next_tcb;
        task_tcb->next_tcb->pre_tcb = task_tcb->pre_tcb;
        (registry->task_num)--;
    }
    /* Remove the first item in the registry. */
    else if (NULL == task_tcb->pre_tcb && NULL != task_tcb->next_tcb)
    {
        registry->registry_head = task_tcb->next_tcb;
        task_tcb->next_tcb->pre_tcb = task_tcb->pre_tcb;
        (registry->task_num)--;
    }
    /* Remove the last item in the registry. */
    else if (NULL != task_tcb->pre_tcb && NULL == task_tcb->next_tcb)
    {
        task_tcb->pre_tcb->next_tcb = task_tcb->next_tcb;
        (registry->task_num)--;
    }
    /* Registry is empty. */
    else if (NULL == task_tcb->pre_tcb && NULL == task_tcb->next_tcb)
    {
        registry->registry_head = task_tcb->next_tcb;
        registry->task_num = 0;
    }

    return EXE_PASS;
}


base_type_t task_pool_init()
{
    /* Allocate space for the ready list and initialize the ready list. */
    for (int i = 0; i < TASK_PRIORITY_NUMBER; i++)
    {
        task_list_ready[i] = (list_t *)malloc(sizeof(list_t));
        if (NULL != task_list_ready[i])
        {
            list_init(task_list_ready[i]);
        }
        else
        {
            task_pool_creat_success = LOGIC_FALSE;
            return EXE_FAIL;
        }
    }

    /* Allocate space for the suspend list and initialize the suspend list. */
    task_list_suspend = (list_t *)malloc(sizeof(list_t));
    if (NULL != task_list_suspend)
    {
        list_init(task_list_suspend);
    }
    else
    {
        task_pool_creat_success = LOGIC_FALSE;
        return EXE_FAIL;
    }

    /* Allocate space for the block list and initialize the block list. */
    task_list_block = (list_t*)malloc(sizeof(list_t));
    if (NULL != task_list_block)
    {
        list_init(task_list_block);
    }
    else
    {
        task_pool_creat_success = LOGIC_FALSE;
        return EXE_FAIL;
    }

    /* Allocate space for the overflow block list and initialize the overflow block list. */
    task_list_block_overflow = (list_t*)malloc(sizeof(list_t));
    if (NULL != task_list_block_overflow)
    {
        list_init(task_list_block_overflow);
    }
    else
    {
        task_pool_creat_success = LOGIC_FALSE;
        return EXE_FAIL;
    }

    task_pool_creat_success = LOGIC_TRUE;
    return EXE_PASS;
}

base_type_t task_pool_enter(task_tcb_t * task_tcb)
{
    if (LOGIC_FALSE == task_pool_creat_success || NULL == task_tcb)
    {
        return EXE_FAIL;
    }

    /* Add to different lists according to the different runnig states of tasks. */
    switch (task_tcb->task_state)
    {
        case READY:
            task_add_to_ready_list(&(task_tcb->task_state_list_item));
            task_need_to_be_scheduled_number_increment(task_tcb);
            break;
        case BLOCKED:
            task_add_to_block_list(&(task_tcb->task_state_list_item));
            break;
        case SUSPENDED:
            task_add_to_suspend_list(&(task_tcb->task_state_list_item));
            break;
        default:
            break;
    }
    
    return EXE_PASS;
}

base_type_t task_pool_out(task_tcb_t* task_tcb)
{
    if (LOGIC_FALSE == task_pool_creat_success)
    {
        return EXE_FAIL;
    }

    /* Remove task from the task pool. */
    if (NULL != (&(task_tcb->task_state_list_item))->container)
    {
        list_remove(&(task_tcb->task_state_list_item));
    }

    return EXE_PASS;
}


list_t * task_list_ready_point_get(task_priority_e priority)
{
    for(int i = 0; i < TASK_PRIORITY_NUMBER; i++)
    {
        if(priority == i)
        {
            return task_list_ready[i];
        }
    }

    return NULL;
}

list_t * task_list_block_point_get(void)
{
    return task_list_block;
}

list_t * task_list_block_overflow_point_get(void)
{
    return task_list_block_overflow;
}

list_t * task_list_suspend_point_get(void)
{
    return task_list_suspend;
}

base_type_t task_create
(
    task_function_t task_function,
    const char* const task_name,
    const ubase_type_t stack_size,
    task_priority_e task_priority,
    tick_type_t task_period,
    task_tcb_t * const task_create_tcb
)
{
    static base_type_t task_id = 0;

    task_create_tcb->pre_tcb = NULL;
    task_create_tcb->next_tcb = NULL;

    task_create_tcb->task_id = task_id++;

    task_create_tcb->task_name = task_name;

    /* Initialize the list item. */
    task_create_tcb->task_state_list_item.container = NULL;
    task_create_tcb->task_state_list_item.item_owner = task_create_tcb;
    task_create_tcb->task_state_list_item.item_value = 0;
    task_create_tcb->task_state_list_item.pre_list_item = NULL;
    task_create_tcb->task_state_list_item.next_list_item = NULL;

    /* Task state machine is idle state. */
    task_create_tcb->task_state_machine_state = 0;
    task_create_tcb->task_state_machine_next_state = 0;

    /* Clear message queue. */
    for(int priority = 0; priority < MESSAGE_PRIORITY_NUMBER; priority++)
    {
        task_create_tcb->message_head[priority] = 0;
        task_create_tcb->message_tail[priority] = 0;
        task_create_tcb->message_number[priority] = 0;
    }

    task_create_tcb->need_to_be_scheduled_number = 0;

    task_create_tcb->run_period = task_period;

    task_create_tcb->task_delay_flag = 0;
    task_create_tcb->task_start_delay_tick_overflow_count = 0;
    task_create_tcb->task_wake_up_tick = 0;
    task_create_tcb->last_block_list = NULL;

    task_create_tcb->task_state = READY;

    task_create_tcb->task_priority = task_priority;

    task_create_tcb->task_handler = task_function;

    task_create_tcb->task_stack_size = stack_size;
    task_create_tcb->task_stack = NULL;

    if(EXE_PASS == registry_login(task_create_tcb) && EXE_FAIL == task_pool_enter(task_create_tcb))
    {
        return EXE_PASS;
    }

    return EXE_FAIL;
}

base_type_t task_delete(task_tcb_t * task_to_delete)
{
    if (NULL == task_to_delete)
    {
        return EXE_FAIL;
    }

    /* Delete the task from the registry and the task pool, set 
     * task state to DELETED, and reset next_task_unblock_tick.
     */
    registry_logout(task_to_delete);
    task_pool_out(task_to_delete);
    task_state_set(task_to_delete, DELETED);
    reset_next_task_unblock_tick();

    return EXE_PASS;
}

void task_current_running_tcb_set(task_tcb_t * task_tcb)
{
    task_current_running_tcb = task_tcb;
}

void task_delay(tick_type_t tick_to_delay)
{
    if(0 < tick_to_delay)
    {
        add_current_task_to_block_list(tick_to_delay, LOGIC_FALSE);
    }
}

task_state_e task_state_get(task_tcb_t* task_tcb)
{
    return task_tcb->task_state;
}

void task_state_set(task_tcb_t* task_tcb, task_state_e task_new_state)
{
    if(NULL == task_tcb)
    {
        return ;
    }

    task_tcb->task_state = task_new_state;
}

task_priority_e task_priority_get(const task_tcb_t* task_tcb)
{
    return task_tcb->task_priority;
}

void task_priority_set(task_tcb_t* task_tcb, task_priority_e new_priority)
{
    if(NULL == task_tcb || TASK_PRIORITY_NUMBER <= new_priority)
    {
        return ;
    }

    task_tcb->task_priority = new_priority;
}

void task_suspend(task_tcb_t* task_tcb)
{
    if (NULL == task_tcb)
    {
        return ;
    }

    /* The task has been deleted and can not be suspended. */
    if(DELETED == task_state_get(task_tcb))
    {
        return ;
    }

    task_add_to_suspend_list(&(task_tcb->task_state_list_item));
}

void task_resume(task_tcb_t* task_tcb)
{
    if (NULL == task_tcb)
    {
        return ;
    }

    /* The task has been deleted and can not be resumed. */
    if(DELETED == task_state_get(task_tcb))
    {
        return ;
    }

    /* Clear the message queue and need_to_be_scheduled_number when the task resumes from the suspended state. */
    if(SUSPENDED == task_state_get(task_tcb))
    {
        for (ubase_type_t message_priority = 0; message_priority < MESSAGE_PRIORITY_NUMBER; message_priority++)
        {
            task_tcb->message_head[message_priority] = 0;
            task_tcb->message_tail[message_priority] = 0;
            task_tcb->message_number[message_priority] = 0;
        }
        task_tcb->need_to_be_scheduled_number = 0;
        task_add_to_ready_list(&(task_tcb->task_state_list_item));
    }
}

void task_suspend_all(void)
{
    list_item_t *cur_task_state_item = NULL;

    /* Suspend all tasks in ready list. */
    for (ubase_type_t priority = 0; priority < TASK_PRIORITY_NUMBER; priority++)
    {
        while (LOGIC_FALSE == list_is_empty(task_list_ready[priority]))
        {
            cur_task_state_item = list_get_first_item(task_list_ready[priority]);
            task_suspend(cur_task_state_item->item_owner);
        }
    }

    /* Suspend all tasks in block list. */
    while (LOGIC_FALSE == list_is_empty(task_list_block))
    {
        cur_task_state_item = list_get_first_item(task_list_block);
        task_suspend(cur_task_state_item->item_owner);
    }

    /* Suspend all tasks in overflow block list. */
    while (LOGIC_FALSE == list_is_empty(task_list_block_overflow))
    {
        cur_task_state_item = list_get_first_item(task_list_block_overflow);
        task_suspend(cur_task_state_item->item_owner);
    }
}

void task_resume_all(void)
{
    list_item_t* cur_task_state_item = NULL;

    /* Resume all tasks in suspend list. */
    while (LOGIC_FALSE == list_is_empty(task_list_suspend))
    {
        cur_task_state_item = list_get_first_item(task_list_suspend);
        task_resume(cur_task_state_item->item_owner);
    }
}

base_type_t task_add_to_ready_list(list_item_t * task_state_list_item)
{
    if(NULL == task_state_list_item || NULL == task_state_list_item->item_owner)
    {
        return EXE_FAIL;
    }
    
    /* Remove from the original list and insert ready list. */
    if(NULL != task_state_list_item->container)
    {
        list_remove(task_state_list_item);
    }
    list_insert_end(task_list_ready[task_priority_get(task_state_list_item->item_owner)], task_state_list_item);
    task_state_set(task_state_list_item->item_owner, READY);
    reset_next_task_unblock_tick();
    return EXE_PASS;
}

base_type_t task_add_to_suspend_list(list_item_t * task_state_list_item)
{
    if(NULL == task_state_list_item || NULL == task_state_list_item->item_owner)
    {
        return EXE_FAIL;
    }

    /* Remove from the original list and insert suspend list. */
    if(NULL != task_state_list_item->container)
    {
        list_remove(task_state_list_item);
    }
    list_insert_end(task_list_suspend, task_state_list_item);
    task_state_set(task_state_list_item->item_owner, SUSPENDED);
    reset_next_task_unblock_tick();
    return EXE_PASS;
}

base_type_t task_add_to_block_list(list_item_t * task_state_list_item)
{
    if(NULL == task_state_list_item || NULL == task_state_list_item->item_owner)
    {
        return EXE_FAIL;
    }

    /* Remove from the original list and insert block list. */
    if(NULL != task_state_list_item->container)
    {
        list_remove(task_state_list_item);
    }
    list_insert(task_list_block, task_state_list_item);
    task_state_set(task_state_list_item->item_owner, BLOCKED);
    reset_next_task_unblock_tick();
    return EXE_PASS;
}

base_type_t task_add_to_block_list_overflow(list_item_t * task_state_list_item)
{
    if(NULL == task_state_list_item || NULL == task_state_list_item->item_owner)
    {
        return EXE_FAIL;
    }

    /* Remove from the original list and insert overflow block list. */
    if(NULL != task_state_list_item->container)
    {
        list_remove(task_state_list_item);
    }
    list_insert(task_list_block_overflow, task_state_list_item);
    task_state_set(task_state_list_item->item_owner, BLOCKED);
    reset_next_task_unblock_tick();
    return EXE_PASS;
}

ubase_type_t task_number_get(void)
{
    if (NULL == registry)
    {
        return 0;
    }
    return registry->task_num;
}

const char * task_name_get(task_tcb_t* task_tcb)
{
    return task_tcb->task_name;
}

tick_type_t task_tick_increment(void)
{
    task_tcb_t* task_tcb = NULL;
    list_t* temp_list = NULL;
    tick_type_t item_value = 0;
    tick_type_t task_current_tick = 0;

    task_tick++;
    task_current_tick = task_tick;
    
    /* Swap task_list_block and task_list_block_overflow when task counters overflow. */
    if (0 == task_current_tick)
    {
        temp_list = task_list_block;
        task_list_block = task_list_block_overflow;
        task_list_block_overflow = temp_list;
        task_tick_overflow_count++;
        reset_next_task_unblock_tick();
    }

    /* When the task end blocking, add it to the ready list. */
    if(task_current_tick >= next_task_unblock_tick)
    {
        while(1)
        {
            if (LOGIC_TRUE == list_is_empty(task_list_block))
            {
                next_task_unblock_tick = MAX_BLOCK_TICK;
                break;
            }
            else
            {
                task_tcb = (task_tcb_t *) list_get_owner_of_first_item(task_list_block);
                item_value = (&(task_tcb->task_state_list_item))->item_value;

                if (task_current_tick < item_value)
                {
                    next_task_unblock_tick = item_value;
                    break;
                }

                /* It is time to remove the item from the Block list and insert it into Ready list. */
                task_add_to_ready_list(&(task_tcb->task_state_list_item));
                task_need_to_be_scheduled_number_increment(task_tcb);
                task_tcb->task_delay_flag = TASK_DELAY_ENDING;
            }
        }
    }
    return task_tick;
}

tick_type_t task_tick_count_get(void)
{
    return task_tick;
}

tick_type_t task_tick_overflow_count_get(void)
{
    return task_tick_overflow_count;
}


base_type_t task_message_is_empty(task_tcb_t * task_tcb, message_priority_e message_priority)
{
    return (task_tcb->message_number[message_priority]) ? LOGIC_FALSE : LOGIC_TRUE;
}

base_type_t task_message_transmit(task_tcb_t * task_tcb, base_type_t transmit_message, message_priority_e message_priority)//Affect task pool
{
    if(NULL == task_tcb)
    {
        return EXE_FAIL;
    }

    /* Task message queue stores messages. */
    task_tcb->receive_message[message_priority][task_tcb->message_tail[message_priority]] = transmit_message;
    
    /* The task message is full, overwriting the oldest message. */
    if(MAX_MESSAGE_NUMBER == task_tcb->message_number[message_priority])
    {
        (task_tcb->message_tail[message_priority])++;
        (task_tcb->message_head[message_priority])++;
    }
    /* The task message is not full, add one to the number of messages. */
    else
    {
        (task_tcb->message_tail[message_priority])++;
        (task_tcb->message_number[message_priority])++;
    }

    /* Task message queue header overflows, reset it to zero. */
    if(MAX_MESSAGE_NUMBER == task_tcb->message_head[message_priority])
    {
        task_tcb->message_head[message_priority] = 0;
    }

    /* Task message queue tail overflows, reset it to zero. */
    if(MAX_MESSAGE_NUMBER == task_tcb->message_tail[message_priority])
    {
        task_tcb->message_tail[message_priority] = 0;
    }

    /* Insert the task into the ready list when the task is blocked. */
    task_need_to_be_scheduled_number_increment(task_tcb);
    if(BLOCKED == task_state_get(task_tcb) || IDLE == task_state_get(task_tcb))
    {
        task_add_to_ready_list(&(task_tcb->task_state_list_item));
        task_state_set(task_tcb, READY);
    }
    
    return EXE_PASS;
}

base_type_t task_message_receive(task_tcb_t * task_tcb)
{
    base_type_t oldest_message = TASK_MESSAGE_IS_EMPTY;

    for(int priority = 0; priority < MESSAGE_PRIORITY_NUMBER; priority++)
    {
        /* When the message queue of current priority is empty, enter the message queue of lower priority. */
        if (LOGIC_TRUE == task_message_is_empty(task_tcb, priority))
        {
            continue;
        }

        /* Get the oldest message of task message queue. */
        oldest_message = task_tcb->receive_message[priority][task_tcb->message_head[priority]];
        (task_tcb->message_number[priority])--;
        (task_tcb->message_head[priority])++;
        if (MAX_MESSAGE_NUMBER == task_tcb->message_head[priority])
        {
            task_tcb->message_head[priority] = 0;
        }

        return oldest_message;
    }

    return oldest_message;
}

ubase_type_t task_message_number(task_tcb_t * task_tcb, message_priority_e message_priority)
{
    return task_tcb->message_number[message_priority];
}


base_type_t task_does_not_need_to_be_scheduled(task_tcb_t * task_tcb)
{
    return (task_tcb->need_to_be_scheduled_number) ? LOGIC_FALSE : LOGIC_TRUE;
}

void task_need_to_be_scheduled_number_increment(task_tcb_t * task_tcb)
{
    (task_tcb->need_to_be_scheduled_number)++;
}

void task_need_to_be_scheduled_number_reduction(task_tcb_t * task_tcb)
{
    if(0 == task_tcb->need_to_be_scheduled_number)
    {
        return ;
    }

    (task_tcb->need_to_be_scheduled_number)--;
}


base_type_t task_state_machine_state_get(task_tcb_t * task_tcb)
{
    return task_tcb->task_state_machine_state;
}

base_type_t task_state_machine_state_set(task_tcb_t * task_tcb, base_type_t new_state)
{
    if(NULL == task_tcb)
    {
        return EXE_FAIL;
    }
    task_tcb->task_state_machine_state = new_state;
    return EXE_PASS;
}

base_type_t task_state_machine_next_state_get(task_tcb_t * task_tcb)
{
    return task_tcb->task_state_machine_next_state;
}

base_type_t task_state_machine_next_state_set(task_tcb_t * task_tcb, base_type_t new_state)
{
    if(NULL == task_tcb)
    {
        return EXE_FAIL;
    }
    task_tcb->task_state_machine_next_state = new_state;
    return EXE_PASS;
}

base_type_t task_state_machine_state_transition(task_tcb_t * task_tcb)
{
    if(NULL == task_tcb)
    {
        return EXE_FAIL;
    }
    task_tcb->task_state_machine_state = task_tcb->task_state_machine_next_state;
    return EXE_PASS;
}
