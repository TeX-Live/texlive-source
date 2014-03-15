% luainit.w
%
% Copyright 2006-2014 Taco Hoekwater <taco@@luatex.org>
%
% This file is part of LuaTeX.
%
% LuaTeX is free software; you can redistribute it and/or modify it under
% the terms of the GNU General Public License as published by the Free
% Software Foundation; either version 2 of the License, or (at your
% option) any later version.
%
% LuaTeX is distributed in the hope that it will be useful, but WITHOUT
% ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
% FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
% License for more details.
%
% You should have received a copy of the GNU General Public License along
% with LuaTeX; if not, see <http://www.gnu.org/licenses/>.

@ @c
static const char _svn_version[] =
    "$Id: luainit.w 4881 2014-03-14 12:51:16Z taco $"
    "$URL: https://foundry.supelec.fr/svn/luatex/trunk/source/texk/web2c/luatexdir/lua/luainit.w $";

#include "ptexlib.h"

#include <kpathsea/c-stat.h>

#include "lua/luatex-api.h"

/* internalized strings: see luatex-api.h */
make_lua_key(LTL);
make_lua_key(MathConstants);
make_lua_key(RTT);
make_lua_key(TLT);
make_lua_key(TRT);
make_lua_key(accent);
make_lua_key(action);
make_lua_key(action_id);
make_lua_key(action_type);
make_lua_key(additional);
make_lua_key(adjust_head);
make_lua_key(advance);
make_lua_key(aleph);
make_lua_key(align_head);
make_lua_key(area);
make_lua_key(attr);
make_lua_key(attributes);
make_lua_key(auto_expand);
make_lua_key(best_ins_ptr);
make_lua_key(best_page_break);
make_lua_key(best_size);
make_lua_key(bot);
make_lua_key(bot_accent);
make_lua_key(bottom_left);
make_lua_key(bottom_right);
make_lua_key(box_left);
make_lua_key(box_left_width);
make_lua_key(box_right);
make_lua_key(box_right_width);
make_lua_key(broken_ins);
make_lua_key(broken_ptr);
make_lua_key(cache);
make_lua_key(cal_expand_ratio);
make_lua_key(char);
make_lua_key(characters);
make_lua_key(checksum);
make_lua_key(cidinfo);
make_lua_key(class);
make_lua_key(command);
make_lua_key(commands);
make_lua_key(comment);
make_lua_key(components);
make_lua_key(contrib_head);
make_lua_key(core);
make_lua_key(cost);
make_lua_key(count);
make_lua_key(data);
make_lua_key(degree);
make_lua_key(delim);
make_lua_key(delimptr);
make_lua_key(denom);
make_lua_key(depth);
make_lua_key(designsize);
make_lua_key(dest_id);
make_lua_key(dest_type);
make_lua_key(dir);
make_lua_key(dir_h);
make_lua_key(direction);
make_lua_key(dirs);
make_lua_key(display);
make_lua_key(down);
make_lua_key(dvi_ptr);
make_lua_key(embedding);
make_lua_key(encodingbytes);
make_lua_key(encodingname);
make_lua_key(end);
make_lua_key(etex);
make_lua_key(exactly);
make_lua_key(expansion_factor);
make_lua_key(ext);
make_lua_key(extend);
make_lua_key(extender);
make_lua_key(extensible);
make_lua_key(extra_space);
make_lua_key(fam);
make_lua_key(fast);
make_lua_key(file);
make_lua_key(filename);
make_lua_key(font);
make_lua_key(fonts);
make_lua_key(format);
make_lua_key(fullname);
make_lua_key(global);
make_lua_key(glue_order);
make_lua_key(glue_set);
make_lua_key(glue_sign);
make_lua_key(glyph);
make_lua_key(head);
make_lua_key(height);
make_lua_key(hold_head);
make_lua_key(horiz_variants);
make_lua_key(hyphenchar);
make_lua_key(id);
make_lua_key(image);
make_lua_key(index);
make_lua_key(italic);
make_lua_key(kern);
make_lua_key(kerns);
make_lua_key(lang);
make_lua_key(large_char);
make_lua_key(large_fam);
make_lua_key(last_ins_ptr);
make_lua_key(leader);
make_lua_key(least_page_cost);
make_lua_key(left);
make_lua_key(left_boundary);
make_lua_key(left_protruding);
make_lua_key(level);
make_lua_key(ligatures);
make_lua_key(link_attr);
make_lua_key(list);
make_lua_key(log);
make_lua_key(lua);
make_lua_key(luatex);
make_lua_key(luatex_node);
make_lua_key(mLTL);
make_lua_key(mRTT);
make_lua_key(mTLT);
make_lua_key(mTRT);
make_lua_key(mark);
make_lua_key(mathdir);
make_lua_key(mathkern);
make_lua_key(mathstyle);
make_lua_key(mid);
make_lua_key(mode);
make_lua_key(modeline);
make_lua_key(name);
make_lua_key(named_id);
make_lua_key(new_window);
make_lua_key(next);
make_lua_key(no);
make_lua_key(noad);
make_lua_key(node);
make_lua_key(node_properties);
make_lua_key(node_properties_indirect);
make_lua_key(nomath);
make_lua_key(nop);
make_lua_key(nucleus);
make_lua_key(num);
make_lua_key(number);
make_lua_key(objnum);
make_lua_key(omega);
make_lua_key(ordering);
make_lua_key(pLTL);
make_lua_key(pRTT);
make_lua_key(pTLT);
make_lua_key(pTRT);
make_lua_key(page_head);
make_lua_key(page_ins_head);
make_lua_key(parameters);
make_lua_key(pdftex);
make_lua_key(pen_broken);
make_lua_key(pen_inter);
make_lua_key(penalty);
make_lua_key(pop);
make_lua_key(post);
make_lua_key(pre);
make_lua_key(pre_adjust_head);
make_lua_key(prev);
make_lua_key(prevdepth);
make_lua_key(prevgraf);
make_lua_key(psname);
make_lua_key(ptr);
make_lua_key(push);
make_lua_key(quad);
make_lua_key(ref_count);
make_lua_key(reg);
make_lua_key(registry);
make_lua_key(renew);
make_lua_key(rep);
make_lua_key(replace);
make_lua_key(right);
make_lua_key(right_boundary);
make_lua_key(right_protruding);
make_lua_key(rule);
make_lua_key(scale);
make_lua_key(script);
make_lua_key(scriptscript);
make_lua_key(shift);
make_lua_key(shrink);
make_lua_key(shrink_order);
make_lua_key(size);
make_lua_key(skewchar);
make_lua_key(slant);
make_lua_key(slot);
make_lua_key(small_char);
make_lua_key(small_fam);
make_lua_key(space);
make_lua_key(space_shrink);
make_lua_key(space_stretch);
make_lua_key(spacefactor);
make_lua_key(spec);
make_lua_key(special);
make_lua_key(stack);
make_lua_key(start);
make_lua_key(step);
make_lua_key(stream);
make_lua_key(stretch);
make_lua_key(stretch_order);
make_lua_key(string);
make_lua_key(style);
make_lua_key(sub);
make_lua_key(subst_ex_font);
make_lua_key(subtype);
make_lua_key(sup);
make_lua_key(supplement);
make_lua_key(surround);
make_lua_key(tail);
make_lua_key(temp_head);
make_lua_key(term);
make_lua_key(term_and_log);
make_lua_key(tex);
make_lua_key(text);
make_lua_key(thread_attr);
make_lua_key(thread_id);
make_lua_key(top);
make_lua_key(top_accent);
make_lua_key(top_left);
make_lua_key(top_right);
make_lua_key(tounicode);
make_lua_key(transform);
make_lua_key(type);
make_lua_key(uchyph);
make_lua_key(umath);
make_lua_key(units_per_em);
make_lua_key(used);
make_lua_key(user_id);
make_lua_key(value);
make_lua_key(version);
make_lua_key(vert_variants);
make_lua_key(width);
make_lua_key(writable);
make_lua_key(x_height);
make_lua_key(xoffset);
make_lua_key(xyz_zoom);
make_lua_key(yoffset);

