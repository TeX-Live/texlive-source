/*
 * Gregorio is a program that translates gabc files to GregorioTeX
 * This file implements the command line interface of Gregorio.
 *
 * Copyright (C) 2006-2015 The Gregorio Project (see CONTRIBUTORS.md)
 *
 * This file is part of Gregorio.
 * 
 * Gregorio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gregorio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Gregorio.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#ifdef USE_KPSE
#include <kpathsea/kpathsea.h>
#define gregorio_basename xbasename
#else
#include <getopt.h>
#include <libgen.h> /* for basename */
#define gregorio_basename basename
#endif
#include <string.h> /* for strcmp */
#include <locale.h>
#include <limits.h>
#include <errno.h>
#include "struct.h"
#include "plugins.h"
#include "messages.h"
#include "characters.h"
#include "support.h"
#include "gabc/gabc.h"
#include "vowel/vowel.h"

#ifndef MODULE_PATH_ENV
#define MODULE_PATH_ENV        "MODULE_PATH"
#endif

typedef enum gregorio_file_format {
    FORMAT_UNSET = 0,
    GABC,
    GTEX,
    DUMP
} gregorio_file_format;

#define GABC_STR "gabc"
#define GTEX_STR "gtex"
#define DUMP_STR "dump"

#define DEFAULT_INPUT_FORMAT    GABC
#define DEFAULT_OUTPUT_FORMAT   GTEX

/* realpath is not in mingw32 */
#ifdef _WIN32
/* _MAX_PATH is being passed for the maxLength (third) argument of _fullpath,
 * but we are always passing NULL for the absPath (first) argument, so it will
 * be ignored per the MSDN documentation */
#define gregorio_realpath(path,resolved_path) _fullpath(resolved_path,path,_MAX_PATH)
#else
#ifdef FUNC_REALPATH_WORKS
#define gregorio_realpath(path,resolved_path) realpath(path,resolved_path)
#else
/* When realpath doesn't work (on an older system), we are forced to use
 * PATH_MAX to allocate a buffer */
#define gregorio_realpath(path,resolved_path) realpath(path,gregorio_malloc(PATH_MAX))
#endif /* FUNC_REALPATH_WORKS */
#endif /* _WIN32 */

/* define_path attempts to canonicalize the pathname of a given string */
static char *define_path(char *current_directory, char *string)
{
    int length;
    char *file_name;
    char *temp_name;
    char *base_name;
#ifdef _WIN32
    char *last_backslash;
#endif

    temp_name = gregorio_strdup(string);
    base_name = strrchr(temp_name, '/');
#ifdef _WIN32
    last_backslash = strrchr(temp_name, '\\');
    if (last_backslash > base_name) {
        base_name = last_backslash;
    }
#endif
    if (base_name) {
        /* some path was supplied */

        *base_name = '\0';
        base_name++;

        /* try to resolve it */
        file_name = gregorio_realpath(temp_name, NULL);
        if (!file_name) {
            fprintf(stderr, "the directory %s for %s does not exist\n",
                    temp_name, base_name);
            exit(1);
        }
    } else {
        /* no path was supplied */
        base_name = string;
        file_name = gregorio_malloc(
                strlen(current_directory) + strlen(base_name) + 2);
        strcpy(file_name, current_directory);
    }

    /* build the file name */
    length = strlen(file_name);
    file_name = gregorio_realloc(file_name, length + strlen(base_name) + 2);
    file_name[length] = '/';
    strcpy(file_name + length + 1, base_name);

    free(temp_name);
    return file_name;
}

/* function that returns the filename without the extension */
static char *get_base_filename(char *fbasename)
{
    char *p;
    int l;
    char *ret;
    p = strrchr(fbasename, '.');
    if (!p) {
        return NULL;
    }
    l = strlen(fbasename) - strlen(p);
    ret = (char *) gregorio_malloc(l + 1);
    gregorio_snprintf(ret, l + 1, "%s", fbasename);
    ret[l] = '\0';
    return ret;
}

/* function that adds the good extension to a basename (without extension) */
static char *get_output_filename(char *fbasename, const char *extension)
{
    char *output_filename = NULL;
    output_filename =
        (char *) gregorio_malloc((strlen(extension) + strlen(fbasename) + 2));
    output_filename = strcpy(output_filename, fbasename);
    output_filename = strcat(output_filename, ".");
    output_filename = strcat(output_filename, extension);
    return output_filename;
}

/*
 * the type definitions of the function to read a score from a file, and to
 * write a score to a file. Necessary for the libtool stuff... 
 */

