#ifndef _SCHEDULER_TASK_H_
#define _SCHEDULER_TASK_H_

#include "scheduler.h"
#include "list.h"


/* Task is aperiodic. */
#define APERIODIC_TASK 0

/* Task delay falg. */
#define TASK_DELAY_ONGOING 1
#define TASK_DELAY_ENDING 0


/* Task states returned. */
enum _task_state
{
    IDLE = 0,       /* The task being queried is IDLE and not in the task pool. */
    RUNNING,        /* The task being queried is RUNNING and not in the task pool. */
    READY,          /* The task being queried is READY and in a ready list. */
    BLOCKED,        /* The task being queried is BLOCKED and in the block list. */
    SUSPENDED,      /* The task being queried is SUSPENDED and in the suspend list. */
    DELETED,        /* The task being queried is DELETED and not in the task pool.. */
};
typedef enum _task_state task_state_e;

/* task TCB struct */
struct _task_tcb
{
    struct _task_tcb * pre_tcb;                                                     /* A pointer to the previous task's tcb. */
    struct _task_tcb * next_tcb;                                                    /* A pointer to the next task's tcb. */

    base_type_t task_id;                                                            /* The id number of task, it must be unique. */
    const char* task_name;                                                          /* The name of task. */

    list_item_t task_state_list_item;                                               /* The list item of task. It will be inserted into ready list when task is ready. Similarly, when the task is in another state, it will be inserted into the corresponding list. */

    base_type_t task_state_machine_state;                                           /* The current state of state machine. Each task has a state machine. */
    base_type_t task_state_machine_next_state;                                      /* The next state of state machine. */

    base_type_t receive_message[MESSAGE_PRIORITY_NUMBER][MAX_MESSAGE_NUMBER];       /* The message queue of task. Message are also prioritized, and different message correspond to a queue. */
    base_type_t message_head[MESSAGE_PRIORITY_NUMBER];                              /* The head position of message queue. */
    base_type_t message_tail[MESSAGE_PRIORITY_NUMBER];                              /* The tail position of message queue. */
    ubase_type_t message_number[MESSAGE_PRIORITY_NUMBER];                           /* The number of message queue. */

    ubase_type_t need_to_be_scheduled_number;                                       /* When task is need to be scheduled, it will not be zero. */

    tick_type_t run_period;                                                         /* The runnig period of task. When task is period task, it will be periodically inserted to ready list. */

    base_type_t task_delay_flag;
    tick_type_t task_start_delay_tick_overflow_count;                               /* The number of overflow when task start to delay. It will be used to distinguish the scheduler counter overflow when task is reinserted into corresponding list. */
    tick_type_t task_wake_up_tick;                                                  /* The wake-up tick that the task should wake-up. */
    list_t * last_block_list;

    task_state_e task_state;                                                        /* The state of task. */

    task_priority_e task_priority;                                                  /* The priority of task. */

    void (*task_handler)(struct _task_tcb* task_tcb);                               /* The function of task. */

    uint8_t* task_stack;                                                            /* The statck of task. Reserved. */
    ubase_type_t task_stack_size;
};
typedef struct _task_tcb task_tcb_t;

struct _task_registry
{
    task_tcb_t * registry_head;             /* The head of registry. The registry is a double linked list containing all task control blocks. */
    ubase_type_t task_num;                  /* The number of tasks that are created. */
};
typedef struct _task_registry task_registry_t;


/*
 * Must be called before creating a task and starting scheduling. The 
 * registry is a double linked list containing all task control blocks.
 *
 * @return EXE_PASS when initialization success or EXE_FAIL when 
 * initialization fail.
 *
 * \page registry_init registry_init
 * \ingroup registry
 */
base_type_t registry_init(void);

/*
 * Insert a task control block into the registry.
 *
 * @param task_tcb The task control block of task to be registered.
 *
 * @return EXE_PASS when the registration is successful or EXE_FAIL 
 * when the registration fails.
 *
 * \page registry_login registry_login
 * \ingroup registry
 */
