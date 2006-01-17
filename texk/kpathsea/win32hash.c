/* hash.c: hash table operations.

Copyright (C) 1994, 95, 96, 97, 98, 99, 2000 Karl Berry & Olaf Weber.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <kpathsea/config.h>
#include <kpathsea/c-ctype.h>

#include <kpathsea/hash.h>
#include <kpathsea/str-list.h>

#ifdef HAVE_GETURL
#include <kpathsea/remote.h>
#endif

/* The hash function.  We go for simplicity here.  */

/* All our hash tables are related to filenames.  */
#ifdef MONOCASE_FILENAMES
#if defined(WIN32) && !defined(__i386_pc_gnu__)
/* This is way faster under Win32. */
#define TRANSFORM(x) ((unsigned)CharLower((LPTSTR)(BYTE)(x)))
#else
#define TRANSFORM(x) (tolower(x))
#endif
#else
#define TRANSFORM(x) (x)
#endif

#define MAX_HASH_TABLES 64

#define HASH_NULL(ptr) (ptr == hash_memory)
#define HASH_PTR(ptr) ((char *)hash_memory + (unsigned long)(ptr))
#define HASH_REL(rel) ((unsigned long)((char *)(rel) - (char *)hash_memory))
#define HASH_BUCKETS(table, n) (*((hash_element_type **)((char *)hash_memory + (unsigned long)table->buckets + (n) * sizeof(hash_element_type *))))
/* #define HASH_BUCKETS(table, n)  *((hash_element_type **)(HASH_PTR((unsigned long) ((hash_element_type **)HASH_PTR(table->buckets))[n]))) */
HANDLE hash_handle = 0;
void *hash_memory = NULL;
static unsigned long hash_memory_allocated = 0;
unsigned long hash_memory_size = 8L*1024L*1024L;

typedef struct _hash_memory_description {
  unsigned long sig;
  unsigned long memory_size;
  unsigned long first_free_address;
  unsigned long hash_offsets[MAX_HASH_TABLES];
  unsigned long db_dir_list_len;
  unsigned long db_dir_list;
} hash_memory_description;

hash_memory_description *hash_bloc;

/* We need to protect initialisation and insert / lookup */
static HANDLE hash_mutex;
static boolean hash_init_done = false;

extern str_list_type db_dir_list;

void hash_restore_str_list(unsigned long ptr, unsigned len, str_list_type *l);
void hash_save_str_list(str_list_type l);

#if 0
extern unsigned hash P2H(hash_table_type *table,  const_string key);
#ifdef NORMALIZED
extern unsigned hash_normalized P2H(hash_table_type *table,  const_string key);
#endif
#else
static unsigned
hash P2C(hash_table_type *, table,  const_string, key)
{
  unsigned n = 0;

  // fprintf(stderr, "Hashing %s to ", key);

  /* Our keys aren't often anagrams of each other, so no point in
     weighting the characters.  */
  while (*key != 0)
    n = (n + n + TRANSFORM (*key++)) % table->size;
  
  // fprintf(stderr, "%d\n", n);

  return n;
}

#ifdef NORMALIZED
/* Identical has function as above, but does not normalize keys. */
static unsigned
hash_normalized P2C(hash_table_type *, table,  const_string, key)
{
  unsigned n = 0;

  // fprintf(stderr, "Hashing (normalized) %s to ", key);

  /* Our keys aren't often anagrams of each other, so no point in
     weighting the characters.  */
  while (*key != 0)
    n = (n + n +  (*key++)) % table->size;

  // fprintf(stderr, "%d\n", n);
  
  return n;
}
#endif
#endif

