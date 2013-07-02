#include "xdvi-config.h"
#include "xdvi.h"
#include "util.h"
#include "string_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Helper functions for string lists
 */
char *
string_list_to_str(char **list, const char *sep)
{
    char *result = xstrdup("");
    int i;
    
    for (i = 0; list[i] != NULL; i++) {
	result = xstrcat(result, list[i]);
	result = xstrcat(result, sep);
    }
    return result;
}


void
string_list_print(char **list)
{
    int i;
    
    for (i = 0; list != NULL && list[i] != NULL; i++) {
	fprintf(stderr, "List %d: |%s|\n", i, list[i]);
    }
}

/*
 * Reorder the list so that str (which is supposed to occur in the list somewhere)
 * is the first element.
 */
char **
string_list_reorder(char **list, char *str)
{
    size_t i;
    char *tmp;
    Boolean found = False;
    
    for (i = 0; list[i] != NULL; i++) {
	if (strcmp(list[i], str) == 0) {
	    found = True;
	    break;
	}
    }

    if (!found) {
	XDVI_ERROR((stderr, "Item `%s' not found in list!\n", str));
	return list;
    }

    tmp = list[0];
    list[0] = str;
    list[i] = tmp;
    return list;
}

/*
 * Move the item at position idx to the start of the list,
 * by iteratively swapping it with its predecessor.
 */
char **
string_list_move_to_start(char **list, size_t idx)
{
    size_t i;

#if 0
    for (i = 0; list[i] != NULL; i++) {
	fprintf(stderr, "list before: %d: |%s|\n", i, list[i]);
    }
#endif
    
    for (i = idx; i > 0; i--) {
	char *tmp = list[i];
	list[i] = list[i - 1];
	list[i - 1] = tmp;
    }

#if 0
    for (i = 0; list[i] != NULL; i++) {
	fprintf(stderr, "list after: %d: |%s|\n", i, list[i]);
    }
#endif
    
    return list;
}

/*
 * Rotate the list so that the first element is shifted to the end,
 * and all next elements are moved down by 1; e.g.:
 * before rotate: a b c d e f
 * after rotate:  b c d e f a
 */
char **
string_list_rotate_down(char **list)
{
    size_t i;
    char *tmp;

    tmp = list[0];
    if (tmp == NULL) {
	return list;
    }
    for (i = 1; list[i] != NULL; i++) {
	list[i - 1] = list[i];
    }
    list[i - 1] = tmp;
    
    return list;
}

/*
 * Rotate the list so that the last element is shifted to the beginning,
 * and all next elements are moved up by 1; e.g.:
 * before rotate: a b c d e f
 * after rotate:  f a b c d e
 */
char **
string_list_rotate_up(char **list)
{
    size_t i;
    char *tmp = NULL;

    if (list[0] == NULL) {
	return list;
    }
    /* go to end */
    for (i = 0; list[i] != NULL; i++) { ; }
    i--; /* get last index */
    
    tmp = list[i];
    for (; i > 0; i--) {
	list[i] = list[i - 1];
    }
    list[0] = tmp;

    return list;
}

char **
string_list_prepend(char **list, const char *str)
{
    int i, k;
    /* reallocate with larger capacity */
    for (i = 0; list[i] != NULL; i++) { ; }
    i++;
    list = xrealloc(list, (i + 1) * sizeof *list);

    /* shift old contents down */
    for (k = i; k > 0; k--) {
	list[k] = list[k - 1];
    }
    /* add new element at beginning */
    list[0] = xstrdup(str);

    return list;
}
