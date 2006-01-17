/* hash.h: declarations for a hash table.

   Copyright 1999, 2005 Olaf Weber.
   Copyright 1994, 95 Karl Berry.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef HASH_H
#define HASH_H

#include <kpathsea/c-proto.h>
#include <kpathsea/types.h>


/* A single (key,value) pair.  */
typedef struct hash_element_struct
{
  const_string key;
  const_string value;
  struct hash_element_struct *next;
} hash_element_type;

/* The usual arrangement of buckets initialized to null.  */
typedef struct
{
  hash_element_type **buckets;
  unsigned size;
} hash_table_type;

#ifdef KPSE_DEBUG
/* How to print the hash results when debugging.  */
extern KPSEDLL boolean kpse_debug_hash_lookup_int;
#endif

/* Create a hash table of size SIZE.  */
extern KPSEDLL hash_table_type hash_create P1H(unsigned size);

/* Insert the (KEY,VALUE) association into TABLE.  KEY may have more
   than one VALUE.  Neither KEY nor VALUE is copied.  */
extern KPSEDLL void hash_insert P3H(hash_table_type *table,
                                    const_string key,
                                    const_string value);

/* Insert the (KEY, VALUE) association into TABLE.  KEY may have more
   than one VALUE.  Neither KEY nor VALUE is copied.  Assume that KEY
   is already normalized (all lowercase) on platforms where this matters. */
extern KPSEDLL void hash_insert_normalized P3H(hash_table_type *table,
                                               const_string key,
                                               const_string value);

/* Remove the (KEY,VALUE) association from TABLE.  */
extern KPSEDLL void hash_remove P3H(hash_table_type *table,  const_string key,
                                    const_string value);

/* Look up KEY in MAP, and return NULL-terminated list of all matching
   values (not copies), in insertion order.  If none, return NULL.  */
extern KPSEDLL string *hash_lookup P2H(hash_table_type table, const_string key);

/* Print TABLE to stderr.  */
extern void hash_print P2H(hash_table_type table, boolean summary_only);

#endif /* not HASH_H */