/*
  This  function will be  registered by  atexit()  to  be called  upon
  program termination.     It   will free resources     allocated  for
  hash-tables.  It has to be registered only once, when the very first
  hash-table is allocated.
*/
void CDECL
hash_cleanup (void)
{
#ifdef HAVE_GETURL
  cleanup_remote_files();
#endif

  if (hash_memory != NULL) {
    if (UnmapViewOfFile(hash_memory) == 0) {
      /* Report an error */
      WARNING2("Can't UnMapViewOfFile(%x) (Error %d)!\n", hash_memory, GetLastError());
    }
    hash_memory = NULL;
  }

  if (hash_handle) {
    if (CloseHandle(hash_handle) == 0) {
      /* Report an error */
      WARNING2("Can't CloseHandle(%x) (Error %d)!\n", hash_handle, GetLastError());
    }
    hash_handle = NULL;
  }

  if (hash_mutex) {
    if (CloseHandle(hash_mutex) == 0) {
      /* Report an error */
      WARNING2("Can't CloseHandle(%x) (Error %d)!\n", hash_mutex, GetLastError());
    }
    hash_mutex = NULL;
  }
    
#ifdef KPSE_DEBUG
  if (KPSE_DEBUG_P (KPSE_DEBUG_SHM))
    DEBUGF1("Cleaned up shared hash tables, %ld memory was allocated.\n", hash_memory_allocated);
#endif
}

KPSEDLL boolean hash_init_needed = false;

boolean
hash_lock(boolean force)
{
  int dwWaitResult;

  if (!force && !hash_init_done)
    return true;

  /* Request ownership of mutex. */
  dwWaitResult = WaitForSingleObject(hash_mutex, 25000L);
 
  switch (dwWaitResult) {
    /* The thread got mutex ownership. */
  case WAIT_OBJECT_0: 
    return true;
    
    /* Cannot get mutex ownership due to time-out. */
  case WAIT_TIMEOUT: 
    WARNING("Timeout waiting to lock shared memory access!\n");
    return false; 

    /* Got ownership of the abandoned mutex object. */
  case WAIT_ABANDONED: 
    WARNING("Waiting to lock shared memory access abandoned!\n");
    return false; 
  }

  return false;
}

void
hash_unlock(void)
{
  if (! hash_init_done)
    return;

  if (ReleaseMutex(hash_mutex) == 0) {
    FATAL2("%s: Can't ReleaseMutex() access to shared memory databases (Error %d)!\n", kpse_program_name, GetLastError());
  }
}

static void
hash_start_init()
{
#ifdef KPSE_DEBUG
  if (KPSE_DEBUG_P (KPSE_DEBUG_SHM))
    DEBUGF1("%s: Start of shared hash tables initialisation.\n", kpse_program_name);
#endif
  hash_lock(true);
}

void
hash_end_init()
{
  hash_init_done = true;
  hash_unlock();

#ifdef KPSE_DEBUG
  if (KPSE_DEBUG_P (KPSE_DEBUG_SHM))
    DEBUGF1("%s: End of shared hash tables initialisation.\n", kpse_program_name);
#endif
}

void
hash_init (void)
{
  SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

  hash_init_done = false;
  if ((hash_mutex = CreateMutex(NULL, FALSE, "kpse_initialization")) == 0) {
    FATAL1("Can't CreateMutex() for shared memory locking (Error %d)!\n", GetLastError());
  }

  if ((hash_handle = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, HASH_MAPPING_NAME)) == 0) {

    hash_start_init();

    /* First call, need to allocate memory. */
    if ((hash_handle = CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE,
					 0, hash_memory_size, HASH_MAPPING_NAME)) == 0) {
      /* Report an error. */
      FATAL2("Can't CreateFileMapping() of size %ul (Error %d)!\n", hash_memory_size, GetLastError());
    };
    atexit(hash_cleanup);
    hash_init_needed = true;
  }

  if ((hash_memory = MapViewOfFile(hash_handle, FILE_MAP_ALL_ACCESS, 0, 0, 0)) == NULL) {
    /* Report an error. */
      FATAL2("Can't MapViewOfFile(%x) (Error %d)!\n", hash_handle, GetLastError());
  }

  hash_bloc = (hash_memory_description *)hash_memory;
  if (hash_init_needed) {
    /* Is it more efficient to zero the whole block first ? */
    ZeroMemory(hash_memory, sizeof(hash_bloc));
    hash_bloc->sig = sizeof(hash_memory_description);
    hash_bloc->memory_size = hash_memory_size;
    hash_bloc->first_free_address = sizeof(hash_memory_description);
  }
  else {
    extern boolean first_search;

    hash_start_init();

#ifdef KPSE_DEBUG
  if (KPSE_DEBUG_P (KPSE_DEBUG_SHM))
    DEBUGF1("Reusing shared hash tables at %x.\n", hash_memory);
#endif

    assert(hash_bloc->sig == sizeof(hash_memory_description));
    /*
      We could be a little bit cleaner here, just
      relying on the hashtable_instances enumeration.
    */
    cnf_hash = (hash_table_type *)hash_exists_p(hashtable_cnf);
    db = (hash_table_type *)hash_exists_p(hashtable_db);
    alias_db = (hash_table_type *)hash_exists_p(hashtable_alias);
    map = (hash_table_type *)hash_exists_p(hashtable_map);
    link_table = (hash_table_type *)hash_exists_p(hashtable_links);
    remote_db = (hash_table_type *)hash_exists_p(hashtable_remote);

    db_dir_list = str_list_init();
    /* Awful kludge ! */
    first_search = false;
    hash_restore_str_list(hash_bloc->db_dir_list, hash_bloc->db_dir_list_len, &db_dir_list);
    
  }
}

