/*
 * Copyright (c) 2004 Stefan Ulrich
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/*
  Exit handlers: Procedures taking a void * that can be registered
  via register_exit_handler() and will be called fromx xdvi_exit().
*/

#include "xdvi-config.h"
#include "xdvi.h"
#include "exit-handlers.h"
#include "util.h"

static struct exit_list {
    exit_procedure proc;
    void *arg;
} *exit_procs = NULL;

static size_t exit_procs_size = 0;

void register_exit_handler(exit_procedure proc, void *arg)
{
    size_t idx = exit_procs_size;
    exit_procs_size++;
    exit_procs = xrealloc(exit_procs, sizeof *exit_procs * exit_procs_size);
    exit_procs[idx].proc = proc;
    exit_procs[idx].arg = arg;
}

void unregister_exit_handler(exit_procedure proc)
{
    size_t i;
    for (i = 0; i < exit_procs_size; i++) {
	if (exit_procs[i].proc == proc) {
	    fprintf(stderr, "unregistering %p!\n", (void *)proc);
	    exit_procs[i].proc = NULL;
	    exit_procs[i].arg = NULL;
	}
    }
}

void call_exit_handlers(void)
{
    size_t i;
    for (i = 0; i < exit_procs_size; i++) {
	if (exit_procs[i].proc)
	    exit_procs[i].proc(exit_procs[i].arg);
	/* 	fprintf(stderr, "calling exit proc %lu\n", (unsigned long)i); */
    }
    free(exit_procs);
}
