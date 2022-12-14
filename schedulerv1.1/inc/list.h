#ifndef _SCHEDULER_LIST_H_
#define _SCHEDULER_LIST_H_

#include "scheduler.h"

struct _list;

/* list item struct */
struct _list_item
{
    void * item_owner;                          /* The owner(most likely a tcb) of the list item. */
    struct _list * container;                   /* The list pointer of list items is stored. Such as, it is task_read_list pointer when the item is stored in ready list. */
    ubase_type_t item_value;                    /* The weight(usually the wake-up time) of list item. The ordered list will be stored by its value. */
    struct _list_item * pre_list_item;          /* The pointer to the previous item. */
    struct _list_item * next_list_item;         /* The pointer to the next item. */
};
typedef struct _list_item list_item_t;

/* list struct */
struct _list
{
    list_item_t* list_head;                     /* The head of the list. */
    list_item_t list_end;                       /* The end of a bidirectional loop list. The list_head pointer always points to it. */
    ubase_type_t list_item_number;              /* The number of list items. */
};
typedef struct _list list_t;


/*
 * Must be called before a list is used! This initialises all the members
 * of the list structure and inserts the list_end item into the list as a
 * marker to the back of the list.
 *
 * @param list: Pointer to the list being initialised.
 *
 * \page list_init list_init
 * \ingroup Double_circular_linked_list
 */
void list_init(list_t * list);

/*
 * Insert an item into a list. The item will be inserted into the list in
 * a position determined by its item value (ascending item value order).
 *
 * @param list: The list into which the item is to be inserted.
 *
 * @param new_list_item: A new item inserted into the list.
 *
 * \page list_insert list_insert
 * \ingroup Double_circular_linked_list
 */
void list_insert(list_t * list, list_item_t * new_list_item);


/*
 * Insert an item into a list. The item will be inserted at the end of 
 * the list.
 *
 * @param list: The list into which the item is to be inserted.
 *
 * @param new_list_item: A new item inserted into the list.
 *
 * \page list_insert_end list_insert_end
 * \ingroup Double_circular_linked_list
 */
void list_insert_end(list_t* list, list_item_t * new_list_item);

/*
 * Remove an item from a list. The list item has a pointer to the list that
 * it is in, so only the list item need be passed into the function.
 *
 * @param remove_list_item: An item to be removed from the list.
 *
 * \page list_remove list_remove
 * \ingroup Double_circular_linked_list
 */
void list_remove(list_item_t* remove_list_item);

/*
 * Determine whether the list is empty.
 *
 * @param list: The list that will be Determined whether it is empty.
 *
 * @return LOGIC_TRUE when the list is empty ,otherwise LOGIC_FALSE.
 *
 * \page list_is_empty list_is_empty
 * \ingroup Double_circular_linked_list
 */
base_type_t list_is_empty(list_t * list);

/*
 * Get the numbers of the list items.
 *
 * @param list: The list for the number of items to be obtained.
 *
 * @return Number of items in the list.
 *
 * \page list_item_number_get list_item_number_get
 * \ingroup Double_circular_linked_list
 */
ubase_type_t list_item_number_get(list_t * list);

/*
 * Get the owner(usually a task control block) of the first list item. Each
 * item has an owner, which can quickly find the task control block of the
 * item.
 *
 * @param list: The list for the number of items to be obtained.
 *
 * @return Pointer to owner.
 *
 * \page list_get_owner_of_first_item list_get_owner_of_first_item
 * \ingroup Double_circular_linked_list
 */
void * list_get_owner_of_first_item(list_t * list);

/*
 * Get the first item of the list.
 *
 * @param list: The list that need to get the first item.
 *
 * @return Pointer to the first item in the list.
 *
 * \page list_get_first_item list_get_first_item
 * \ingroup Double_circular_linked_list
 */
list_item_t * list_get_first_item(list_t* list);

#endif
