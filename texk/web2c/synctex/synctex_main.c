/* 
Copyright (c) 2008 jerome DOT laurens AT u-bourgogne DOT fr

This file is part of the SyncTeX package.

License:
--------
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE

Except as contained in this notice, the name of the copyright holder  
shall not be used in advertising or otherwise to promote the sale,  
use or other dealings in this Software without prior written  
authorization from the copyright holder.

Important notice:
-----------------
This file is named "synctex_main.c".
This is the command line interface to the synctex_parser.c.
*/

#   include <stdlib.h>
#   include <stdio.h>
#   include <string.h>
#   include <stdarg.h>
#   include "synctex_parser.h"

int main(int argc, char *argv[]);

void synctex_help(char * error,...);
void synctex_help_view(char * error,...);
void synctex_help_edit(char * error,...);

int synctex_view(int argc, char *argv[]);
int synctex_edit(int argc, char *argv[]);
int synctex_update(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	int arg_index = 1;
	if(arg_index<argc) {
		if(0==strcmp("help",argv[arg_index])) {
			if(++arg_index<argc) {
				if(0==strcmp("view",argv[arg_index])) {
					synctex_help_view(NULL);
					return 0;
				} else if(0==strcmp("edit",argv[arg_index])) {
					synctex_help_edit(NULL);
					return 0;
				}
			}
			synctex_help(NULL);
			return 0;
		} else if(0==strcmp("view",argv[arg_index])) {
			return synctex_view(argc-arg_index-1,argv+arg_index+1);
		} else if(0==strcmp("edit",argv[arg_index])) {
			return synctex_edit(argc-arg_index-1,argv+arg_index+1);
		} else if(0==strcmp("update",argv[arg_index])) {
			return synctex_update(argc-arg_index-1,argv+arg_index+1);
		}
	}
	synctex_help("Missing options");
	return 0;
}

void synctex_usage(char * error,va_list ap) {
	if(error) {
		fprintf(stderr,"SyncTeX ERROR: ");
		vfprintf(stderr,error,ap);
		fprintf(stderr,"%s\n");
	}
	fprintf((error?stderr:stdout),
		"usage: synctex <subcommand> [options] [args]\n"
		"Synchronize TeXnology command-line client, version 0.1\n\n"
		"The Synchronization TeXnology by Jérôme Laurens is a new feature of recent TeX engines.\n"
		"It allows to synchronize between input and output, which means to\n"
		"navigate from the source document to the typeset material and vice versa.\n\n"
	);
	return;
}

void synctex_help(char * error,...) {
	va_list v;
	va_start(v, error);
	synctex_usage(error, v);
	va_end(v);
	fprintf((error?stderr:stdout),
		"Available subcommands:\n"
		"   view     to perform forwards synchronization\n"
		"   edit     to perform backwards synchronization\n"
		"   update   to update a synctex file after a dvi/xdv to pdf filter\n"
		"   help     this help\n\n"
		"Type 'synctex help <subcommand>' for help on a specific subcommand.\n"
		"Type 'synctex --version' to see the program version number.\n\n"
	);
	return;
}