make_lua_key(immediate);
make_lua_key(compresslevel);
make_lua_key(objcompression);
make_lua_key(direct);
make_lua_key(page);
make_lua_key(streamfile);
make_lua_key(annot);
make_lua_key(lua_functions);

@
TH: TODO

This file is getting a bit messy, but it is not simple to fix unilaterally.

Better to wait until Karl has some time (after texlive 2008) so we can
synchronize with kpathsea. One problem, for instance, is that I would
like to resolve the full executable path.  |kpse_set_program_name()| does
that, indirectly (by setting SELFAUTOLOC in the environment), but it
does much more, making it hard to use for our purpose. 

In fact, it sets three C variables:

  |kpse_invocation_name|  |kpse_invocation_short_name|  |kpse->program_name|

and five environment variables:

  SELFAUTOLOC  SELFAUTODIR  SELFAUTOPARENT  SELFAUTOGRANDPARENT  progname

@c
const_string LUATEX_IHELP[] = {
#ifdef LuajitTeX
    "Usage: luajittex --lua=FILE [OPTION]... [TEXNAME[.tex]] [COMMANDS]",
    "   or: luajittex --lua=FILE [OPTION]... \\FIRST-LINE",
    "   or: luajittex --lua=FILE [OPTION]... &FMT ARGS",
    "  Run LuajitTeX on TEXNAME, usually creating TEXNAME.pdf.",
#else
    "Usage: luatex --lua=FILE [OPTION]... [TEXNAME[.tex]] [COMMANDS]",
    "   or: luatex --lua=FILE [OPTION]... \\FIRST-LINE",
    "   or: luatex --lua=FILE [OPTION]... &FMT ARGS",
    "  Run LuaTeX on TEXNAME, usually creating TEXNAME.pdf.",
#endif
    "  Any remaining COMMANDS are processed as luatex input, after TEXNAME is read.",
    "",
    "  Alternatively, if the first non-option argument begins with a backslash,",
#ifdef LuajitTeX
    "  luajittex interprets all non-option arguments as an input line.",
#else
    "  luatex interprets all non-option arguments as an input line.",
#endif
    "",
    "  Alternatively, if the first non-option argument begins with a &, the",
    "  next word is taken as the FMT to read, overriding all else.  Any",
    "  remaining arguments are processed as above.",
    "",
    "  If no arguments or options are specified, prompt for input.",
    "",
    "  The following regular options are understood: ",
    "",
    "   --8bit                        ignored, input is assumed to be in UTF-8 encoding",
    "   --credits                     display credits and exit",
    "   --debug-format                enable format debugging",
    "   --default-translate-file=     ignored, input is assumed to be in UTF-8 encoding",
    "   --disable-write18             disable \\write18{SHELL COMMAND}",
    "   --draftmode                   switch on draft mode (generates no output PDF)",
    "   --enable-write18              enable \\write18{SHELL COMMAND}",
    "   --etex                        ignored, the etex extensions are always active",
    "   --[no-]file-line-error        disable/enable file:line:error style messages",
    "   --[no-]file-line-error-style  aliases of --[no-]file-line-error",
    "   --fmt=FORMAT                  load the format file FORMAT",
    "   --halt-on-error               stop processing at the first error",
    "   --help                        display help and exit",
#ifdef LuajitTeX
    "   --ini                         be iniluajittex, for dumping formats",
#else
    "   --ini                         be iniluatex, for dumping formats",
#endif
    "   --interaction=STRING          set interaction mode (STRING=batchmode/nonstopmode/scrollmode/errorstopmode)",
    "   --jobname=STRING              set the job name to STRING",
    "   --kpathsea-debug=NUMBER       set path searching debugging flags according to the bits of NUMBER",
    "   --lua=s                       load and execute a lua initialization script",
    "   --[no-]mktex=FMT              disable/enable mktexFMT generation (FMT=tex/tfm)",
    "   --nosocket                    disable the lua socket library",
    "   --output-comment=STRING       use STRING for DVI file comment instead of date (no effect for PDF)",
    "   --output-directory=DIR        use existing DIR as the directory to write files in",
    "   --output-format=FORMAT        use FORMAT for job output; FORMAT is 'dvi' or 'pdf'",
    "   --[no-]parse-first-line       disable/enable parsing of the first line of the input file",
    "   --progname=STRING             set the program name to STRING",
    "   --recorder                    enable filename recorder",
    "   --safer                       disable easily exploitable lua commands",
    "   --[no-]shell-escape           disable/enable \\write18{SHELL COMMAND}",
    "   --shell-restricted            restrict \\write18 to a list of commands given in texmf.cnf",
    "   --synctex=NUMBER              enable synctex",
    "   --translate-file=             ignored, input is assumed to be in UTF-8 encoding",
    "   --version                     display version and exit",
    "",
    "Alternate behaviour models can be obtained by special switches",
    "",
    "  --luaonly                run a lua file, then exit",
    "  --luaconly               byte-compile a lua file, then exit",
    "  --luahashchars           the bits used by current Lua interpreter for strings hashing",
#ifdef LuajitTeX
    "  --jiton                  turns the JIT compiler on (default off)",
    "  --jithash=STRING         choose the hash function for the lua strings (lua51|luajit20: default lua51)",
#endif
    "",
    "See the reference manual for more information about the startup process.",
    NULL
};