static void print_licence(void)
{
    printf("\n\
Tools for manipulation of gregorian chant files\n\
Copyright (C) 2006-2015 Gregorio project authors (see CONTRIBUTORS.md)\n\
\n\
This program is free software: you can redistribute it and/or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, either version 3 of the License, or\n\
(at your option) any later version.\n\
\n");
    printf("This program is distributed in the hope that it will be useful,\n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n");
}

static void print_usage(char *name)
{
    printf(_("\nUsage :\n%s [OPTION] {file}\n  where OPTION is :\n\
\t-o file    writes output to specified file\n\
\t-S         writes output to stdout\n\
\t-F format  specifies output file format (default: gtex)\n\
\t-l file    writes messages output to specified file (default: stderr)\n\
\t-f format  specifies input file format (default: gabc)\n\
\t-s         reads input from stdin\n\
\t-p         generate point-and-click information\n\
\t-h         displays this message\n\
\t-V         displays %s version\n"), name, name);
    printf(_("\t-L         displays licence\n\
\t-v         verbose mode\n\
\t-W         displays all warnings\n\
\n\
available formats are:\n\
\t gabc      gabc\n\
\t gtex      GregorioTeX\n\
\t dump      simple text dump (for debugging purpose)\n\
\n"));
}

static void check_input_clobber(char *input_file_name, char *output_file_name)
{
    if (input_file_name && output_file_name) {
        char *absolute_input_file_name;
        char *absolute_output_file_name;
        char *current_directory;
        int file_cmp;
        size_t bufsize = 128;
        char *buf = gregorio_malloc(bufsize);
        while ((current_directory = getcwd(buf, bufsize)) == NULL
                && errno == ERANGE && bufsize < MAX_BUF_GROWTH) {
            free(buf);
            bufsize <<= 1;
            buf = gregorio_malloc(bufsize);
        }
        if (current_directory == NULL) {
            fprintf(stderr, _("can't determine current directory"));
            free(buf);
            exit(1);
        }
        absolute_input_file_name = define_path(current_directory, input_file_name);
        absolute_output_file_name = define_path(current_directory, output_file_name);
        file_cmp = strcmp(absolute_input_file_name, absolute_output_file_name);
        if (file_cmp == 0) {
            fprintf(stderr, "error: refusing to overwrite the input file\n");
        }
        free(buf);
        free(absolute_input_file_name);
        free(absolute_output_file_name);
        if (file_cmp == 0) {
            exit(1);
        }
    }
}

static char *encode_point_and_click_filename(char *input_file_name)
{
    /* percent-encoding favors capital hex digits */
    static const char *const hex = "0123456789ABCDEF";
    char *filename, *result = NULL, *r = NULL, *p;

    filename = gregorio_realpath(input_file_name, NULL);
    if (!filename) {
        fprintf(stderr, "error: unable to resolve %s\n", input_file_name);
        exit(1);
    }

    /* 2 extra characters for a possible leading slash and final NUL */
    r = result = gregorio_malloc(strlen(filename) * 4 + 2);

#ifdef _WIN32
    *(r++) = '/';
#endif

    for (p = filename; *p; ++p) {
#ifdef _WIN32
        if (*p == '\\') {
            *p = '/';
        }
#endif

        /* note that -, _ and ~ are conspicuously missing from this list
         * because they cause trouble in TeX; we will percent-encode them */
        if ((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p < 'z')
                || (*p >= '0' && *p <= '9') || *p == '.' || *p == '/'
#ifdef _WIN32
                || *p == ':'
#endif
                ) {
            *(r++) = *p;
        }
        else {
            /* percent-encode anything else */
            *(r++) = '\\'; /* must escape it because it's TeX */
            *(r++) = '%';
            *(r++) = hex[(*p >> 4) & 0x0FU];
            *(r++) = hex[*p & 0x0FU];
        }
    }

    *r = '\0';

    free(filename);
    return result;
}