void synctex_help_view(char * error,...) {
	va_list v;
	va_start(v, error);
	synctex_usage(error, v);
	va_end(v);
	fprintf((error?stderr:stdout),
		"synctex view: forwards synchronization,\n"
		"command sent by the editor to view the output corresponding to the position under the mouse\n"
		"\n"
		"usage: synctex view -i line:column:input -o output [-x viewer-command] [-h before/offset:middle/after]\n"
		"\n"
		"-i line:column:input\n"
		"       specify the line, column and input file.\n"
		"       The line and column are 1 based integers,\n"
		"       they allow to identify every character in a file.\n"
		"       column is the offset of a character relative to the containing line.\n"
		"       Pass 0 if this information is not relevant.\n"
		"       input is either the name of the main source file or an included document.\n"
		"       It must be the very name as understood by TeX.\n"
		"       \n"
		"-o output\n"
		"       is the full path of the output file (with any relevant path extension).\n"
		"       \n"
		"-x viewer-command\n"
		"       Normally the synctex tool outputs its result to the stdout.\n"
		"       It is possible to launch an external tool with the result.\n"
		"       The viewer-command is a printf like format string with numbered specifiers.\n"
		"       %{output} is the name of the main document, without path extension.\n"
		"       %{page} is the 0 based page number, %{page+1} is the 1 based page number.\n"
		"       To synchronize by point, %{x} is the x coordinate, %{y} is the y coordinate,\n"
		"       both in dots and relative to the top left corner of the page.\n"
		"       To synchronize by box,\n"
		"       %{h} is the horizontal coordinate of the origin of the enclosing box,\n"
		"       %{v} is the vertical coordinate of the origin of the enclosing box,\n"
		"       both in dots and relative to the upper left corner of the page.\n"
		"       They may be different from the preceding pair of coordinates.\n"
		"       %{width} is the width, %{height} is the height of the enclosing box.\n"
		"       The latter dimension is naturally counted from bottom to top.\n"
		"       There is no notion of depth for such a box.\n"
		"       To synchronize by content, %{before} is the word before,\n"
		"       %{offset} is the offset, %{middle} is the middle word, and %{after} is the word after.\n"
		"\n"
		"       If no viewer command is provided, the content of the SYNCTEX_VIEWER environment variable is used instead.\n"
		"\n"
		"-h before/offset:middle/after\n"
		"       This hint allows a forwards synchronization by contents.\n"
		"       Instead of giving a character offset in a line, you can give full words.\n"
		"       A full word is a sequence of characters (excepting '/').\n"
		"       You will choose full words in the source document that will certainly appear unaltered in the output.\n"
		"       The middle word contains the character under the mouse at position offset.\n"
		"       before is a full word preceding middle and after is following it.\n"
		"       The before or after word can be missing, they are then considered as void strings.\n"
		"       \n"
	);
	return;
}