@ The return value will be the directory of the executable, e.g.: \.{c:/TeX/bin}
@c
static char *ex_selfdir(char *argv0)
{
#if defined(WIN32)
#if defined(__MINGW32__)
    char path[PATH_MAX], *fp;

    /* SearchPath() always gives back an absolute directory */
    if (SearchPath(NULL, argv0, ".exe", PATH_MAX, path, NULL) == 0)
        FATAL1("Can't determine where the executable %s is.\n", argv0);
    /* slashify the dirname */
    for (fp = path; fp && *fp; fp++)
        if (IS_DIR_SEP(*fp))
            *fp = DIR_SEP;
#else /* __MINGW32__ */
#define PATH_MAX 512
    char short_path[PATH_MAX], path[PATH_MAX], *fp;

    /* SearchPath() always gives back an absolute directory */
    if (SearchPath(NULL, argv0, ".exe", PATH_MAX, short_path, &fp) == 0)
        FATAL1("Can't determine where the executable %s is.\n", argv0);
    if (getlongpath(path, short_path, sizeof(path)) == 0) {
        FATAL1("This path points to an invalid file : %s\n", short_path);
    }
#endif /* __MINGW32__ */
   return xdirname(path);
#else /* WIN32 */
    return kpse_selfdir(argv0);
#endif
}




@ @c
static void
prepare_cmdline(lua_State * L, char **av, int ac, int zero_offset)
{
    int i;
    char *s;
    luaL_checkstack(L, ac + 3, "too many arguments to script");
    lua_createtable(L, 0, 0);
    for (i = 0; i < ac; i++) {
        lua_pushstring(L, av[i]);
        lua_rawseti(L, -2, (i - zero_offset));
    }
    lua_setglobal(L, "arg");
    lua_getglobal(L, "os");
    s = ex_selfdir(argv[0]);
    lua_pushstring(L, s);
    xfree(s);
    lua_setfield(L, -2, "selfdir");
    return;
}

@ @c
string input_name = NULL;

static string user_progname = NULL;

char *startup_filename = NULL;
int lua_only = 0;
int lua_offset = 0;
unsigned char show_luahashchars = 0;

#ifdef LuajitTeX
int luajiton   = 0;
char *jithash_hashname = NULL;
#endif

int safer_option = 0;
int nosocket_option = 0;

@ Reading the options.  

@ Test whether getopt found an option ``A''.
Assumes the option index is in the variable |option_index|, and the
option table in a variable |long_options|.  

@c
#define ARGUMENT_IS(a) STREQ (long_options[option_index].name, a)

/* SunOS cc can't initialize automatic structs, so make this static.  */
static struct option long_options[]
= { {"fmt", 1, 0, 0},
{"lua", 1, 0, 0},
{"luaonly", 0, 0, 0},
{"luahashchars", 0, 0, 0},
#ifdef LuajitTeX
{"jiton", 0, 0, 0},
{"jithash", 1, 0, 0},
#endif
{"safer", 0, &safer_option, 1},
{"nosocket", 0, &nosocket_option, 1},
{"help", 0, 0, 0},
{"ini", 0, &ini_version, 1},
{"interaction", 1, 0, 0},
{"halt-on-error", 0, &haltonerrorp, 1},
{"kpathsea-debug", 1, 0, 0},
{"progname", 1, 0, 0},
{"version", 0, 0, 0},
{"credits", 0, 0, 0},
{"recorder", 0, &recorder_enabled, 1},
{"etex", 0, 0, 0},
{"output-comment", 1, 0, 0},
{"output-directory", 1, 0, 0},
{"draftmode", 0, 0, 0},
{"output-format", 1, 0, 0},
{"shell-escape", 0, &shellenabledp, 1},
{"no-shell-escape", 0, &shellenabledp, -1},
{"enable-write18", 0, &shellenabledp, 1},
{"disable-write18", 0, &shellenabledp, -1},
{"shell-restricted", 0, 0, 0},
{"debug-format", 0, &debug_format_file, 1},
{"file-line-error-style", 0, &filelineerrorstylep, 1},
{"no-file-line-error-style", 0, &filelineerrorstylep, -1},
      /* Shorter option names for the above. */
{"file-line-error", 0, &filelineerrorstylep, 1},
{"no-file-line-error", 0, &filelineerrorstylep, -1},
{"jobname", 1, 0, 0},
{"parse-first-line", 0, &parsefirstlinep, 1},
{"no-parse-first-line", 0, &parsefirstlinep, -1},
{"translate-file", 1, 0, 0},
{"default-translate-file", 1, 0, 0},
{"8bit", 0, 0, 0},
{"mktex", 1, 0, 0},
{"no-mktex", 1, 0, 0},
/* Synchronization: just like "interaction" above */
{"synctex", 1, 0, 0},
{0, 0, 0, 0}
};

@ @c
int lua_numeric_field_by_index(lua_State * L, int name_index, int dflt)
{
    register int i = dflt;
    lua_rawgeti(L, LUA_REGISTRYINDEX, name_index);      /* fetch the stringptr */
    lua_rawget(L, -2);
    if (lua_type(L, -1) == LUA_TNUMBER) {
        i = lua_roundnumber(L, -1);
    }
    lua_pop(L, 1);
    return i;
}


