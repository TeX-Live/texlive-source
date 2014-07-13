/* hash.c: hash table operations.

   Copyright 1994-2000, 2002, 2005, 2008, 2012
   Karl Berry & Olaf Weber.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

#include <kpathsea/config.h>
#include <kpathsea/c-ctype.h>

#include <kpathsea/hash.h>
#include <kpathsea/str-list.h>


/* The hash function.  We go for simplicity here.  */

/* All our hash tables are related to filenames.  */

static unsigned
hash (hash_table_type table,  const_string key)
{
  unsigned n = 0;

  /* Our keys aren't often anagrams of each other, so no point in
     weighting the characters.  */
  while (*key != 0)
#if defined(WIN32)
    if (IS_KANJI(key)) {
      n = (n + n + (unsigned)(*key++)) % table.size;
      n = (n + n + (unsigned)(*key++)) % table.size;
    } else
#endif
    n = (n + n + TRANSFORM (*key++)) % table.size;

  return n;
}

/* Identical has function as above, but does not normalize keys. */
static unsigned
hash_normalized (hash_table_type table,  const_string key)
{
  unsigned n = 0;

  /* Our keys aren't often anagrams of each other, so no point in
     weighting the characters.  */
  while (*key != 0)
    n = (n + n + (*key++)) % table.size;

  return n;
}

hash_table_type
hash_create (unsigned size)
{
  /* The was "static ..." since Oct3, 1997 to work around a gcc
     optimizer bug for Alpha. That particular optimization bug
     should be gone by now (Mar4, 2009).
  */
  hash_table_type ret;
  unsigned b;
  ret.buckets = XTALLOC (size, hash_element_type *);
  ret.size = size;

  /* calloc's zeroes aren't necessarily NULL, so be safe.  */
  for (b = 0; b <ret.size; b++)
    ret.buckets[b] = NULL;

  return ret;
}

/* Whether or not KEY is already in TABLE, insert it and VALUE.  Do not
   duplicate the strings, in case they're being purposefully shared.  */

void
hash_insert (hash_table_type *table,
             const_string key,
             const_string value)
{
  unsigned n = hash (*table, key);
  hash_element_type *new_elt = XTALLOC1 (hash_element_type);

  new_elt->key = key;
  new_elt->value = value;
  new_elt->next = NULL;

  /* Insert the new element at the end of the list.  */
  if (!table->buckets[n])
    /* first element in bucket is a special case.  */
    table->buckets[n] = new_elt;
  else
    {
      hash_element_type *loc = table->buckets[n];
      while (loc->next)         /* Find the last element.  */
        loc = loc->next;
      loc->next = new_elt;      /* Insert the new one after.  */
    }
}

/* Same as above, for normalized keys. */
void
hash_insert_normalized (hash_table_type *table,
                        const_string key,
                        const_string value)
{
  unsigned n = hash_normalized (*table, key);
  hash_element_type *new_elt = XTALLOC1 (hash_element_type);

  new_elt->key = key;
  new_elt->value = value;
  new_elt->next = NULL;

  /* Insert the new element at the end of the list.  */
  if (!table->buckets[n])
    /* first element in bucket is a special case.  */
    table->buckets[n] = new_elt;
  else
    {
      hash_element_type *loc = table->buckets[n];
      while (loc->next)         /* Find the last element.  */
        loc = loc->next;
      loc->next = new_elt;      /* Insert the new one after.  */
    }
}

/* Remove a (KEY, VALUE) pair.  */

void
hash_remove (hash_table_type *table,  const_string key,
             const_string value)
{
  hash_element_type *p;
  hash_element_type *q;
  unsigned n = hash (*table, key);

  /* Find pair.  */
  for (q = NULL, p = table->buckets[n]; p != NULL; q = p, p = p->next)
    if (FILESTRCASEEQ (key, p->key) && STREQ (value, p->value))
      break;
  if (p) {
    /* We found something, remove it from the chain.  */
    if (q) q->next = p->next; else table->buckets[n] = p->next;
    /* We cannot dispose of the contents.  */
    free (p);
  }
}

/* Look up KEY in TABLE, and return NULL-terminated list of all matching
   values (not copies), in insertion order.  If none, return NULL.  */

const_string *
hash_lookup (hash_table_type table,  const_string key)
{
  hash_element_type *p;
  cstr_list_type ret;
  unsigned n = hash (table, key);
  ret = cstr_list_init ();

  /* Look at everything in this bucket.  */
  for (p = table.buckets[n]; p != NULL; p = p->next)
    if (FILESTRCASEEQ (key, p->key))
      cstr_list_add (&ret, p->value);

  /* If we found anything, mark end of list with null.  */
  if (STR_LIST (ret))
    cstr_list_add (&ret, NULL);

#ifdef KPSE_DEBUG
#if defined (KPSE_COMPAT_API)
  {
  kpathsea kpse = kpse_def;
  if (KPATHSEA_DEBUG_P (KPSE_DEBUG_HASH))
    {
      DEBUGF1 ("hash_lookup(%s) =>", key);
      if (!STR_LIST (ret))
        fputs (" (nil)\n", stderr);
      else
        {
          const_string *r;
          for (r = STR_LIST (ret); *r; r++)
            {
              putc (' ', stderr);
              if (kpse->debug_hash_lookup_int)
#if defined(_WIN64)
                fprintf (stderr, "%I64d", (__int64) *r);
#else
                fprintf (stderr, "%ld", (long) *r);
#endif
              else
                fputs (*r, stderr);
            }
          putc ('\n', stderr);
        }
      fflush (stderr);
    }
  }
#endif
#endif

  return STR_LIST (ret);
}

#ifdef KPSE_DEBUG
/* We only print nonempty buckets, to decrease output volume.  */

void
hash_print (hash_table_type table,  boolean summary_only)
{
  unsigned b;
  unsigned total_elements = 0, total_buckets = 0;

  for (b = 0; b < table.size; b++) {
    hash_element_type *bucket = table.buckets[b];

    if (bucket) {
      unsigned len = 1;
      hash_element_type *tb;

      total_buckets++;
      if (!summary_only) fprintf (stderr, "%4d ", b);

      for (tb = bucket->next; tb != NULL; tb = tb->next)
        len++;
      if (!summary_only) fprintf (stderr, ":%-5d", len);
      total_elements += len;

      if (!summary_only) {
        for (tb = bucket; tb != NULL; tb = tb->next)
          fprintf (stderr, " %s=>%s", tb->key, tb->value);
        putc ('\n', stderr);
      }
    }
  }

  fprintf (stderr,
          "%u buckets, %u nonempty (%u%%); %u entries, average chain %.1f.\n",
          table.size,
          total_buckets,
          100 * total_buckets / table.size,
          total_elements,
          total_buckets ? total_elements / (double) total_buckets : 0.0);
}
#endif

#if KPATHSEA_CAN_FREE
void
hash_free (hash_table_type table)
{
    struct hash_element_struct *p, *q;
    p = (struct hash_element_struct *)table.buckets;
    while (p != NULL) {
        q = p->next;
        free ((char *)p->key);
        free ((char *)p->value);
        free (p);
        p = q;
    }
}
#endif
