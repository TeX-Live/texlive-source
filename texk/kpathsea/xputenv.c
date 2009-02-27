/* xputenv.c: set an environment variable without return. */

/* Copyright 1993-98, 2008 Karl Berry.
   Copyright 2003-05 Olaf Weber.

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

#ifdef WIN32
#include <stdlib.h>
#else
#if !HAVE_DECL_PUTENV
extern int putenv P1H(char* entry);
#endif
#endif /* not WIN32 */

/* These record the strings we've set and have to keep around.
 * This function can be called many times during a run, and this
 * allows us to reclaim memory we allocated.
 */
static char **saved_env;
static int    saved_count;

/*
 * We have different arguments from the "standard" function.  A separate
 * var and value tends to be much more practical.
 *
 * The standards for putenv are clear: put the passed string into the
 * environment, and if you alter that string, the environment changes.
 * Of course various implementations are broken in a number of ways,
 * which include making copies of the passed string, and more.
 */
void
xputenv(const char *var, const char *value)
{
    char  *cur_item;
    char  *old_item;
    char  *new_item;
    size_t var_lim;
    int    cur_loc;

    /* kpse_debug2(KPSE_DEBUG_VARS, "kpse_putenv($%s,%s)", var, value); */
    
    old_item = NULL;
    cur_item = concat3(var, "=", value);
    /* Include '=' in length. */
    var_lim = strlen(var) + 1;
    
    /* Have we stored something for this value already?  */
    for (cur_loc = 0; cur_loc != saved_count; ++cur_loc) {
        if (strncmp(saved_env[cur_loc], cur_item, var_lim) == 0) {
            /* Get the old value.  We need this is case another part
             * of the program didn't use us to change the environment.
             */
            old_item = getenv(var);
            break;
        }
    }

    if (old_item && strcmp(old_item, cur_item+var_lim) == 0) {
        /* Set same value as is in environment, don't bother to set. */
        free(cur_item);
        return;
    } else {
        /* We set a different value. */
        if (putenv(cur_item) < 0)
            FATAL1("putenv(%s)", cur_item);
        /* Get the new string. */
        new_item = getenv(var);
        if (new_item != cur_item+var_lim) {
            /* Our new string isn't used, don't keep it around. */
            free(cur_item);
            return;
        }
    }

    /* If we get here, it means getenv() returned a reference to cur_item.
     * So we save cur_item, and free the old string we also owned.
     */
    if (cur_loc == saved_count) {
        /* No old string. */
        saved_count++;
        saved_env = XRETALLOC(saved_env, saved_count, char *);
    } else {
        /* We owned the old string. */
        free(saved_env[cur_loc]);
    }
    saved_env[cur_loc] = cur_item;

    return;
}

/* A special case for setting a variable to a numeric value
   (specifically, KPATHSEA_DPI).  We don't need to dynamically allocate
   and free the string for the number, since it's saved as part of the
   environment value.  */

void
xputenv_int P2C(const_string, var_name,  int, num)
{
  char str[MAX_INT_LENGTH];
  sprintf (str, "%d", num);
  
  xputenv (var_name, str);
}
