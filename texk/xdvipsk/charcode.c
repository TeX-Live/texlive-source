/*
 *   charcode.c
 *   This routine handles the operations on chardesctype hash table
 *   and replaces original font chardesc array operations.
 */
#include "xdvips.h" /* The copyright notice in that file is included too!*/

#include "uthash.h"
/*
 *   The external declarations:
 */
#include "protos.h"


chardesctype *add_chardesc(fontdesctype *f, int charcode)
{
    chardesctype *s;

    HASH_FIND_INT(f->chardesc_hh, &charcode, s);  /* id already in the hash? */
    if (s==NULL) {
        s = (chardesctype*)malloc(sizeof(chardesctype));
        s->charcode = charcode;
        s->TFMwidth = 0;
        s->packptr = NULL;
        s->pixelwidth = 0;
        s->flags = 0;
        s->flags2 = 0;
        s->cid = 0;
        HASH_ADD_INT( f->chardesc_hh, charcode, s );  /* id: name of key field */
    }
    return s;
}

chardesctype *find_chardesc(fontdesctype *f, int charcode)
{
    chardesctype *s;

    HASH_FIND_INT( f->chardesc_hh, &charcode, s );  /* s: output pointer */
    return s;
}

void delete_chardesc(fontdesctype *f, int charcode)
{
    chardesctype *s;

    HASH_FIND_INT(f->chardesc_hh, &charcode, s);  /* id already in the hash? */
    if (s!=NULL) {
        HASH_DEL( f->chardesc_hh, s);  /* user: pointer to deletee */
        free(s);
    }
}

void delete_all_chardesc(fontdesctype *f)
{
    chardesctype *current, *tmp;

    HASH_ITER(hh, f->chardesc_hh, current, tmp) {
        HASH_DEL(f->chardesc_hh,current);  /* delete it (users advances to next) */
        free(current);            /* free it */
    }
}

static int charcode_sort(chardesctype *a, chardesctype *b)
{
    return (a->charcode - b->charcode);
}

void sort_by_charcode(fontdesctype *f)
{
    HASH_SORT(f->chardesc_hh, charcode_sort);
}