base_type_t registry_login(task_tcb_t* task_tcb);

/*
 * Remove a task control block from the registry.
 *
 * @param task_tcb The task control block of task that needs to be 
 * unregistered.
 *
 * @return EXE_PASS when logout succeeds or EXE_FAIL when logout fails.
 *
 * \page registry_logout registry_logout
 * \ingroup registry
 */
base_type_t registry_logout(task_tcb_t* task_tcb);

/*
 * Must be called before creating a task and starting scheduling. The 
 * task pool consists of multiple ready lists(one priority corresponds
 * to one ready list), tow block lists(block list and overflow block 
 * list) and a suspend list.
 *
 * @return EXE_PASS when initialization success or EXE_FAIL when 
 * initialization fail.
 *
 * \page task_pool_init task_pool_init
 * \ingroup task_pool
 */
base_type_t task_pool_init(void);

/*
 * Insert a task into the task pool.
 *
 * @param task_tcb The task control block of task that needs to be 
 * inserted into the task pool.
 *
 * @return EXE_PASS when the insertion is successful or EXE_FAIL when
 * the insertion fails.
 *
 * \page task_pool_enter task_pool_enter
 * \ingroup task_pool
 */
base_type_t task_pool_enter(task_tcb_t* task_tcb);

/*
 * Remove a task from the task pool.
 *
 * @param task_tcb The task control block of task that needs to be 
 * removed from the task pool.
 *
 * @return EXE_PASS when removal succeeds or EXE_FAIL when removal 
 * fails.
 *
 * \page task_pool_out task_pool_out
 * \ingroup task_pool
 */
base_type_t task_pool_out(task_tcb_t* task_tcb);


/*
 * Get pointer to ready list of the task pool.
 *
 * @param priority The priority of task.
 *
 * @return The pointer to the ready list of the corresponding priority.
 *
 * \page task_list_ready_point_get task_list_ready_point_get
 * \ingroup task_pool
 */
list_t * task_list_ready_point_get(task_priority_e priority);

/*
 * Get pointer to block list of the task pool.
 *
 * @return The pointer to the block list.
 *
 * \page task_list_block_point_get task_list_block_point_get
 * \ingroup task_pool
 */
list_t * task_list_block_point_get(void);

/*
 * Get pointer to overflow block list of the task pool.
 *
 * @return The pointer to the overflow block list.
 *
 * \page task_list_block_overflow_point_get task_list_block_overflow_point_get
 * \ingroup task_pool
 */
list_t * task_list_block_overflow_point_get(void);

/*
 * Get pointer to suspend block list of the task pool.
 *
 * @return The pointer to the suspend block list.
 *
 * \page task_list_suspend_point_get task_list_suspend_point_get
 * \ingroup task_pool
 */
list_t * task_list_suspend_point_get(void);


/*
 * Create a task. The created task will be registered in the registry
 * and added to the task pool
 *
 * @param task_function The proceessing function of the task.
 *
 * @param task_name The name of the task.
 *
 * @param stack_size The stack size of task stack. Stack not implemented.
 *
 * @param task_priority The priority of the task.
 *
 * @param task_period The period of the task. When it is zero, the task
 * is aperiodic.
 *
 * @param task_create_tcb The task control block of the task.
 *
 * @return EXE_PASS when the creation is successful or EXE_FAIL when the 
 * creation fails.
 *
 * \page task_create task_create
 * \ingroup task
 */
base_type_t task_create(
    task_function_t task_function,
    const char* const task_name,
    const ubase_type_t stack_size,
    task_priority_e task_priority,
    tick_type_t task_period,
    task_tcb_t* const task_create_tcb
);

/*
 * Delete a task. The deleted task will be unregistered from the registry 
 * and removed from the task pool.
 * 
 * @param task_to_delete The task control block of the task.
 *
 * @return EXE_PASS when the deletion is successful or EXE_FAIL when the 
 * deletion fails.
 *
 * \page task_delete task_delete
 * \ingroup task
 */
