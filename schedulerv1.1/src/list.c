#include <stdlib.h>
#include "list.h"

void list_init(list_t * list)
{
    if (NULL == list)
    {
        return ;
    }

    /* Double loop list header points to list footer. The item_value of 
     * list_end is always MAX_BLOCK_TICK. Initially, the list has no items.
     */
    list->list_head = &(list->list_end);
    list->list_end.pre_list_item = &(list->list_end);
    list->list_end.next_list_item = &(list->list_end);
    list->list_end.item_value = MAX_BLOCK_TICK;
    list->list_end.container = NULL;
    list->list_end.item_owner = NULL;
    list->list_item_number = 0;
}

// base_type_t list_destory(list_t * list)
// {
//     if (NULL == list)
//     {
//         return EXE_FAIL;
//     }

//     list_item_t * cur_item = list->head;
//     while (cur_item)
//     {
//         list_item_t * next_item = cur_item->next;
//         free(cur_item);
//         cur_item = next_item;
//     }

//     list->list_head = &(list->list_end);
//     list->list_end.pre_list_item = &(list->list_end);
//     list->list_end.next_list_item = &(list->list_end);
//     list->list_end.item_value = MAX_BLOCK_TICK;
//     list->list_end.container = NULL;
//     list->list_end.item_owner = NULL;
//     list->list_item_number = 0;

//     return EXE_PASS;
// }

void list_insert(list_t * list, list_item_t * new_list_item)
{
    list_item_t * item_insert_position;
    tick_type_t new_list_item_value = new_list_item->item_value;

    if(NULL == list || NULL == new_list_item)
    {
        return ;
    }
    
    /* The item value is MAX_BLOCK_TICK, so insert it at the end of the list. */
    if( new_list_item_value == MAX_BLOCK_TICK )
    {
        item_insert_position = list->list_end.pre_list_item;
    }
    /* Find the insert position. The value of position item is the maximum value
     * less than the value of the insert item.
     */
    else
    {
        for( item_insert_position = ( list_item_t * ) &( list->list_end );
         item_insert_position->next_list_item->item_value <= new_list_item_value;
          item_insert_position = item_insert_position->next_list_item )
        {
        }
    }

    /* Insert a new item to double loop list. */
    new_list_item->next_list_item = item_insert_position->next_list_item;
    new_list_item->next_list_item->pre_list_item = new_list_item;
    new_list_item->pre_list_item = item_insert_position;
    item_insert_position->next_list_item = new_list_item;

    /* Remember which list the item is in. */
    new_list_item->container = list;

    ( list->list_item_number )++;
}

void list_insert_end(list_t* list, list_item_t * new_list_item)
{
    list_item_t * list_head = list->list_head;
    if(NULL == list || NULL == new_list_item)
    {
        return ;
    }

    /* Insert a new item at the end of the double loop list. */
    new_list_item->next_list_item = list_head;
    new_list_item->pre_list_item = list_head->pre_list_item;
    list_head->pre_list_item->next_list_item = new_list_item;
    list_head->pre_list_item = new_list_item;

    /* Remember which list the item is in. */
    new_list_item->container = list;

    ( list->list_item_number )++;
}

void list_remove(list_item_t * remove_list_item)
{
    list_t * list = remove_list_item->container;
    if (NULL == remove_list_item || NULL == list)
    {
        return ;
    }

    /* Remove a item from double loop list. */
    remove_list_item->next_list_item->pre_list_item = remove_list_item->pre_list_item;
    remove_list_item->pre_list_item->next_list_item = remove_list_item->next_list_item;

    //???
    // /* Make sure the index is left pointing to a valid item. */
    // if( list->list_head == remove_list_item )
    // {
    //     list->list_head = remove_list_item->pre_list_item;
    // }

    remove_list_item->container = NULL;
    ( list->list_item_number )--;
}

base_type_t list_is_empty(list_t * list)
{
    return (list->list_item_number) ? LOGIC_FALSE : LOGIC_TRUE;
}

ubase_type_t list_item_number_get(list_t* list)
{
    return list->list_item_number;
}

void * list_get_owner_of_first_item(list_t* list)
{
    if (LOGIC_TRUE == list_is_empty(list))
    {
        return NULL;
    }

    return ((&((list)->list_end))->next_list_item->item_owner);
}

list_item_t * list_get_first_item(list_t* list)
{
    if (LOGIC_TRUE == list_is_empty(list))
    {
        return NULL;
    }

    return ((&((list)->list_end))->next_list_item);
}