int main(int argc, char **argv)
{
    const char *copyright =
        "Copyright (C) 2006-2015 Gregorio project authors (see CONTRIBUTORS.md)";
    int c;

    char *input_file_name = NULL;
    char *output_file_name = NULL;
    char *output_basename = NULL;
    char *error_file_name = NULL;
    FILE *input_file = NULL;
    FILE *output_file = NULL;
    FILE *error_file = NULL;
    gregorio_file_format input_format = FORMAT_UNSET;
    gregorio_file_format output_format = FORMAT_UNSET;
    gregorio_verbosity verb_mode = 0;
    bool point_and_click = false;
    char *point_and_click_filename = NULL;
    int number_of_options = 0;
    int option_index = 0;
    static struct option long_options[] = {
        {"output-file", 1, 0, 'o'},
        {"stdout", 0, 0, 'S'},
        {"output-format", 1, 0, 'F'},
        {"messages-file", 1, 0, 'l'},
        {"input-format", 1, 0, 'f'},
        {"stdin", 0, 0, 's'},
        {"help", 0, 0, 'h'},
        {"version", 0, 0, 'V'},
        {"licence", 0, 0, 'L'},
        {"verbose", 0, 0, 'v'},
        {"all-warnings", 0, 0, 'W'},
        {"point-and-click", 0, 0, 'p'},
    };
    gregorio_score *score = NULL;

    #ifdef USE_KPSE
        kpse_set_program_name(argv[0], "gregorio");
    #endif
    if (argc == 1) {
        print_usage(argv[0]);
        exit(0);
    }
    setlocale(LC_CTYPE, "C");

    while (1) {
        c = getopt_long(argc, argv, "o:SF:l:f:shOLVvWp",
                        long_options, &option_index);
        if (c == -1)
            break;
        switch (c) {
        case 'o':
            if (output_file_name) {
                fprintf(stderr,
                        "warning: several output files declared, %s taken\n",
                        output_file_name);
                break;
            }
            if (output_file) {  /* means that stdout is defined */
                fprintf(stderr,
                        "warning: can't write to file and stdout, writing on stdout\n");
                break;
            }
            output_file_name = optarg;
            break;
        case 'S':
            if (output_file_name) {
                fprintf(stderr,
                        "warning: can't write to file and stdout, writing on %s\n",
                        output_file_name);
                break;
            }
            if (output_file) {  /* means that stdout is defined */
                fprintf(stderr, "warning: option used two times: %c\n", c);
                break;
            }
            output_file = stdout;
            break;
        case 'F':
            if (output_format) {
                fprintf(stderr,
                        "warning: several output formats declared, first taken\n");
                break;
            }
            if (!strcmp(optarg, GABC_STR)) {
                output_format = GABC;
                break;
            }
            if (!strcmp(optarg, GTEX_STR)) {
                output_format = GTEX;
                break;
            }
            if (!strcmp(optarg, DUMP_STR)) {
                output_format = DUMP;
                break;
            } else {
                fprintf(stderr, "error: unknown output format: %s\n", optarg);
                exit(0);
            }
            break;
        case 'l':
            if (error_file_name) {
                fprintf(stderr,
                        "warning: several error files declared, %s taken\n",
                        error_file_name);
                break;
            }
            error_file_name = optarg;
            break;
        case 'f':
            if (input_format) {
                gregorio_set_error_out(error_file);
                fprintf(stderr,
                        "warning: several output formats declared, first taken\n");
                break;
            }
            if (!strcmp(optarg, GABC_STR)) {
                input_format = GABC;
                break;
            } else {
                fprintf(stderr, "error: unknown input format: %s\n", optarg);
                exit(0);
            }
            break;
        case 's':
            if (input_file_name) {
                fprintf(stderr,
                        "warning: can't read from file and stdin, writing on %s\n",
                        input_file_name);
                break;
            }
            if (input_file) { /* means that stdin is defined */
                fprintf(stderr, "warning: option used two times: %c\n", c);
                break;
            }
            input_file = stdin;
            break;
        case 'h':
            print_usage(argv[0]);
            exit(0);
            break;
        case 'V':
            printf("Gregorio version %s.\n%s\n", GREGORIO_VERSION, copyright);
            exit(0);
            break;
        case 'v':
            if (verb_mode && verb_mode != VERBOSITY_WARNING) {
                fprintf(stderr, "warning: verbose option passed two times\n");
                break;
            }
            verb_mode = VERBOSITY_INFO;
            break;
        case 'W':
            if (verb_mode == VERBOSITY_WARNING) {
                fprintf(stderr,
                        "warning: all-warnings option passed two times\n");
                break;
            }
            if (verb_mode != VERBOSITY_INFO) {
                verb_mode = VERBOSITY_WARNING;
            }
            break;
        case 'L':
            print_licence();
            exit(0);
            break;
        case 'p':
            if (point_and_click) {
                fprintf(stderr,
                        "warning: point-and-click option passed two times\n");
                break;
            }
            point_and_click = true;
            break;
        case '?':
            break;
        default:
            fprintf(stderr, "unknown option: %c\n", c);
            print_usage(argv[0]);
            exit(0);
            break;
        }
        number_of_options++;
    } /* end of while */
    if (optind == argc) {
        if (!input_file) { /* input not undefined (could be stdin) */
            fprintf(stderr, "error: no input file specified\n");
            print_usage(argv[0]);
            exit(1);
        }
    } else {
        input_file_name = argv[optind];
        output_basename = get_base_filename(input_file_name);
        if (input_file) {
            fprintf(stderr,
                    "warning: can't read from stdin and a file, reading from file %s\n",
                    input_file_name);
            input_file = NULL;
        }
    }
    optind++;
    if (optind < argc) {
        printf("ignored arguments: ");
        while (number_of_options < argc)
            printf("%s ", argv[number_of_options++]);
        printf("\n");
    }

    if (!input_format) {
        input_format = DEFAULT_INPUT_FORMAT;
    }

    if (!output_format) {
        output_format = DEFAULT_OUTPUT_FORMAT;
    }

    #ifdef USE_KPSE
        if (!kpse_in_name_ok(input_file_name)) {
            fprintf(stderr, "Error: kpse doesn't allow to read from file  %s\n",
                    input_file_name);
            exit(1);
        }
    #endif

    /* then we act... */

    if (!output_file_name && !output_file) {
        if (!output_basename) {
            output_file = stdout;
        } else {
            if (input_format != output_format) {
                switch (output_format) {
                case GABC:
                    output_file_name =
                        get_output_filename(output_basename, "gabc");
                    break;
                case GTEX:
                    output_file_name =
                        get_output_filename(output_basename, "gtex");
                    break;
                case DUMP:
                    output_file_name =
                        get_output_filename(output_basename, "dump");
                    break;
                default:
                    fprintf(stderr, "error: unsupported format");
                    exit(1);
                }
            }
        }
    }
    if (output_basename) {
        free(output_basename);
    }

    if (!output_file) {
        if (!input_file) {
            check_input_clobber(input_file_name, output_file_name);
        }
        #ifdef USE_KPSE
            if (!kpse_out_name_ok(output_file_name)) {
                fprintf(stderr, "Error: kpse doesn't allow to write in file  %s\n",
                        output_file_name);
                exit(1);
            }
        #endif
        output_file = fopen(output_file_name, "wb");
        if (!output_file) {
            fprintf(stderr, "error: can't write in file %s",
                    output_file_name);
        }
    }

    /* we always have input_file or input_file_name */
    if (input_file) {
        if (point_and_click) {
            fprintf(stderr,
                    "warning: disabling point-and-click since reading from stdin\n");
        }
    } else {
        input_file = fopen(input_file_name, "r");
        if (!input_file) {
            fprintf(stderr, "error: can't open file %s for reading\n",
                    input_file_name);
            exit(1);
        }
        gregorio_set_file_name(gregorio_basename(input_file_name));
        if (point_and_click) {
            point_and_click_filename = encode_point_and_click_filename(
                    input_file_name);
        }
    }

    if (!error_file_name) {
        error_file = stderr;
        gregorio_set_error_out(error_file);
    } else {
        error_file = fopen(error_file_name, "wb");
        if (!error_file) {
            fprintf(stderr, "error: can't open file %s for writing\n",
                    error_file_name);
            exit(1);
        }
        gregorio_set_error_out(error_file);
    }

    if (!verb_mode) {
        verb_mode = VERBOSITY_DEPRECATION;
    }

    gregorio_set_verbosity_mode(verb_mode);

    switch (input_format) {
    case GABC:
        score = gabc_read_score(input_file);
        break;
    default:
        fprintf(stderr, "error : invalid input format\n");
        fclose(input_file);
        fclose(output_file);
        exit(1);
        break;
    }

    fclose(input_file);
    if (score == NULL) {
        fclose(output_file);
        fprintf(stderr, "error in file parsing\n");
        exit(1);
    }

    gregorio_fix_initial_keys(score, gregorio_default_clef);

    switch (output_format) {
    case GABC:
        gabc_write_score(output_file, score);
        break;
    case GTEX:
        gregoriotex_write_score(output_file, score, point_and_click_filename);
        break;
    case DUMP:
        dump_write_score(output_file, score);
        break;
    default:
        fprintf(stderr, "error : invalid output format\n");
        gregorio_free_score(score);
        fclose(output_file);
        exit(1);
        break;
    }
    fclose(output_file);
    if (point_and_click_filename) {
        free(point_and_click_filename);
    }
    gregorio_free_score(score);
    gregorio_vowel_tables_free();
    gabc_score_determination_lex_destroy();
    gabc_notes_determination_lex_destroy();
    gregorio_vowel_rulefile_lex_destroy();
    if (error_file_name) {
        fclose(error_file);
    }

    exit(gregorio_get_return_value());
}