base_type_t task_delete(task_tcb_t* task_to_delete);

/*
 * Bolck currently running task.
 * 
 * @param tick_to_delay The number of ticks that currently running task 
 * need to be blocked.
 *
 * \page task_delay task_delay
 * \ingroup task
 */
void task_delay(tick_type_t tick_to_delay);

/*
 * Set the currently running task.
 * 
 * @param task_tcb The task control block of the currently running task.
 *
 * \page task_current_running_tcb_set task_current_running_tcb_set
 * \ingroup task
 */
void task_current_running_tcb_set(task_tcb_t* task_tcb);

/*
 * Get the current running status of the task.
 * 
 * @param task_tcb The task control block of the task.
 *
 * @return The current running status of the task.
 *
 * \page task_state_get task_state_get
 * \ingroup task
 */
task_state_e task_state_get(task_tcb_t* task_tcb);

/*
 * Set the current running status of the task.
 * 
 * @param task_tcb The task control block of the task.
 * 
 * @param task_new_state The status that the task needs to be set.
 *
 * \page task_state_set task_state_set
 * \ingroup task
 */
void task_state_set(task_tcb_t* task_tcb, task_state_e task_new_state);

/*
 * Get the priority of the task.
 * 
 * @param task_tcb The task control block of the task.
 *
 * @return The priority of the task.
 *
 * \page task_priority_get task_priority_get
 * \ingroup task
 */
task_priority_e task_priority_get(const task_tcb_t* task_tcb);

/*
 * Set the priority of the task.
 * 
 * @param task_tcb The task control block of the task.
 * 
 * @param new_priority The priority that the task needs to be set.
 *
 * \page task_priority_set task_priority_set
 * \ingroup task
 */
void task_priority_set(task_tcb_t* task_tcb, task_priority_e new_priority);

/*
 * Suspend the task. The suspended task will be added to the 
 * suspended list.
 * 
 * @param task_tcb The task control block of the task that needs 
 * to be suspended.
 *
 * \page task_suspend task_suspend
 * \ingroup task
 */
void task_suspend(task_tcb_t* task_tcb);

/*
 * Resume the task. The resumed task will be added to the 
 * ready list.
 * 
 * @param task_tcb The task control block of the task that needs 
 * to be resumed.
 *
 * \page task_resume task_resume
 * \ingroup task
 */
void task_resume(task_tcb_t* task_tcb);

/*
 * Suspend all tasks. All tasks will be added to the suspend 
 * list.
 *
 * \page task_suspend_all task_suspend_all
 * \ingroup task
 */
void task_suspend_all(void);

/*
 * Resume all tasks. All tasks will be added to the ready list.
 *
 * \page task_resume_all task_resume_all
 * \ingroup task
 */
void task_resume_all(void);

/*
 * Insert a list item to ready list.
 * 
 * @param task_state_list_item The task state list item that needs 
 * to be inserted into the ready list.
 *
 * @return EXE_PASS when the insertion is successful, EXE_FAIL when 
 * the insertion fails.
 *
 * \page task_add_to_ready_list task_add_to_ready_list
 * \ingroup task
 */
base_type_t task_add_to_ready_list(list_item_t * task_state_list_item);

/*
 * Insert a list item to suspend list.
 * 
 * @param task_state_list_item The task state list item that needs 
 * to be inserted into the suspend list.
 *
 * @return EXE_PASS when the insertion is successful, EXE_FAIL when 
 * the insertion fails.
 *
 * \page task_add_to_suspend_list task_add_to_suspend_list
 * \ingroup task
 */
base_type_t task_add_to_suspend_list(list_item_t * task_state_list_item);