void
hash_reinit()
{
  if (!hash_init_needed) {
    fprintf(stderr, "%s: your `ls-R' database files have been modified\n", kpse_program_name);
    fprintf(stderr, "while kpathsea Dll was loaded by other programs.\n");
    fprintf(stderr, "It is recommended that you terminate all of them.\n");
    fprintf(stderr, "in order to reinitialize the shared databases.\n");
  }
}

boolean
hash_extend_memory ()
{
  /* We need to allocate a new memory bloc twice the size
     of the previous one, copy everything and free the previous block.
  */
  SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
  HANDLE new_hash_handle = 0;
  void *new_hash_memory = NULL;
  unsigned long new_hash_memory_size = 2 * hash_memory_size;

  assert(hash_memory != NULL);

  /* First, create some private unnamed bloc */  
  if ((new_hash_handle = CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE,
					   0, new_hash_memory_size, NULL)) == 0) {
    /* Report an error. */
    FATAL2("Can't CreateFileMapping() of size %ul (Error %d)!\n", new_hash_memory_size, GetLastError());
  };

  if ((new_hash_memory = MapViewOfFile(new_hash_handle, FILE_MAP_ALL_ACCESS, 0, 0, 0)) == NULL) {
    /* Report an error. */
      FATAL2("Can't MapViewOfFile(%x) (Error %d)!\n", new_hash_handle, GetLastError());
  }
  /* Copy the data */
  CopyMemory(new_hash_memory, hash_memory, hash_memory_size);
  /* Cleanup handles */
  hash_cleanup();
  /* make the values point to the new bloc */
  hash_memory_size = new_hash_memory_size;
  hash_memory = new_hash_memory;
  hash_handle = new_hash_handle;
  
  /* Now, create the actual named, shareable bloc */
  if ((new_hash_handle = CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE,
					   0, hash_memory_size, HASH_MAPPING_NAME)) == 0) {
    /* Report an error. */
    FATAL2("Can't CreateFileMapping() of size %ul (Error %d)!\n", new_hash_memory_size, GetLastError());
  };

  if ((new_hash_memory = MapViewOfFile(new_hash_handle, FILE_MAP_ALL_ACCESS, 0, 0, 0)) == NULL) {
    /* Report an error. */
      FATAL2("Can't MapViewOfFile(%x) (Error %d)!\n", new_hash_handle, GetLastError());
  }
  /* Copy the data */
  CopyMemory(new_hash_memory, hash_memory, hash_memory_size);
  /* Cleanup the handles */
  hash_cleanup();

  hash_memory = new_hash_memory;
  hash_handle = new_hash_handle;
  hash_bloc = (hash_memory_description *)hash_memory;

  return true;
}

unsigned long
hash_malloc P1C(unsigned, size)
{
  unsigned long ret = hash_bloc->first_free_address;
  hash_bloc->first_free_address += size;
  
  if ((unsigned long)hash_bloc->first_free_address > (unsigned long)hash_bloc->memory_size) {
    if (! hash_extend_memory()) {
      fprintf(stderr, "Memory exhausted: base = %x, first_free_address = %x, size = %x\n",
	      hash_memory, hash_bloc->first_free_address, hash_bloc->memory_size);
    }
  }
  assert((unsigned long)hash_bloc->first_free_address < (unsigned long)hash_bloc->memory_size);

  hash_memory_allocated += size;

  return ret;
}