/* "usage: synctex view -i line:column:input -o output [-x viewer-command] [-h before/offset:middle/after]\n" */
int synctex_view(int argc, char *argv[]) {
	int arg_index = 0;
	int line = -1;
	int column = 0;
	char * input = NULL;
	char * viewer = NULL;
	char * before = NULL;
	int offset = 0;
	char * middle = NULL;
	char * after = NULL;
	char * output = NULL;
	/* required */
	if((arg_index>=argc) || strcmp("-i",argv[arg_index]) || (++arg_index>=argc)) {
		synctex_help_view("Missing -i required argument");
		return -1;
	}
	char * start = argv[arg_index];
	char * end = NULL;
	line = strtol(start,&end,10);
	if(end>start && strlen(end)>0 && *end==':') {
		start = end+1;
		column = strtol(start,&end,10);
		if(end == start || column < 0) {
			column = 0;
		}
		if(strlen(end)>1 && *end==':') {
			input = end+1;
			goto scan_output;
		}
	}
	synctex_help_view("Bad -i argument");
	return -1;
scan_output:
	if((++arg_index>=argc) || strcmp("-o",argv[arg_index]) || (++arg_index>=argc)) {
		synctex_help_view("Missing -o required argument");
		return -1;
	}
	output = argv[arg_index];
	/* now scan the optional arguments */
	if(++arg_index<argc) {
		if(0 == strcmp("-x",argv[arg_index])) {
			if(++arg_index<argc) {
				if(strcmp("-",argv[arg_index])) {
					/* next option does not start with '-', this is a command */
					viewer = argv[arg_index];
					if(++arg_index<argc) {
						goto option_hint;
					} else {
						goto proceed;
					}
				} else {
					/* retrieve the environment variable */
					viewer = getenv("SYNCTEX_VIEWER");
					goto option_hint;
				}
			} else {
				viewer = getenv("SYNCTEX_VIEWER");
				goto proceed;
			}
		}
option_hint:
		if(0 == strcmp("-h",argv[arg_index]) && ++arg_index<argc) {
			/* modify the argument */;
			after = strstr(argv[arg_index],"/");
			if(NULL != after) {
				before = argv[arg_index];
				*after = '\0';
				++after;
				offset = strtol(after,&middle,10);
				if(middle>after && offset>=0 && strlen(middle)>2) {
					after = strstr(++middle,"/");
					if(NULL != after) {
						*after = '\0';
						if(offset<strlen(middle)) {
							++after;
							goto proceed;
						}
					}
				}
			}
			synctex_help_view("Bad hint");
			return -1;
		}
	}
	/* Now proceed, ignore any other argument */
	synctex_scanner_t scanner = NULL;
	char * synctex = NULL;
	size_t size = 0;
proceed:
#if SYNCTEX_DEBUG
	printf("line:%i\n",line);
	printf("column:%i\n",column);
	printf("input:%s\n",input);
	printf("viewer:%s\n",viewer);
	printf("before:%s\n",before);
	printf("offset:%i\n",offset);
	printf("middle:%s\n",middle);
	printf("after:%s\n",after);
	printf("output:%s\n",output);
	printf("cwd:%s\n",getcwd(NULL,0));
#endif
	size = strlen(output)+9;
	synctex = (char *)malloc(size);
	if(NULL == synctex) {
		synctex_help_view("No more memory");
		return -1;
	}
	if(0 == strlcpy(synctex,output,size)) {
		synctex_help_view("Copy problem");
		return -1;
	}
	char * ptr = synctex;
	char * where = NULL;
	/* remove the path extension of the last path component. */
#   define SYNCTEX_PATH_COMPONENTS_SEPARATOR "/"
	if(where = strstr(ptr,SYNCTEX_PATH_COMPONENTS_SEPARATOR)) {
		do {
			ptr = where;
		} while(where = strstr(ptr+1,SYNCTEX_PATH_COMPONENTS_SEPARATOR));
	}
#   define SYNCTEX_PATH_EXTENSION_SEPARATOR "."
	if(where = strstr(ptr,SYNCTEX_PATH_EXTENSION_SEPARATOR)) {
		do {
			ptr = where;
		} while(where = strstr(ptr+1,SYNCTEX_PATH_EXTENSION_SEPARATOR));
		*ptr = '\0';
	}
	if(0 == strlcat(synctex,".synctex",size)){
		synctex_help_view("Concatenation problem");
		return -1;
	}
	size = 0;
	scanner = synctex_scanner_new_with_contents_of_file(synctex);
	free(synctex);
	synctex = NULL;
	if(scanner && synctex_display_query(scanner,input,line,column)) {
		synctex_node_t node = NULL;
		if(node = synctex_next_result(scanner)) {
			/* filtering the command */
			if(viewer && strlen(viewer)) {
				/* Replace %{ by &{, then remove all unescaped '%'*/
				char * where = NULL;
				while(where = strstr(viewer,"%{")) {
					*where = '&';
				}
				where = viewer;
				while(where = strstr(viewer,"%")) {
					if(strlen(++where)) {
						if(*where == '%') {
							++where;
						} else {
							*(where-1)='&';
						}
					}
				}
				char * dictionary [] = {
					"&{output}","%1$s",
					"&{page}","%2$i",
					"&{page+1}","%3$i",
					"&{x}","%4$f",
					"&{y}","%5$f",
					"&{h}","%6$f",
					"&{v}","%7$f",
					"&{width}","%8$f",
					"&{height}","%9$f",
					"&{before}","%10$s",
					"&{offset}","%11$i",
					"&{middle}","%12$s",
					"&{after}","%13$s"};
				int i = 26;
				while(i>0) {
					char * value = dictionary[--i];
					char * key = dictionary[--i];
find_a_key:
					where = strstr(viewer,key);
					if(where) {
						memcpy(where,value,strlen(value));
						memmove(where+strlen(value),where+strlen(key),strlen(where)-strlen(key)+1);
						goto find_a_key;
					}
				}
				size = strlen(viewer)+3*sizeof(int)+6*sizeof(float)+4*(65536)+1;
				char * buffer = malloc(size);
				if(NULL == buffer) {
					synctex_help_view("No memory available");
					return -1;
				}
				if(size-1<=snprintf(buffer,size,viewer,
						"<main output>",
						synctex_node_page(node)-1,
						synctex_node_page(node),
						synctex_node_h(node),
						synctex_node_v(node),
						synctex_node_box_visible_h(node),
						synctex_node_box_visible_v(node)+synctex_node_box_visible_depth(node),
						synctex_node_box_visible_width(node),
						synctex_node_box_visible_height(node)+synctex_node_box_visible_depth(node),
						(before?before:""),
						offset,
						(middle?middle:""),
						(after?after:""))) {
					/* Not all the characters were printed*/
					free(buffer);
					buffer= NULL;
					synctex_help_view("Problem with\n%s\n",viewer);
					return -1;
				}
				printf("SyncTeX: Executing\n%s\n",buffer);
				int status = system(buffer);
				free(buffer);
				buffer = NULL;
				return status;
			} else {
				/* just print out the results */
				puts("SyncTeX result begin\n");
				do {
					printf(	"Output:%s\n"
							"Page:%i\n"
							"x:%f\n"
							"y:%f\n"
							"h:%f\n"
							"v:%f\n"
							"W:%f\n"
							"H:%f\n"
							"before:%s\n"
							"offset:%i\n"
							"middle:%s\n"
							"after:%s\n",
							output,
							synctex_node_sheet(node),
							synctex_node_h(node),
							synctex_node_v(node),
							synctex_node_box_h(node),
							synctex_node_box_v(node)+synctex_node_box_depth(node),
							synctex_node_box_width(node),
							synctex_node_box_height(node)+synctex_node_box_depth(node),
							(before?before:""),
							offset,
							(middle?middle:""),
							(after?after:""));
				} while(node = synctex_next_result(scanner));
				puts("SyncTeX result end\n");
			}
		}
	}
	return 0;
}