/*
 * Insert a list item to block list.
 * 
 * @param task_state_list_item The task state list item that needs 
 * to be inserted into the block list.
 *
 * @return EXE_PASS when the insertion is successful, EXE_FAIL when 
 * the insertion fails.
 *
 * \page task_add_to_block_list task_add_to_block_list
 * \ingroup task
 */
base_type_t task_add_to_block_list(list_item_t * task_state_list_item);

/*
 * Insert a list item to overflow block list.
 * 
 * @param task_state_list_item The task state list item that needs 
 * to be inserted into the overflow block list.
 *
 * @return EXE_PASS when the insertion is successful, EXE_FAIL when 
 * the insertion fails.
 *
 * \page task_add_to_block_list_overflow task_add_to_block_list_overflow
 * \ingroup task
 */
base_type_t task_add_to_block_list_overflow(list_item_t * task_state_list_item);

/*
 * Get the number of all tasks.
 * 
 * @return The number of all tasks.
 *
 * \page task_number_get task_number_get
 * \ingroup task
 */
ubase_type_t task_number_get(void);

/*
 * Get pointer to the task name.
 * 
 * @param task_tcb The task control block of the task.
 *
 * @return Pointer to the task name.
 *
 * \page task_name_get task_name_get
 * \ingroup task
 */
const char * task_name_get(task_tcb_t* task_tcb);

/*
 * Add one to task counter tick. Swap task_list_block and 
 * task_list_block_overflow when task counters overflow. 
 * Check whether there are blocking tasks ending blocking, 
 * and add them to ready list.
 * 
 * @param task_tcb The task control block of the task.
 *
 * @return The number of current task ticks.
 *
 * \page task_tick_increment task_tick_increment
 * \ingroup task
 */
tick_type_t task_tick_increment(void);

/*
 * Get the current number of ticks for the task counter. 
 *
 * @return The number of current task ticks.
 *
 * \page task_tick_count_get task_tick_count_get
 * \ingroup task
 */
tick_type_t task_tick_count_get(void);

/*
 * Get the current number of overflow for the task counter. 
 *
 * @return The number of current task counter overflow.
 *
 * \page task_tick_overflow_count_get task_tick_overflow_count_get
 * \ingroup task
 */
tick_type_t task_tick_overflow_count_get(void);


/*
 * Transmit a message to the specified task. Message are 
 * also prioritized, and different message correspond to 
 * a queue. High priority messages will be received first 
 * when task_message_receive is called.
 *
 * @param task_tcb The task control block of the task.
 *
 * @param transmit_message The message transmitted to the task.
 *
 * @param message_priority The priority of transmitting message.
 *
 * @return EXE_PASS when the transmission is successful or 
 * EXE_FAIL when the transmission fails.
 *
 * \page task_message_transmit task_message_transmit
 * \ingroup task_message
 */
base_type_t task_message_transmit(task_tcb_t* task_tcb, base_type_t transmit_message, message_priority_e message_priority);

/*
 * Receive a message from the specified task. We will get the highest
 * priority and the earliest message add to the message queue.
 *
 * @param task_tcb The task control block of the task.
 *
 * @return The earliest message add to the message queue.
 *
 * \page task_message_receive task_message_receive
 * \ingroup task_message
 */
base_type_t task_message_receive(task_tcb_t* task_tcb);

/*
 * Check whether the message queue of the task is empty.
 *
 * @param task_tcb The task control block of the task.
 *
 * @param message_priority The priority of transmitting message.
 *
 * @return LOGIC_TRUE when the message queue is empty or 
 * LOGIC_FALSE when the message queue is not empty.
 *
 * \page task_message_is_empty task_message_is_empty
 * \ingroup task_message
 */
base_type_t task_message_is_empty(task_tcb_t * task_tcb, message_priority_e message_priority);

/*
 * Get the number of messages in the task message queue.
 *
 * @param task_tcb The task control block of the task.
 *
 * @param message_priority The priority of transmitting message.
 *
 * @return The number of messages in the task message queue.
 *
 * \page task_message_number task_message_number
 * \ingroup task_message
 */