unsigned long
hash_strdup P1C(const_string, p)
{
  unsigned long q = hash_malloc(strlen(p)+1);
  memcpy(HASH_PTR(q), p, strlen(p)+1);
  return q;
}

void
hash_free P1C(void *, address)
{
  /* Don't want to play with this for the moment.
     Hash-Tables just grow up, no need to remove 
     anything from them. The `hash_remove' function
     is only called from contrib/variables.c .
  */
}

void
hash_save_str_list(str_list_type l)
{
  unsigned int i;
  unsigned long *q;
  assert(hash_bloc != NULL);
  hash_bloc->db_dir_list_len = STR_LIST_LENGTH(l);

  if (hash_bloc->db_dir_list_len == 0) return;

  hash_bloc->db_dir_list = hash_malloc(hash_bloc->db_dir_list_len * sizeof(unsigned long));
  for (i = 0, q = (unsigned long *)HASH_PTR(hash_bloc->db_dir_list); 
       i < hash_bloc->db_dir_list_len; 
       i++, q++) {
    *q = hash_strdup(STR_LIST_ELT(l, i));
  }
}

void
hash_restore_str_list(unsigned long ptr, unsigned len, str_list_type *l)
{
  int i;
  unsigned long *q;
  for (i = 0, q = (unsigned long *)HASH_PTR(ptr); 
       i < len; 
       i++, q++) {
      str_list_add (l, HASH_PTR(*q));
  }
#if 0
  fprintf(stderr, "Restored db_dir_list :\n");
  for (i = 0; i < STR_LIST_LENGTH(*l); i++) {
    fprintf(stderr, "\t%s\n", STR_LIST_ELT(*l, i));
  }
#endif
}

hash_table_type *
hash_exists_p P1C(hashtable_instances, hash_number)
{
  unsigned long rel;

  assert(hash_memory != NULL);
  assert(hash_bloc != NULL);
  
#if 0
  fprintf(stderr, "Looking up for table %d ... ", hash_number);
#endif
  if (hash_bloc) {
    if (rel = hash_bloc->hash_offsets[hash_number]) {
#if 0
      fprintf(stderr, "found (%x) for %d\n", (char *)hash_memory + rel, hash_number);
#endif
      return (hash_table_type *)((char *)hash_memory + rel);
    }
  }
#if 0
  fprintf(stderr, "found (null) for %d\n", hash_number);
#endif
  return NULL;
}

hash_table_type *
hash_create P2C(unsigned, size, hashtable_instances, hash_number) 
{
  /* hash_table_type ret; changed into "static ..." to work around gcc
     optimizer bug for Alpha.  */
  static hash_table_type *ret;
  unsigned b;

  if (hash_memory == NULL) {
    hash_init();
  }

  if (ret = hash_exists_p(hash_number)) {
    assert(size == ret->size);
    return ret;
  }

  hash_lock(false);

#ifdef KPSE_DEBUG
  if (KPSE_DEBUG_P (KPSE_DEBUG_SHM))
    DEBUGF2("Hash: creating shared hash table %u of size %u\n", hash_number, size);
#endif

  hash_bloc->hash_offsets[hash_number] = hash_malloc(sizeof(hash_table_type));
  ret = (hash_table_type *)HASH_PTR(hash_bloc->hash_offsets[hash_number]);

  ret->buckets = (hash_element_type **)hash_malloc(size * sizeof(hash_element_type *));
  ret->size = size;
  
  /* calloc's zeroes aren't necessarily NULL, so be safe.  */
  for (b = 0; b <ret->size; b++)
    HASH_BUCKETS(ret, b) = NULL;

  hash_unlock();

  return ret;
}

#if 0
/* Not really usable. */
hash_table_type *
hash_reset P1C(hashtable_instances, hash_number) 
{
  /* hash_table_type ret; changed into "static ..." to work around gcc
     optimizer bug for Alpha.  */
  static hash_table_type *ret;
  unsigned b;

  if (hash_memory == NULL
      || (ret = hash_exists_p(hash_number)) == NULL) {
    return NULL;
  }

  hash_lock(false);

#ifdef KPSE_DEBUG
  if (KPSE_DEBUG_P (KPSE_DEBUG_SHM))
    DEBUGF1("Hash: resetting shared hash table %u\n", hash_number);
#endif

  /* calloc's zeroes aren't necessarily NULL, so be safe.  */
  for (b = 0; b <ret->size; b++)
    HASH_BUCKETS(ret, b) = NULL;

  hash_unlock();

  return ret;
}