void synctex_help_edit(char * error,...) {
	va_list v;
	va_start(v, error);
	synctex_usage(error, v);
	va_end(v);
	fputs(
	"synctex edit: backwards synchronization,\n"
	"command sent by the viewer to edit the source corresponding to the position under the mouse\n\n"
	"\n"
	"usage: synctex edit -o page:x:y:file [-x editor-command] [-h offset:context]\n"
	"\n"
	"-o page:x:y:file\n"
	"       specify the page and coordinates of the point under the mouse.\n"
	"       page is 1 based.\n"
	"       Coordinates x and y are counted from the top left corner of the page.\n"
	"       Their unit is the big point (72 dpi).\n"
	"       \n"
	"       file is the full path of a tex file (including the synctex path extension)\n"
	"       \n"
	"-x editor-command\n"
	"       Normally the synctex tool outputs its result to the stdout.\n"
	"       It is possible to execute an external tool with the result.\n"
	"       The editor-command is a printf like format string with following specifiers.\n"
	"       They will be replaced by their value before the command is executed.\n"
	"       %{output} is the full path of the output document, with no extension.\n"
	"       %{input} is the name of the input document.\n"
	"       %{line} is the 0 based line number. %{line+1} is the 1 based line number.\n"
	"       %{column} is the 0 based column number or -1. %{column+1} is the 1 based column number or -1.\n"
	"       %{offset} is the 0 based offset and %{context} is the context of the hint.\n"
	"       \n"
	"       If no editor command is provided, the content of the SYNCTEX_EDITOR environment variable is used instead.\n"
	"       \n"
	"-h offset:context\n"
	"       This hint allows a backwards synchronization by contents.\n"
	"       You give a context including the character at the mouse location, and\n"
	"       the offset of this character relative to the beginning of this bunch of text.\n"
	"       \n",
	(error?stderr:stdout)
	);
	return;
}

