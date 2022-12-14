#ifndef _SCHEDULER_CONFIG_H_
#define _SCHEDULER_CONFIG_H_


/* The return value of logical function. */
#define LOGIC_FALSE                              ( ( base_type_t ) 0 )
#define LOGIC_TRUE                               ( ( base_type_t ) 1 )

/* The return value of the running function. */
#define EXE_PASS                                 ( LOGIC_FALSE )
#define EXE_FAIL                                 ( LOGIC_TRUE )

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

/* The basic data type of scheduler program. */
typedef long base_type_t;
typedef unsigned long ubase_type_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

/* The type of task function pointer. */
typedef void (*task_function_t)(void*);

/* The data type of scheduler tick counter. */
typedef uint32_t tick_type_t;

/* The max value of blocked ticks. */
#define MAX_BLOCK_TICK ( tick_type_t ) 0xffffffffUL

#endif