#endif
/* WARNING: the comment below is frightening !
   However, it does not seem to be the case that the strings
   are changed after being inserted in the map.
   IHMO, this should be strictly forbidden.
*/
/* Whether or not KEY is already in MAP, insert it and VALUE.  Do not
   duplicate the strings, in case they're being purposefully shared.  */

void
hash_insert P3C(hash_table_type *, table,  const_string, key,
                const_string, value)
{

  unsigned n = hash (table, key);
  unsigned long new_elt;

  hash_lock(false);

  new_elt = hash_malloc(sizeof(hash_element_type));

  ((hash_element_type *)(HASH_PTR(new_elt)))->key = (const_string)hash_strdup(key);
  ((hash_element_type *)(HASH_PTR(new_elt)))->value = (const_string)hash_strdup(value);
  ((hash_element_type *)(HASH_PTR(new_elt)))->next = NULL;
  
  /* Insert the new element at the end of the list.  */
  if (! HASH_BUCKETS(table, n))
    /* first element in bucket is a special case.  */
    HASH_BUCKETS(table, n) = (hash_element_type *)new_elt;
  else
    {
      hash_element_type *loc = (hash_element_type *)HASH_PTR(HASH_BUCKETS(table, n));
      while (loc->next)		/* Find the last element.  */
        loc = (hash_element_type *)HASH_PTR(loc->next);
      loc->next = (hash_element_type *)new_elt;	/* Insert the new one after.  */
    }

  hash_unlock();
}

#ifdef NORMALIZED
/* Same as above, for normalized keys. */
void
hash_insert_normalized P3C(hash_table_type *, table,
                           const_string, key,
                           const_string, value)
{
  unsigned n = hash_normalized (table, key);
  unsigned long new_elt;

  hash_lock(false);

  new_elt = hash_malloc(sizeof(hash_element_type));

  ((hash_element_type *)(HASH_PTR(new_elt)))->key = (const_string)hash_strdup(key);
  ((hash_element_type *)(HASH_PTR(new_elt)))->value = (const_string)hash_strdup(value);
  ((hash_element_type *)(HASH_PTR(new_elt)))->next = NULL;
  
  /* Insert the new element at the end of the list.  */
  if (! HASH_BUCKETS(table, n))
    /* first element in bucket is a special case.  */
    HASH_BUCKETS(table, n) = (hash_element_type *)new_elt;
  else
    {
      hash_element_type *loc = (hash_element_type *)HASH_PTR(HASH_BUCKETS(table, n));
      while (loc->next)		/* Find the last element.  */
        loc = (hash_element_type *)HASH_PTR(loc->next);
      loc->next = (hash_element_type *)new_elt;	/* Insert the new one after.  */
    }

  hash_unlock();
}
#endif /* WIN32*/

/* Remove a (KEY, VALUE) pair.  */

void
hash_remove P3C(hash_table_type *, table,  const_string, key,
                const_string, value)
{
  hash_element_type *p;
  hash_element_type *q;
  unsigned n = hash (table, key);

  hash_lock(false);

  /* Find pair.  */
  for (q = NULL, p = (hash_element_type *)HASH_PTR(HASH_BUCKETS(table, n)); ! HASH_NULL(p); q = p, p = (hash_element_type *)HASH_PTR(p->next))
    if (FILESTRCASEEQ (key, HASH_PTR(p->key)) && STREQ (value, HASH_PTR(p->value)))
      break;
  if (p) {
    /* We found something, remove it from the chain.  */
    if (q) q->next = p->next; else HASH_BUCKETS(table, n) = p->next;
    /* We cannot dispose of the contents.  */
    hash_free ((void *)HASH_REL(p));
  }

  hash_unlock();
}

/* Look up STR in MAP.  Return a (dynamically-allocated) list of the
   corresponding strings or NULL if no match.  */ 