/*	"usage: synctex edit -o page:x:y:output [-x editor-command] [-h offset:context]\n"  */
int synctex_edit(int argc, char *argv[]) {
	int arg_index = 0;
	int page = -1;
	float x = 0;
	float y = 0;
	char * output = NULL;
	char * editor = NULL;
	int offset = 0;
	char * context = NULL;
	/* required */
	if((arg_index>=argc) || strcmp("-o",argv[arg_index]) || (++arg_index>=argc)) {
		synctex_help_view("Missing -o required argument");
		return -1;
	}
	char * start = argv[arg_index];
	char * end = NULL;
	page = strtol(start,&end,10);
	if(end>start && strlen(end)>1 && *end==':') {
		start = end+1;
		x = strtof(start,&end);
		if(end>start && strlen(end)>1 && *end==':') {
			y = strtof(start,&end);
			if(end>start && strlen(end)>1 && *end==':') {
				output = ++end;
				goto scan_execute;
			}
		}
	}
	synctex_help_edit("Bad -o argument");
	return -1;
scan_execute:
	/* now scan the optional arguments */
	if(++arg_index<argc) {
		if(0 == strcmp("-x",argv[arg_index])) {
			if(++arg_index<argc) {
				if(strcmp("-",argv[arg_index])) {
					/* next option does not start with '-', this is a command */
					editor = argv[arg_index];
					if(++arg_index<argc) {
						goto option_hint;
					} else {
						goto proceed;
					}
				} else {
					/* retrieve the environment variable */
					editor = getenv("SYNCTEX_EDITOR");
					goto option_hint;
				}
			} else {
				editor = getenv("SYNCTEX_EDITOR");
				goto proceed;
			}
		}
option_hint:
		if(0 == strcmp("-h",argv[arg_index]) && ++arg_index<argc) {
			
			start = argv[arg_index];
			end = NULL;
			offset = strtol(start,&end,10);
			if(end>start && strlen(end)>1 && *end==':') {
				context = end+1;
				goto proceed;
			}
			synctex_help_edit("Bad -h argument");
			return -1;
		}
	}
	synctex_scanner_t scanner = NULL;
	char * synctex = NULL;
	size_t size = 0;
proceed:
#if SYNCTEX_DEBUG
	printf("page:%i\n",page);
	printf("x:%f\n",x);
	printf("y:%f\n",y);
	printf("output:%s\n",output);
	printf("editor:%s\n",editor);
	printf("offset:%i\n",offset);
	printf("context:%s\n",context);
	printf("cwd:%s\n",getcwd(NULL,0));
#endif
	size = strlen(output)+9;
	synctex = (char *)malloc(size);
	if(NULL == synctex) {
		synctex_help_edit("No more memory");
		return -1;
	}
	if(0 == strlcpy(synctex,output,size)) {
		synctex_help_edit("Copy problem");
		return -1;
	}
	char * ptr = synctex;
	char * where = NULL;
	/* remove the path extension of the last path component. */
	if(where = strstr(ptr,SYNCTEX_PATH_COMPONENTS_SEPARATOR)) {
		do {
			ptr = where;
		} while(where = strstr(ptr+1,SYNCTEX_PATH_COMPONENTS_SEPARATOR));
	}
	if(where = strstr(ptr,SYNCTEX_PATH_EXTENSION_SEPARATOR)) {
		do {
			ptr = where;
		} while(where = strstr(ptr+1,SYNCTEX_PATH_EXTENSION_SEPARATOR));
		*ptr = '\0';
	}
	if(0 == strlcat(synctex,".synctex",size)){
		synctex_help_edit("Concatenation problem");
		return -1;
	}
	size = 0;
	scanner = synctex_scanner_new_with_contents_of_file(synctex);
	free(synctex);
	synctex = NULL;
	if(scanner && synctex_edit_query(scanner,page,x,y)) {
		synctex_node_t node = NULL;
		char * input = NULL;
		if((node = synctex_next_result(scanner))
				&& (input = (char *)synctex_scanner_get_name(scanner,synctex_node_tag(node)))) {
			/* filtering the command */
			if(strlen(editor)) {
				/* Replace %{ by &{, then remove all unescaped '%'*/
				while(where = strstr(editor,"%{")) {
					*where = '&';
				}
				where = editor;
				while(where = strstr(editor,"%")) {
					if(strlen(++where)) {
						if(*where == '%') {
							++where;
						} else {
							*(where-1)='&';
						}
					}
				}
				char * dictionary [] = {
					"&{output}","%1$s",
					"&{input}","%2$s",
					"&{line}","%3$i",
					"&{column}","%4$i",
					"&{offset}","%5$i",
					"&{context}","%6$s"};
				int i = 12;
				while(i>0) {
					char * value = dictionary[--i];
					char * key = dictionary[--i];
find_a_key:
					where = strstr(editor,key);
					if(where) {
						memcpy(where,value,strlen(value));
						memmove(where+strlen(value),where+strlen(key),strlen(where)-strlen(key)+1);
						goto find_a_key;
					}
				}
				size = strlen(editor)+3*sizeof(int)+6*sizeof(float)+4*(65536)+1;
				char * buffer = malloc(size);
				if(NULL == buffer) {
					printf("SyncTeX ERROR: No memory available\n",editor);
					return -1;
				}
				if(size-1<=snprintf(buffer,size,editor,
							output,
							input,
							synctex_node_line(node),
							synctex_node_column(node),
							offset,
							(context?context:""))) {
					/* Not all the characters were printed*/
					free(buffer);
					buffer= NULL;
					printf("SyncTeX ERROR: Problem with\n%s\n",editor);
					return -1;
				}
				printf("SyncTeX: Executing\n%s\n",buffer);
				int status = system(buffer);
				free(buffer);
				buffer = NULL;
				return status;
			} else {
				/* just print out the results */
				puts("SyncTeX result begin\n");
				do {
					printf(	"Output:%s\n"
							"Input:%s\n"
							"Line:%f\n"
							"Column:%s\n"
							"Offset:%i\n"
							"Context:%s\n",
							output,
							input,
							synctex_node_line(node),
							synctex_node_column(node),
							offset,
							(context?context:""));
				} while(node = synctex_next_result(scanner));
				puts("SyncTeX result end\n");
			}
		}
	}
	return 0;
}

