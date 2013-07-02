#include "xdvi-config.h"
#include "dl_list.h"

size_t dl_list_len(struct dl_list *list)
{
    size_t len = 0;
    struct dl_list *ptr;
    for (ptr = list; ptr != NULL; ptr = ptr->next, len++) { ; }
    return len;
}

/*
  Insert item to the list and return the result.
*/
struct dl_list *
dl_list_insert(struct dl_list *list, void *item)
{
    struct dl_list *new_elem = xmalloc(sizeof *new_elem);
    new_elem->item = item;
    new_elem->next = NULL;
    new_elem->prev = NULL;
    
    if (list == NULL) {
	list = new_elem;
    }
    else {
	/* append after current position */
	struct dl_list *ptr = list;
	
	new_elem->next = ptr->next;
	new_elem->prev = ptr;
	
	if (ptr->next != NULL)
	    ptr->next->prev = new_elem;
	
	ptr->next = new_elem;
    }
    return new_elem;
}

/*
  Return head of the list.
*/
struct dl_list *
dl_list_head(struct dl_list *list)
{
    for (; list != NULL && list->prev != NULL; list = list->prev) { ; }
    return list;
}

/*
  Put a new item at the front of the list (current front is passed in first argument)
  and return its position.
*/
struct dl_list *
dl_list_push_front(struct dl_list *list, void *item)
{
    struct dl_list *new_elem = xmalloc(sizeof *new_elem);
    new_elem->item = item;
    new_elem->next = NULL;
    new_elem->prev = NULL;
    
    if (list != NULL) { /* prepend to current position */
	new_elem->next = list;
	list->prev = new_elem;
    }
    return new_elem;
}

/*
  Truncate list so that current pointer is the last element.
*/
struct dl_list *
dl_list_truncate(struct dl_list *list)
{
    struct dl_list *ptr = list->next;
    struct dl_list *save;

    list->next = NULL;
    
    while (ptr != NULL) {
	save = ptr->next;
	free(ptr);
	ptr = save;
    }
    return list;
}

/*
  Truncate list at the head (i.e. remove the first element from it - head must be passed to this list),
  and return the result.
*/
struct dl_list *
dl_list_truncate_head(struct dl_list *list)
{
    struct dl_list *ptr = list->next;
    if (list->next != NULL)
	list->next->prev = NULL;
    free(list);
    return ptr;
}

/*
  If the item pointed to by *list isn't the head of the list, remove it,
  set *list to the previous item, and return True. Else return False.
*/
Boolean
dl_list_remove_item(struct dl_list **list)
{
    struct dl_list *ptr = *list; /* item to remove */
    if (ptr->prev == NULL)
	return False;
    ptr->prev->next = ptr->next;
    if (ptr->next != NULL)
	ptr->next->prev = ptr->prev;
    /* update list */
    *list = (*list)->prev;
    /* remove item */
    free(ptr);
    
    return True;
}

void
dl_list_apply(struct dl_list *list, void (*func)(const void *item))
{
    struct dl_list *ptr;
    for (ptr = list; ptr != NULL; ptr = ptr->next) {
	func(ptr->item);
    }
}

/*
  Remove all items matching compare_func() from list. Must be called
  with a pointer to the head of the list, which is also returned.
  Returns the number of removed items in `count'.
*/
struct dl_list *
dl_list_remove(struct dl_list *list, const void *item,
	       int *count,
	       void **removed_item,
	       Boolean (*compare_func)(const void *item1, const void *item2))
{
    struct dl_list *ptr = list;
    while (ptr != NULL) {
	struct dl_list *next = ptr->next;
	if (compare_func(ptr->item, item)) { /* match */
	    *removed_item = ptr->item;
	    (*count)++;
	    if (ptr->prev != NULL) {
		ptr->prev->next = ptr->next;
	    }
	    else { /* removed first element */
		list = list->next;
	    }

	    if (ptr->next != NULL)
		ptr->next->prev = ptr->prev;
	    free(ptr);
	    ptr = NULL;
	}
	ptr = next;
    }
    return list;
}
