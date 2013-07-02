#include <stdio.h>
#include "xdvi.h"
#include "version.h"
#include "kpathsea/c-stat.h"
#include "kpathsea/hash.h"
#include "kpathsea/tex-file.h"

/* double linked list */
struct dl_list {
    void *item;
    struct dl_list *prev;
    struct dl_list *next;
};

void dl_list_apply(struct dl_list *list, void (*func)(const void *item));
size_t dl_list_len(struct dl_list *list);
struct dl_list *dl_list_head(struct dl_list *list);
struct dl_list *dl_list_push_front(struct dl_list *list, void *item);
struct dl_list *dl_list_insert(struct dl_list *list, void *item);
struct dl_list *dl_list_truncate(struct dl_list *list);
struct dl_list *dl_list_truncate_head(struct dl_list *list);
struct dl_list *dl_list_remove(struct dl_list *list,
			       const void *elem,
			       int *count,
			       void **item,
			       Boolean (*compare_func)(const void *item1, const void *item2));
Boolean dl_list_remove_item(struct dl_list **list);