ubase_type_t task_message_number(task_tcb_t * task_tcb, message_priority_e message_priority);


/*
 * Check whether the task needs to be scheduled. The variable 
 * need_to_be_scheduled_number in the task control block 
 * indicates the number of times a task needs to be scheduled. 
 * Each time a task receives a message, it will add one to 
 * indicate the number of times it needs to be scheduled.
 *
 * @param task_tcb The task control block of the task.
 *
 * @return LOGIC_TRUE when the task does not need to be scheduled
 * or LOGIC_FALSE when the task needs to be scheduled.
 *
 * \page task_does_not_need_to_be_scheduled task_does_not_need_to_be_scheduled
 * \ingroup task_scheduled_number
 */
base_type_t task_does_not_need_to_be_scheduled(task_tcb_t * task_tcb);

/*
 * Add one to the number of tasks to be scheduled. It will be called 
 * when the task receive a message or ending blocking.
 *
 * @param task_tcb The task control block of the task.
 *
 * \page task_need_to_be_scheduled_number_increment task_need_to_be_scheduled_number_increment
 * \ingroup task_scheduled_number
 */
void task_need_to_be_scheduled_number_increment(task_tcb_t * task_tcb);

/*
 * Subtract one from the number of tasks to be scheduled. It will be 
 * called when the task is executed.
 *
 * @param task_tcb The task control block of the task.
 *
 * \page task_need_to_be_scheduled_number_reduction task_need_to_be_scheduled_number_reduction
 * \ingroup task_scheduled_number
 */
void task_need_to_be_scheduled_number_reduction(task_tcb_t * task_tcb);


/*
 * Get the current state of the task state mechine. Each task has a
 * state mechine. Tasks can change the state of the state machine
 * according to different messages received
 *
 * @param task_tcb The task control block of the task.
 *
 * @return The current state of the task state mechine.
 *
 * \page task_state_machine_state_get task_state_machine_state_get
 * \ingroup task_state_mechine
 */
base_type_t task_state_machine_state_get(task_tcb_t * task_tcb);

/*
 * Set the current state of the task state mechine.
 *
 * @param task_tcb The task control block of the task.
 *
 * @param new_state The new state of the task state mechine.
 *
 * @return EXE_PASS when the setting is successful or EXE_FAIL when
 * the setting fails.
 *
 * \page task_state_machine_state_set task_state_machine_state_set
 * \ingroup task_state_mechine
 */
base_type_t task_state_machine_state_set(task_tcb_t * task_tcb, base_type_t new_state);

/*
 * Get the next state of the task state mechine. 
 *
 * @param task_tcb The task control block of the task.
 *
 * @return The next state of the task state mechine.
 *
 * \page task_state_machine_next_state_get task_state_machine_next_state_get
 * \ingroup task_state_mechine
 */
base_type_t task_state_machine_next_state_get(task_tcb_t * task_tcb);

/*
 * Set the next state of the task state mechine.
 *
 * @param task_tcb The task control block of the task.
 *
 * @param new_state The new next state of the task state mechine.
 *
 * @return EXE_PASS when the setting is successful or EXE_FAIL when
 * the setting fails.
 *
 * \page task_state_machine_next_state_set task_state_machine_next_state_set
 * \ingroup task_state_mechine
 */
base_type_t task_state_machine_next_state_set(task_tcb_t * task_tcb, base_type_t new_state);

/*
 * Convert the current state of task state mechine to the next state.
 *
 * @param task_tcb The task control block of the task.
 *
 * @param new_state The new next state of the task state mechine.
 *
 * @return EXE_PASS when the conversion is successful or EXE_FAIL when
 * the conversion fails.
 *
 * \page task_state_machine_state_transition task_state_machine_state_transition
 * \ingroup task_state_mechine
 */
base_type_t task_state_machine_state_transition(task_tcb_t * task_tcb);

#endif
