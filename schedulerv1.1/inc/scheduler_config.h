#ifndef _SCHEDULER_CONFIG_H_
#define _SCHEDULER_CONFIG_H_

typedef long base_type_t;
typedef unsigned long ubase_type_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef void (*task_function_t)(void*);

typedef uint32_t tick_type_t;
#define MAX_BLOCK_TICK ( tick_type_t ) 0xffffffffUL


#endif