@ @c
static void parse_options(int ac, char **av)
{
#ifdef WIN32
/* save argc and argv */
    int sargc = argc;
    char **sargv = argv;
#endif
    int g;                      /* `getopt' return code.  */
    int option_index;
    char *firstfile = NULL;
    opterr = 0;                 /* dont whine */
#ifdef LuajitTeX
    if ((strstr(argv[0], "luajittexlua") != NULL) ||
        (strstr(argv[0], "texluajit") != NULL)) {
#else
    if ((strstr(argv[0], "luatexlua") != NULL) ||
        (strstr(argv[0], "texlua") != NULL)) {
#endif
        lua_only = 1;
        luainit = 1;
    }
    for (;;) {
        g = getopt_long_only(ac, av, "+", long_options, &option_index);

        if (g == -1)            /* End of arguments, exit the loop.  */
            break;
        if (g == '?')  {         /* Unknown option.  */
          if (!luainit) 
            fprintf(stderr,"%s: unrecognized option '%s'\n", argv[0], argv[optind-1]);
          continue;
        }

        assert(g == 0);         /* We have no short option names.  */

        if (ARGUMENT_IS("luaonly")) {
            lua_only = 1;
            lua_offset = optind;
            luainit = 1;
        } else if (ARGUMENT_IS("lua")) {
            startup_filename = optarg;
            lua_offset = (optind - 1);
            luainit = 1;
#ifdef LuajitTeX
        } else if (ARGUMENT_IS("jiton")) {
            luajiton = 1;
        } else if (ARGUMENT_IS("jithash")) {
	      size_t len = strlen(optarg);
	      if (len<16)
		jithash_hashname = optarg;     
	      else{
		WARNING2("hash name truncated to 15 characters from %d. (%s)",      
			 (int) len, optarg);
		jithash_hashname = (string) xmalloc(16);
                strncpy(jithash_hashname, optarg, 15);
                jithash_hashname[15] = 0;
	      }
#endif 

        } else if (ARGUMENT_IS("luahashchars")) {
            show_luahashchars = 1;

        } else if (ARGUMENT_IS("kpathsea-debug")) {
            kpathsea_debug |= atoi(optarg);

        } else if (ARGUMENT_IS("progname")) {
            user_progname = optarg;

        } else if (ARGUMENT_IS("jobname")) {
            c_job_name = optarg;

        } else if (ARGUMENT_IS("fmt")) {
            dump_name = optarg;

        } else if (ARGUMENT_IS("output-directory")) {
            output_directory = optarg;

        } else if (ARGUMENT_IS("output-comment")) {
            size_t len = strlen(optarg);
            if (len < 256) {
                output_comment = optarg;
            } else {
                WARNING2("Comment truncated to 255 characters from %d. (%s)",
                         (int) len, optarg);
                output_comment = (string) xmalloc(256);
                strncpy(output_comment, optarg, 255);
                output_comment[255] = 0;
            }

        } else if (ARGUMENT_IS("shell-restricted")) {
            shellenabledp = 1;
            restrictedshell = 1;

        } else if (ARGUMENT_IS("output-format")) {
            pdf_output_option = 1;
            if (strcmp(optarg, "dvi") == 0) {
                pdf_output_value = 0;
            } else if (strcmp(optarg, "pdf") == 0) {
                pdf_output_value = 2;
            } else {
                WARNING1("Ignoring unknown value `%s' for --output-format",
                         optarg);
                pdf_output_option = 0;
            }

        } else if (ARGUMENT_IS("draftmode")) {
            pdf_draftmode_option = 1;
            pdf_draftmode_value = 1;

        } else if (ARGUMENT_IS("mktex")) {
            kpse_maketex_option(optarg, true);

        } else if (ARGUMENT_IS("no-mktex")) {
            kpse_maketex_option(optarg, false);

        } else if (ARGUMENT_IS("interaction")) {
            /* These numbers match CPP defines */
            if (STREQ(optarg, "batchmode")) {
                interactionoption = 0;
            } else if (STREQ(optarg, "nonstopmode")) {
                interactionoption = 1;
            } else if (STREQ(optarg, "scrollmode")) {
                interactionoption = 2;
            } else if (STREQ(optarg, "errorstopmode")) {
                interactionoption = 3;
            } else {
                WARNING1("Ignoring unknown argument `%s' to --interaction",
                         optarg);
            }

        } else if (ARGUMENT_IS("synctex")) {
            /* Synchronize TeXnology: catching the command line option as a long  */
            synctexoption = (int) strtol(optarg, NULL, 0);

        } else if (ARGUMENT_IS("help")) {
            usagehelp(LUATEX_IHELP, BUG_ADDRESS);

        } else if (ARGUMENT_IS("version")) {
            print_version_banner();
            /* *INDENT-OFF* */
#ifdef LuajitTeX
            puts("\n\nExecute  'luajittex --credits'  for credits and version details.\n\n"
#else
            puts("\n\nExecute  'luatex --credits'  for credits and version details.\n\n"
#endif
                 "There is NO warranty. Redistribution of this software is covered by\n"
                 "the terms of the GNU General Public License, version 2 or (at your option)\n"
                 "any later version. For more information about these matters, see the file\n"
                 "named COPYING and the LuaTeX source.\n\n" 
#ifdef LuajitTeX
                 "LuaTeX is Copyright 2014 Taco Hoekwater, the LuaTeX Team.\n"
                 "Libraries and JIT extensions by Luigi Scarso, the LuaTeX SwigLib team.\n");
#else
                 "Copyright 2014 Taco Hoekwater, the LuaTeX Team.\n");
#endif
            /* *INDENT-ON* */
            uexit(0);
        } else if (ARGUMENT_IS("credits")) {
            char *versions;
            initversionstring(&versions);
            print_version_banner();
            /* *INDENT-OFF* */
            puts("\n\nThe LuaTeX team is Hans Hagen, Hartmut Henkel, Taco Hoekwater.\n" 
#ifdef LuajitTex
                 "LuajitTeX merges and builds upon (parts of) the code from these projects:\n\n" 
#else
                 "LuaTeX merges and builds upon (parts of) the code from these projects:\n\n" 
#endif
                 "tex       by Donald Knuth\n" 
                 "etex      by Peter Breitenlohner, Phil Taylor and friends\n" 
                 "omega     by John Plaice and Yannis Haralambous\n" 
                 "aleph     by Giuseppe Bilotta\n" 
                 "pdftex    by Han The Thanh and friends\n" 
                 "kpathsea  by Karl Berry, Olaf Weber and others\n" 
                 "lua       by Roberto Ierusalimschy, Waldemar Celes,\n" 
                 "             Luiz Henrique de Figueiredo\n" 
                 "metapost  by John Hobby, Taco Hoekwater and friends.\n" 
                 "poppler   by Derek Noonburg, Kristian H\\ogsberg (partial)\n" 
#ifdef LuajitTeX
                 "fontforge by George Williams (partial)\n" 
                 "luajit    by Mike Pall\n\n" 
#else
                 "fontforge by George Williams (partial)\n\n" 
#endif
                 "Some extensions to lua and additional lua libraries are used, as well as\n" 
                 "libraries for graphic inclusion. More details can be found in the source.\n" 
                 "Code development was sponsored by a grant from Colorado State University\n" 
#ifdef LuajitTeX
                 "via the 'oriental tex' project, the TeX User Groups, and donations.\n"
                 "The additional libraries and the LuaJIT extensions are provided by the LuaTeX SwigLib project.\n");
#else
                 "via the 'oriental tex' project, the TeX User Groups, and donations.\n");
#endif
            /* *INDENT-ON* */
            puts(versions);
            uexit(0);
        }
    }
    /* attempt to find |input_name| / |dump_name| */
    if (lua_only) {
	if (argv[optind]) {
 	   startup_filename = xstrdup(argv[optind]);
           lua_offset = optind;
        }
    } else if (argv[optind] && argv[optind][0] == '&') {
        dump_name = xstrdup(argv[optind] + 1);
    } else if (argv[optind] && argv[optind][0] != '\\') {
        if (argv[optind][0] == '*') {
            input_name = xstrdup(argv[optind] + 1);
        } else {
            firstfile = xstrdup(argv[optind]);
            if ((strstr(firstfile, ".lua") ==
                 firstfile + strlen(firstfile) - 4)
                || (strstr(firstfile, ".luc") ==
                    firstfile + strlen(firstfile) - 4)
                || (strstr(firstfile, ".LUA") ==
                    firstfile + strlen(firstfile) - 4)
                || (strstr(firstfile, ".LUC") ==
                    firstfile + strlen(firstfile) - 4)) {
	        if (startup_filename == NULL) {
                   startup_filename = firstfile;
  	           lua_offset = optind;
                   lua_only = 1;
                   luainit = 1;
                }
            } else {
                input_name = firstfile;
            }
        }
#ifdef WIN32
    } else if (sargv[sargc-1] && sargv[sargc-1][0] != '-' &&
               sargv[sargc-1][0] != '\\') {
        if (sargv[sargc-1][0] == '&')
            dump_name = xstrdup(sargv[sargc-1] + 1);
        else  {
            char *p;
            if (sargv[sargc-1][0] == '*')
                input_name = xstrdup(sargv[sargc-1] + 1);
            else
                input_name = xstrdup(sargv[sargc-1]);
            sargv[sargc-1] = normalize_quotes(input_name, "argument");
            /* Same as
                  input_name = (char *)xbasename(input_name);
               but without cast const => non-const.  */
            input_name += xbasename(input_name) - input_name;
            p = strrchr(input_name, '.');
            if (p != NULL && strcasecmp(p, ".tex") == 0)
                *p = '\0';
            if (!c_job_name)
                c_job_name = normalize_quotes(input_name, "jobname");
        }
        if (safer_option)      /* --safer implies --nosocket */
            nosocket_option = 1;
        return;
#endif
    }
    if (safer_option)           /* --safer implies --nosocket */
        nosocket_option = 1;

    /* Finalize the input filename. */
    if (input_name != NULL) {
        argv[optind] = normalize_quotes(input_name, "argument");
    }
}