void synctex_help_update(char * error,...) {
	va_list v;
	va_start(v, error);
	synctex_usage(error, v);
	va_end(v);
	fputs(
	"synctex update: up to date synctex file,\n"
	"Use this command to update the synctex file once a dvi/xdv to pdf filter is applied.\n\n"
	"\n"
	"usage: synctex update -o output [-m number] [-x dimension] [-y dimension]\n"
	"\n"
    "-o output     is the file to which the filter applies.\n"
	"-m number     Set additional magnification\n"
    "-x dimension  Set horizontal offset\n"
	"-y dimension  Set vertical offset\n"
	"In general, these are exactly the options provided to the dvi/xdv to pdf filter.\n",
	(error?stderr:stdout)
	);
	return;
}

/*		"usage: synctex update -o output [-m number] [-x dimension] [-y dim output\n"  */
int synctex_update(int argc, char *argv[]) {
	int arg_index = 0;
	if(arg_index>=argc) {
		synctex_help_update("Bad update command");
		return -1;
	}
	char * magnification = NULL;
	char * x = NULL;
	char * y = NULL;
	char * output = NULL;
	/* required */
	if((arg_index>=argc) || strcmp("-o",argv[arg_index]) || (++arg_index>=argc)) {
		synctex_help_update("Missing -o required argument");
		return -1;
	}
	output = argv[arg_index];
	if(++arg_index>=argc) {
		return 0;
	}
next_argument:
	if(0 == strcmp("-m",argv[arg_index])) {
		if(++arg_index>=argc) {
			synctex_help_update("Missing magnification");
			return -1;
		}
		magnification = argv[arg_index];
prepare_next_argument:
		if(++arg_index<argc) {
			goto next_argument;
		}
	} else if(0 == strcmp("-x",argv[arg_index])) {
		if(++arg_index>=argc) {
			synctex_help_update("Missing x offset");
			return -1;
		}
		x = argv[arg_index];
		goto prepare_next_argument;
	} else if(0 == strcmp("-y",argv[arg_index])) {
		if(++arg_index>=argc) {
			synctex_help_update("Missing y offset");
			return -1;
		}
		y = argv[arg_index];
		goto prepare_next_argument;
	}
	/* Arguments parsed */
	size_t size = strlen(output)+9;
	char * synctex = (char *)malloc(size);
	if(NULL == synctex) {
		synctex_help_update("No more memory");
		return -1;
	}
	if(0 == strlcpy(synctex,output,size)) {
		synctex_help_update("Copy problem");
		return -1;
	}
	if(NULL == x
		&& NULL == y
			&& NULL == magnification) {
		/* Nothing to update */
		printf("SyncTeX: Nothing to update\n");
		return 0;
	}
	char * ptr = synctex;
	char * where = NULL;
	/* remove the path extension of the last path component. */
	if(where = strstr(ptr,SYNCTEX_PATH_COMPONENTS_SEPARATOR)) {
		do {
			ptr = where;
		} while(where = strstr(ptr+1,SYNCTEX_PATH_COMPONENTS_SEPARATOR));
	}
	if(where = strstr(ptr,SYNCTEX_PATH_EXTENSION_SEPARATOR)) {
		do {
			ptr = where;
		} while(where = strstr(ptr+1,SYNCTEX_PATH_EXTENSION_SEPARATOR));
		*ptr = '\0';
	}
	if(0 == strlcat(synctex,".synctex",size)){
		synctex_help_update("Concatenation problem");
		return -1;
	}
	FILE * F = fopen(synctex,"a");
	if(NULL == F) {
		synctex_help_update("No file at: %s",synctex);
		return -1;
	}
	printf("SyncTeX: updating %s...",synctex);
	int len = 0;
	if(magnification) {
		len += fprintf(F,"Magnification:%s\n",magnification);
	}
	if(x) {
		len += fprintf(F,"X Offset:%s\n",x);
	}
	if(y) {
		len += fprintf(F,"Y Offset:%s\n",y);
	}
	if(len) {
		fprintf(F,"!%i\n",len);
	}
	fclose(F);
	printf(" done\n");
	return 0;
}
