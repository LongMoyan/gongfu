#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "scheduler_config.h"

#define LOGIC_FALSE                              ( ( base_type_t ) 0 )
#define LOGIC_TRUE                               ( ( base_type_t ) 1 )

#define EXE_PASS                                 ( LOGIC_FALSE )
#define EXE_FAIL                                 ( LOGIC_TRUE )

/* Task is aperiodic. */
#define APERIODIC_TASK 0

/* Task delay falg. */
#define TASK_DELAY_ONGOING 1
#define TASK_DELAY_ENDING 0

/* The maximum number of message received by the task. */
#define MAX_MESSAGE_NUMBER 16

/* Task message is empty. */
#define TASK_MESSAGE_IS_EMPTY 255

/* The number of task priorities. */
#define TASK_PRIORITY_NUMBER 3

/* The number of message priorities. */
#define MESSAGE_PRIORITY_NUMBER 3

/* enmu task priority. */
enum _task_priority
{
    TASK_PRIORITY_HIGH = 0,
    TASK_PRIORITY_MID,
    TASK_PRIORITY_LOW,
};
typedef enum _task_priority task_priority_e;

/* enmu message priority. */
enum _message_priority
{
    MESSAGE_PRIORITY_HIGH = 0,
    MESSAGE_PRIORITY_MID,
    MESSAGE_PRIORITY_LOW,
};
typedef enum _message_priority message_priority_e;


/*
 * Function name: scheduler_init().
 *
 * Must be called before starting scheduling. it will initialize the task
 * pool and registry.
 *
 * \page scheduler_init scheduler_init
 * \ingroup Scheduler
 */
void scheduler_init(void);

/*
 * Function name: scheduler_execute_task().
 *
 * Scheduler execution module will constantly try to obtain a ready task from
 * the ready list and execute the processing function of ready task.
 *
 * @param task_list_ready: Ready list containing at least ready tasks.
 *
 * \page scheduler_execute_task scheduler_execute_task
 * \ingroup Scheduler
 */
void scheduler_execute_task(void * task_list_ready);

/*
 * Function name: scheduler_start().
 *
 * The scheduler starts to work.
 *
 * \page scheduler_start scheduler_start
 * \ingroup Scheduler
 */
void scheduler_start(void);

/*
 * Function name: scheduler_end().
 *
 * Not implemented yet!
 * 
 * \page scheduler_end scheduler_end
 * \ingroup Scheduler
 */
void scheduler_end(void);

#endif