@ test for readability 
@c
#define is_readable(a) (stat(a,&finfo)==0) && S_ISREG(finfo.st_mode) &&  \
  (f=fopen(a,"r")) != NULL && !fclose(f)

@ @c
static char *find_filename(char *name, const char *envkey)
{
    struct stat finfo;
    char *dirname = NULL;
    char *filename = NULL;
    FILE *f;
    if (is_readable(name)) {
        return name;
    } else {
        dirname = getenv(envkey);
        if ((dirname != NULL) && strlen(dirname)) {
            dirname = xstrdup(getenv(envkey));
            if (*(dirname + strlen(dirname) - 1) == '/') {
                *(dirname + strlen(dirname) - 1) = 0;
            }
            filename = xmalloc((unsigned) (strlen(dirname) + strlen(name) + 2));
            filename = concat3(dirname, "/", name);
            if (is_readable(filename)) {
                xfree(dirname);
                return filename;
            }
            xfree(filename);
        }
    }
    return NULL;
}


@ @c
static void init_kpse(void)
{

    if (!user_progname) {
        user_progname = dump_name;
    } else if (!dump_name) {
        dump_name = user_progname;
    }
    if (!user_progname) {
        if (ini_version) {
            if (input_name) {
                char *p = input_name + strlen(input_name) - 1;
                while (p >= input_name) {
                    if (IS_DIR_SEP (*p)) {
                        p++;
                        input_name = p;
                        break;
                    }
                    p--;
                }
                user_progname = remove_suffix (input_name);
            }
            if (!user_progname) {
                user_progname = kpse_program_basename(argv[0]);
            }
        } else {
            if (!dump_name) {
                dump_name = kpse_program_basename(argv[0]);
            }
            user_progname = dump_name;
        }
    }
    kpse_set_program_enabled(kpse_fmt_format, MAKE_TEX_FMT_BY_DEFAULT,
                             kpse_src_compile);

    kpse_set_program_name(argv[0], user_progname);
    init_shell_escape();        /* set up 'restrictedshell' */
    program_name_set = 1;
}

@ @c
static void fix_dumpname(void)
{
    int dist;
    if (dump_name) {
        /* adjust array for Pascal and provide extension, if needed */
        dist = (int) (strlen(dump_name) - strlen(DUMP_EXT));
        if (strstr(dump_name, DUMP_EXT) == dump_name + dist)
            TEX_format_default = dump_name;
        else
            TEX_format_default = concat(dump_name, DUMP_EXT);
    } else {
        /* For |dump_name| to be NULL is a bug.  */
        if (!ini_version)
            abort();
    }
}

@ lua require patch

@ Auxiliary function for kpse search

@c
static const char *luatex_kpse_find_aux(lua_State *L, const char *name,
        kpse_file_format_type format, const char *errname)
{
    const char *filename;
    const char *altname;
    altname = luaL_gsub(L, name, ".", "/"); /* Lua convention */
    filename = kpse_find_file(altname, format, false);
    if (filename == NULL) {
        filename = kpse_find_file(name, format, false);
    }
    if (filename == NULL) {
        lua_pushfstring(L, "\n\t[kpse %s searcher] file not found: " LUA_QS,
                        errname, name);
    }
    return filename;
}

@ The lua search function.
 
When kpathsea is not initialized, then it runs the
normal lua function that is saved in the registry, otherwise
it uses kpathsea.

two registry ref variables are needed: one for the actual lua 
function, the other for its environment .

@c
static int lua_loader_function = 0;

static int luatex_kpse_lua_find(lua_State * L)
{
    const char *filename;
    const char *name;
    name = luaL_checkstring(L, 1);
    if (program_name_set == 0) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, lua_loader_function);
	lua_pushvalue(L, -2);
	lua_call(L, 1, 1);
	return 1;
    }
    filename = luatex_kpse_find_aux(L, name, kpse_lua_format, "lua");
    if (filename == NULL)
        return 1;               /* library not found in this path */
    if (luaL_loadfile(L, filename) != 0) {
        luaL_error(L, "error loading module %s from file %s:\n\t%s",
                   lua_tostring(L, 1), filename, lua_tostring(L, -1));
    }
    return 1;                   /* library loaded successfully */
}

@ @c
static int clua_loader_function = 0;
extern int searcher_C_luatex (lua_State *L, const char *name, const char *filename);