#ifdef KPSE_DEBUG
/* Print the hash values as integers if this is nonzero.  */
boolean kpse_debug_hash_lookup_int = false; 
#endif

string *
hash_lookup P2C(hash_table_type *, table,  const_string, key)
{
  hash_element_type *p;
  str_list_type ret;
  unsigned n = hash (table, key);

  ret = str_list_init ();

  hash_lock(false);

  /* Look at everything in this bucket.  */
  for (p = (hash_element_type *)HASH_PTR(HASH_BUCKETS(table, n)); ! HASH_NULL(p); p = (hash_element_type *)HASH_PTR(p->next))
    if (FILESTRCASEEQ (key, HASH_PTR(p->key)))
      /* Cast because the general str_list_type shouldn't force const data.  */
      str_list_add (&ret, (string) xstrdup((string)HASH_PTR(p->value)));
  
  /* If we found anything, mark end of list with null.  */
  if (STR_LIST (ret))
    str_list_add (&ret, NULL);

#ifdef KPSE_DEBUG
  if (KPSE_DEBUG_P (KPSE_DEBUG_HASH))
    {
      DEBUGF1 ("hash_shared_lookup(%s) =>", key);
      if (!STR_LIST (ret))
        fputs (" (nil)\n", stderr);
      else
        {
          string *r;
          for (r = STR_LIST (ret); *r; r++)
            {
              putc (' ', stderr);
              if (kpse_debug_hash_lookup_int)
                fprintf (stderr, "%ld", (long) *r);
              else
                fputs (*r, stderr);
            }
          putc ('\n', stderr);
        }
      fflush (stderr);
    }
#endif

  hash_unlock();

  return STR_LIST (ret);
}

/* We only print nonempty buckets, to decrease output volume.  */

void
hash_print P2C(hash_table_type *, table,  boolean, summary_only)
{
  unsigned b;
  unsigned total_elements = 0, total_buckets = 0;
  
  if (table == NULL) return;

  hash_lock(false);

  for (b = 0; b < table->size; b++) {
    hash_element_type *bucket = (hash_element_type *)HASH_PTR(HASH_BUCKETS(table, b));

    if (! HASH_NULL(bucket)) {
      unsigned len = 1;
      hash_element_type *tb;

      total_buckets++;
      if (!summary_only) fprintf (stderr, "%4d ", b);

      for (tb = (hash_element_type *)HASH_PTR(bucket->next); ! HASH_NULL(tb); tb = (hash_element_type *)HASH_PTR(tb->next))
        len++;
      if (!summary_only) fprintf (stderr, ":%-5d", len);
      total_elements += len;

      if (!summary_only) {
        for (tb = bucket; ! HASH_NULL(tb); tb = (hash_element_type *)HASH_PTR(tb->next))
          fprintf (stderr, " %s=>%s", HASH_PTR(tb->key), HASH_PTR(tb->value));
        putc ('\n', stderr);
      }
    }
  }

  hash_unlock();

  fprintf (stderr,
          "%u buckets, %u nonempty (%u%%); %u entries, average chain %.1f.\n",
          table->size,
          total_buckets,
          100 * total_buckets / table->size,
          total_elements,
          total_buckets ? total_elements / (double) total_buckets : 0.0);
}

/* Simple iterator over hash-table elements */
void
hash_iter P2C(hash_table_type *, table, hash_iter_fn, fn)
{
  unsigned b;

  if (! table) return;

  hash_lock(false);

  for (b = 0; b < table->size; b++) {
    hash_element_type *bucket = (hash_element_type *)HASH_PTR(HASH_BUCKETS(table, b));
    if (! HASH_NULL(bucket)) {
      unsigned len = 1;
      hash_element_type *tb;
      for (tb = bucket; ! HASH_NULL(tb); tb = (hash_element_type *)HASH_PTR(tb->next)) {
#ifdef KPSE_DEBUG
	if (KPSE_DEBUG_P (KPSE_DEBUG_HASH))
	  {
	    DEBUGF2("applying (shared) fn to %s%s\n", HASH_PTR(tb->value), HASH_PTR(tb->key));
	  }	
#endif
	(*fn)(HASH_PTR(tb->key), HASH_PTR(tb->value));
      }
    }
  }

  hash_unlock();
}
