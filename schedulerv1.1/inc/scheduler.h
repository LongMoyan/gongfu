#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "scheduler_config.h"

/* The state of scheduler. */
enum _scheduler_state
{
    UNINITIALIZED = 0,  /* The scheduler being queried is UNINITIALIZED. Schedulder has not been initialized. */
    INITIALIZED,        /* The scheduler being queried is INITIALIZED. Schedulder has already initialized. */
    STOPPED,            /* The scheduler being queried is STOPPED. Schedulder has been stopped. */
    DISPATCHING,        /* The scheduler being queried is DISPATCHING. Schedulder is working. */
};
typedef enum _scheduler_state scheduler_state_e;


/*
 * Must be called before starting scheduling. it will initialize the task
 * pool and registry.
 *
 * \page scheduler_init scheduler_init
 * \ingroup Scheduler
 */
void scheduler_init(void);

/*
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
 * The scheduler starts to work.
 *
 * \page scheduler_start scheduler_start
 * \ingroup Scheduler
 */
void scheduler_start(void);

/*
 * Not implemented yet!
 * 
 * \page scheduler_end scheduler_end
 * \ingroup Scheduler
 */
void scheduler_end(void);

#endif