static int luatex_kpse_clua_find(lua_State * L)
{
    const char *filename;
    const char *name;
    if (safer_option) {
        lua_pushliteral(L, "\n\t[C searcher disabled in safer mode]");
        return 1;               /* library not found in this path */
    }
    name = luaL_checkstring(L, 1);
    if (program_name_set == 0) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, clua_loader_function);
	lua_pushvalue(L, -2);
	lua_call(L, 1, 1);
	return 1;
    } else {
        const char *path_saved;
        char *prefix, *postfix, *p, *total;
        char *extensionless;
        char *temp_name;
        int j;
        filename = luatex_kpse_find_aux(L, name, kpse_clua_format, "C");
    	if (filename == NULL)
           return 1;               /* library not found in this path */
	extensionless = strdup(filename);
	if (!extensionless) return 1;  /* allocation failure */
	/* Fix Issue 850: replace '.' with LUA_DIRSEP */
        temp_name = strdup(name);
        for(j=0; ; j++){
          if ((unsigned char)temp_name[j]=='\0') {
            break;
          }
          if ((unsigned char)temp_name[j]=='.'){
            temp_name[j]=LUA_DIRSEP[0]; 
          }
        }
	p = strstr(extensionless, temp_name);
	if (!p) return 1;  /* this would be exceedingly weird */
	*p = '\0';
	prefix = strdup(extensionless);
	if (!prefix) return 1;  /* allocation failure */
	postfix = strdup(p+strlen(name));
	if (!postfix) return 1;  /* allocation failure */
	total = malloc(strlen(prefix)+strlen(postfix)+2);
	if (!total) return 1;  /* allocation failure */
	snprintf(total,strlen(prefix)+strlen(postfix)+2, "%s?%s", prefix, postfix);
	/* save package.path */
	lua_getglobal(L,"package");
        lua_getfield(L,-1,"cpath");
	path_saved = lua_tostring(L,-1);
	lua_pop(L,1);
        /* set package.path = "?" */
	lua_pushstring(L,total);
	lua_setfield(L,-2,"cpath");
	lua_pop(L,1); /* pop "package" */
        /* run function */
        lua_rawgeti(L, LUA_REGISTRYINDEX, clua_loader_function);
  	lua_pushstring(L, name);
	lua_call(L, 1, 1);
        /* restore package.path */
	lua_getglobal(L,"package");
	lua_pushstring(L,path_saved);
	lua_setfield(L,-2,"cpath");
	lua_pop(L,1); /* pop "package" */
	free(extensionless);
	free(total);
        free(temp_name);
        return 1;
    }
}

@ Setting up the new search functions. 

This replaces package.searchers[2] and package.searchers[3] with the 
functions defined above.

@c
static void setup_lua_path(lua_State * L)
{
    lua_getglobal(L, "package");
#ifdef LuajitTeX
    lua_getfield(L, -1, "loaders");
#else
    lua_getfield(L, -1, "searchers");
#endif
    lua_rawgeti(L, -1, 2);      /* package.searchers[2] */
    lua_loader_function = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushcfunction(L, luatex_kpse_lua_find);
    lua_rawseti(L, -2, 2);      /* replace the normal lua loader */

    lua_rawgeti(L, -1, 3);      /* package.searchers[3] */
    clua_loader_function = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pushcfunction(L, luatex_kpse_clua_find);
    lua_rawseti(L, -2, 3);      /* replace the normal lua lib loader */

    lua_pop(L, 2);              /* pop the array and table */
}

@ helper variables for the safe keeping of table ids

@c
int tex_table_id;
int pdf_table_id;
int token_table_id;
int node_table_id;


#if defined(WIN32) || defined(__MINGW32__) || defined(__CYGWIN__)
char **suffixlist;

#  define EXE_SUFFIXES ".com;.exe;.bat;.cmd;.vbs;.vbe;.js;.jse;.wsf;.wsh;.ws;.tcl;.py;.pyw"

@ @c
static void mk_suffixlist(void)
{
    char **p;
    char *q, *r, *v;
    int n;

#  if defined(__CYGWIN__)
    v = xstrdup(EXE_SUFFIXES);
#  else
    v = (char *) getenv("PATHEXT");
    if (v)                      /* strlwr() exists also in MingW */
        v = (char *) strlwr(xstrdup(v));
    else
        v = xstrdup(EXE_SUFFIXES);
#  endif

    q = v;
    n = 0;

    while ((r = strchr(q, ';')) != NULL) {
        n++;
        r++;
        q = r;
    }
    if (*q)
        n++;
    suffixlist = (char **) xmalloc((n + 2) * sizeof(char *));
    p = suffixlist;
    *p = xstrdup(".dll");
    p++;
    q = v;
    while ((r = strchr(q, ';')) != NULL) {
        *r = '\0';
        *p = xstrdup(q);
        p++;
        r++;
        q = r;
    }
    if (*q) {
        *p = xstrdup(q);
        p++;
    }
    *p = NULL;
    free(v);
}
#endif

@ @c
void lua_initialize(int ac, char **av)
{

    char *given_file = NULL;
    char *banner;
    int kpse_init;
    static char LC_CTYPE_C[] = "LC_CTYPE=C";
    static char LC_COLLATE_C[] = "LC_COLLATE=C";
    static char LC_NUMERIC_C[] = "LC_NUMERIC=C";
#ifdef LuajitTeX
    static char engine_luatex[] = "engine=luajittex";
#else
    static char engine_luatex[] = "engine=luatex";
#endif
    /* Save to pass along to topenin.  */
    argc = ac;
    argv = av;


    if (luatex_svn < 0) {
#ifdef LuajitTeX
        const char *fmt = "This is LuajitTeX, Version %s" WEB2CVERSION;
#else
        const char *fmt = "This is LuaTeX, Version %s" WEB2CVERSION;
#endif
        size_t len;
        len = strlen(fmt) + strlen(luatex_version_string) ;
	
        banner = xmalloc(len);
        sprintf(banner, fmt, luatex_version_string);
    } else {
#ifdef LuajitTeX
        const char *fmt = "This is LuajitTeX, Version %s" WEB2CVERSION " (rev %d)";
#else
        const char *fmt = "This is LuaTeX, Version %s" WEB2CVERSION " (rev %d)";
#endif
        size_t len;
        len = strlen(fmt) + strlen(luatex_version_string) + 6;
        banner = xmalloc(len);
        sprintf(banner, fmt, luatex_version_string, luatex_svn);
    }
    ptexbanner = banner;

    kpse_invocation_name = kpse_program_basename(argv[0]);

    /* be 'luac' */
    if (argc >1) {
#ifdef LuajitTeX
        if (FILESTRCASEEQ(kpse_invocation_name, "texluajitc"))
            exit(luac_main(ac, av));
        if (STREQ(argv[1], "--luaconly") || STREQ(argv[1], "--luac")) {
            char *argv1 = xmalloc (strlen ("luajittex") + 1);
            av[1] = argv1;
            strcpy (av[1], "luajittex");
            exit(luac_main(--ac, ++av));
        }
#else
        if (FILESTRCASEEQ(kpse_invocation_name, "texluac"))
            exit(luac_main(ac, av));
        if (STREQ(argv[1], "--luaconly") || STREQ(argv[1], "--luac")) {
            strcpy(av[1], "luatex");
            exit(luac_main(--ac, ++av));
        }
#endif
    }
#if defined(WIN32) || defined(__MINGW32__) || defined(__CYGWIN__)
    mk_suffixlist();
#endif

    /* Must be initialized before options are parsed.  */
    interactionoption = 4;
    dump_name = NULL;

    /* 0 means "disable Synchronize TeXnology".
     synctexoption is a *.web variable.
     We initialize it to a weird value to catch the -synctex command line flag
     At runtime, if synctexoption is not |INT_MAX|, then it contains the command line option provided,
     otherwise no such option was given by the user. */
#define SYNCTEX_NO_OPTION INT_MAX
    synctexoption = SYNCTEX_NO_OPTION;

    /* parse commandline */
    parse_options(ac, av);
    if (lua_only)
        shellenabledp = true;

    /* make sure that the locale is 'sane' (for lua) */

    putenv(LC_CTYPE_C);
    putenv(LC_COLLATE_C);
    putenv(LC_NUMERIC_C);

    /* this is sometimes needed */
    putenv(engine_luatex);

    luainterpreter();

    /* init internalized strings */
    init_lua_key(LTL);
    init_lua_key(MathConstants);
    init_lua_key(RTT);
    init_lua_key(TLT);
    init_lua_key(TRT);
    init_lua_key(accent);
    init_lua_key(action);
    init_lua_key(action_id);
    init_lua_key(action_type);
    init_lua_key(additional);
    init_lua_key(adjust_head);
    init_lua_key(advance);
    init_lua_key(aleph);
    init_lua_key(align_head);
    init_lua_key(area);
    init_lua_key(attr);
    init_lua_key(attributes);
    init_lua_key(auto_expand);
    init_lua_key(best_ins_ptr);
    init_lua_key(best_page_break);
    init_lua_key(best_size);
    init_lua_key(bot);
    init_lua_key(bot_accent);
    init_lua_key(bottom_left);
    init_lua_key(bottom_right);
    init_lua_key(box_left);
    init_lua_key(box_left_width);
    init_lua_key(box_right);
    init_lua_key(box_right_width);
    init_lua_key(broken_ins);
    init_lua_key(broken_ptr);
    init_lua_key(cache);
    init_lua_key(cal_expand_ratio);
    init_lua_key(char);
    init_lua_key(characters);
    init_lua_key(checksum);
    init_lua_key(cidinfo);
    init_lua_key(class);
    init_lua_key(command);
    init_lua_key(commands);
    init_lua_key(comment);
    init_lua_key(components);
    init_lua_key(contrib_head);
    init_lua_key(core);
    init_lua_key(cost);
    init_lua_key(count);
    init_lua_key(data);
    init_lua_key(degree);
    init_lua_key(delim);
    init_lua_key(delimptr);
    init_lua_key(denom);
    init_lua_key(depth);
    init_lua_key(designsize);
    init_lua_key(dest_id);
    init_lua_key(dest_type);
    init_lua_key(dir);
    init_lua_key(dir_h);
    init_lua_key(direction);
    init_lua_key(dirs);
    init_lua_key(display);
    init_lua_key(down);
    init_lua_key(dvi_ptr);
    init_lua_key(embedding);
    init_lua_key(encodingbytes);
    init_lua_key(encodingname);
    init_lua_key(end);
    init_lua_key(etex);
    init_lua_key(exactly);
    init_lua_key(expansion_factor);
    init_lua_key(ext);
    init_lua_key(extend);
    init_lua_key(extender);
    init_lua_key(extensible);
    init_lua_key(extra_space);
    init_lua_key(fam);
    init_lua_key(fast);
    init_lua_key(file);
    init_lua_key(filename);
    init_lua_key(font);
    init_lua_key(fonts);
    init_lua_key(format);
    init_lua_key(fullname);
    init_lua_key(global);
    init_lua_key(glue_order);
    init_lua_key(glue_set);
    init_lua_key(glue_sign);
    init_lua_key(glyph);
    init_lua_key(head);
    init_lua_key(height);
    init_lua_key(hold_head);
    init_lua_key(horiz_variants);
    init_lua_key(hyphenchar);
    init_lua_key(id);
    init_lua_key(image);
    init_lua_key(index);
    init_lua_key(italic);
    init_lua_key(kern);
    init_lua_key(kerns);
    init_lua_key(lang);
    init_lua_key(large_char);
    init_lua_key(large_fam);
    init_lua_key(last_ins_ptr);
    init_lua_key(leader);
    init_lua_key(least_page_cost);
    init_lua_key(left);
    init_lua_key(left_boundary);
    init_lua_key(left_protruding);
    init_lua_key(level);
    init_lua_key(ligatures);
    init_lua_key(link_attr);
    init_lua_key(list);
    init_lua_key(log);
    init_lua_key(lua);
    init_lua_key(luatex);
    init_lua_key(mark);
    init_lua_key(mathdir);
    init_lua_key(mathkern);
    init_lua_key(mathstyle);
    init_lua_key(mid);
    init_lua_key(mode);
    init_lua_key(modeline);
    init_lua_key(name);
    init_lua_key(named_id);
    init_lua_key(new_window);
    init_lua_key(next);
    init_lua_key(no);
    init_lua_key(noad);
    init_lua_key(node);
    init_lua_key(nomath);
    init_lua_key(nop);
    init_lua_key(nucleus);
    init_lua_key(num);
    init_lua_key(number);
    init_lua_key(objnum);
    init_lua_key(omega);
    init_lua_key(ordering);
    init_lua_key(page_head);
    init_lua_key(page_ins_head);
    init_lua_key(parameters);
    init_lua_key(pdftex);
    init_lua_key(pen_broken);
    init_lua_key(pen_inter);
    init_lua_key(penalty);
    init_lua_key(pop);
    init_lua_key(post);
    init_lua_key(pre);
    init_lua_key(pre_adjust_head);
    init_lua_key(prev);
    init_lua_key(prevdepth);
    init_lua_key(prevgraf);
    init_lua_key(psname);
    init_lua_key(ptr);
    init_lua_key(push);
    init_lua_key(quad);
    init_lua_key(ref_count);
    init_lua_key(reg);
    init_lua_key(registry);
    init_lua_key(renew);
    init_lua_key(rep);
    init_lua_key(replace);
    init_lua_key(right);
    init_lua_key(right_boundary);
    init_lua_key(right_protruding);
    init_lua_key(rule);
    init_lua_key(scale);
    init_lua_key(script);
    init_lua_key(scriptscript);
    init_lua_key(shift);
    init_lua_key(shrink);
    init_lua_key(shrink_order);
    init_lua_key(size);
    init_lua_key(skewchar);
    init_lua_key(slant);
    init_lua_key(slot);
    init_lua_key(small_char);
    init_lua_key(small_fam);
    init_lua_key(space);
    init_lua_key(space_shrink);
    init_lua_key(space_stretch);
    init_lua_key(spacefactor);
    init_lua_key(spec);
    init_lua_key(special);
    init_lua_key(stack);
    init_lua_key(start);
    init_lua_key(step);
    init_lua_key(stream);
    init_lua_key(stretch);
    init_lua_key(stretch_order);
    init_lua_key(string);
    init_lua_key(style);
    init_lua_key(sub);
    init_lua_key(subst_ex_font);
    init_lua_key(subtype);
    init_lua_key(sup);
    init_lua_key(supplement);
    init_lua_key(surround);
    init_lua_key(tail);
    init_lua_key(temp_head);
    init_lua_key(term);
    init_lua_key(tex);
    init_lua_key(text);
    init_lua_key(thread_attr);
    init_lua_key(thread_id);
    init_lua_key(top);
    init_lua_key(top_accent);
    init_lua_key(top_left);
    init_lua_key(top_right);
    init_lua_key(tounicode);
    init_lua_key(transform);
    init_lua_key(type);
    init_lua_key(uchyph);
    init_lua_key(umath);
    init_lua_key(units_per_em);
    init_lua_key(used);
    init_lua_key(user_id);
    init_lua_key(value);
    init_lua_key(version);
    init_lua_key(vert_variants);
    init_lua_key(width);
    init_lua_key(writable);
    init_lua_key(x_height);
    init_lua_key(xoffset);
    init_lua_key(xyz_zoom);
    init_lua_key(yoffset);
    init_lua_key_alias(luatex_node, "luatex.node");
    init_lua_key_alias(mLTL,"-LTL");
    init_lua_key_alias(mRTT,"-RTT");
    init_lua_key_alias(mTLT,"-TLT");
    init_lua_key_alias(mTRT,"-TRT");
    init_lua_key_alias(node_properties,"node.properties");
    init_lua_key_alias(node_properties_indirect,"node.properties.indirect");
    init_lua_key_alias(pLTL,"+LTL");
    init_lua_key_alias(pRTT,"+RTT");
    init_lua_key_alias(pTLT,"+TLT");
    init_lua_key_alias(pTRT,"+TRT");
    init_lua_key_alias(term_and_log,"term and log");
    init_lua_key_alias(lua_functions,"lua.functions");
    
    init_lua_key(immediate);
    init_lua_key(compresslevel);
    init_lua_key(objcompression);
    init_lua_key(direct);
    init_lua_key(page);
    init_lua_key(streamfile);
    init_lua_key(annot);

    lua_pushstring(Luas,"lua.functions");
    lua_newtable(Luas);
    lua_settable(Luas,LUA_REGISTRYINDEX);

    prepare_cmdline(Luas, argv, argc, lua_offset);      /* collect arguments */
    setup_lua_path(Luas);

    if (startup_filename != NULL) {
        given_file = xstrdup(startup_filename);
        startup_filename = find_filename(startup_filename, "LUATEXDIR");
    }
    /* now run the file */
    if (startup_filename != NULL) {
        char *v1;
        /* hide the 'tex' and 'pdf' table */
        tex_table_id = hide_lua_table(Luas, "tex");
        token_table_id = hide_lua_table(Luas, "token");
        node_table_id = hide_lua_table(Luas, "node");
        pdf_table_id = hide_lua_table(Luas, "pdf");

        if (luaL_loadfile(Luas, startup_filename)) {
            fprintf(stdout, "%s\n", lua_tostring(Luas, -1));
            exit(1);
        }
        /* */
        init_tex_table(Luas);
        if (lua_pcall(Luas, 0, 0, 0)) {
            fprintf(stdout, "%s\n", lua_tostring(Luas, -1));
	    lua_traceback(Luas);
            exit(1);
        }
        /* no filename? quit now! */
        if (!input_name) {
            get_lua_string("texconfig", "jobname", &input_name);
        }
        if (!dump_name) {
            get_lua_string("texconfig", "formatname", &dump_name);
        }
        if (lua_only) {
            if (given_file)
                free(given_file);
            /* this is not strictly needed but it pleases valgrind */
            lua_close(Luas);
            exit(0);
        }
        /* unhide the 'tex' and 'pdf' table */
        unhide_lua_table(Luas, "tex", tex_table_id);
        unhide_lua_table(Luas, "pdf", pdf_table_id);
        unhide_lua_table(Luas, "token", token_table_id);
        unhide_lua_table(Luas, "node", node_table_id);

        /* |kpse_init| */
        kpse_init = -1;
        get_lua_boolean("texconfig", "kpse_init", &kpse_init);

        if (kpse_init != 0) {
            luainit = 0;        /* re-enable loading of texmf.cnf values, see luatex.ch */
            init_kpse();
        }
        /* |prohibit_file_trace| (boolean) */
        tracefilenames = 1;
        get_lua_boolean("texconfig", "trace_file_names", &tracefilenames);

        /* |file_line_error| */
        filelineerrorstylep = false;
        get_lua_boolean("texconfig", "file_line_error", &filelineerrorstylep);

        /* |halt_on_error| */
        haltonerrorp = false;
        get_lua_boolean("texconfig", "halt_on_error", &haltonerrorp);

        /* |restrictedshell| */
        v1 = NULL;
        get_lua_string("texconfig", "shell_escape", &v1);
        if (v1) {
            if (*v1 == 't' || *v1 == 'y' || *v1 == '1') {
                shellenabledp = 1;
            } else if (*v1 == 'p') {
                shellenabledp = 1;
                restrictedshell = 1;
            }
	    free(v1);
        }
        /* If shell escapes are restricted, get allowed cmds from cnf.  */
        if (shellenabledp && restrictedshell == 1) {
            v1 = NULL;
            get_lua_string("texconfig", "shell_escape_commands", &v1);
            if (v1) {
                mk_shellcmdlist(v1);
		free(v1);
            }
        }

        fix_dumpname();

    } else {
        if (luainit) {
            if (given_file) {
                fprintf(stdout, "%s file %s not found\n",
                        (lua_only ? "Script" : "Configuration"), given_file);
                free(given_file);
            } else {
                fprintf(stdout, "No %s file given\n",
                        (lua_only ? "script" : "configuration"));
            }
            exit(1);
        } else {
            /* init */
            init_kpse();
            fix_dumpname();
        }
    }
}

@ @c
void check_texconfig_init(void)
{
    if (Luas != NULL) {
        lua_getglobal(Luas, "texconfig");
        if (lua_istable(Luas, -1)) {
            lua_getfield(Luas, -1, "init");
            if (lua_isfunction(Luas, -1)) {
                int i = lua_pcall(Luas, 0, 0, 0);
                if (i != 0) {
                    /* Can't be more precise here, called before TeX initialization  */
                    fprintf(stderr, "This went wrong: %s\n",
                            lua_tostring(Luas, -1));
                    error();
                }
            }
        }
    }
}

@ @c
void write_svnversion(char *v)
{
    char *a_head, *n;
    char *a = xstrdup(v);
    size_t l = strlen("$Id: luatex.web ");
    if (a != NULL) {
        a_head = a;
        if (strlen(a) > l)
            a += l;
        n = a;
        while (*n != '\0' && *n != ' ')
            n++;
        *n = '\0';
        fprintf(stdout, " luatex.web >= v%s", a);
        free(a_head);
    }
}
