#!/usr/bin/env texlua
-----------------------------------------------------------------------
--         FILE:  luaotfload-tool.lua
--  DESCRIPTION:  database functionality
-- REQUIREMENTS:  luaotfload 2.5
--       AUTHOR:  Khaled Hosny, Élie Roux, Philipp Gesang
--      VERSION:  2.5
--      LICENSE:  GPL v2.0
--     MODIFIED:  2014-08-03 10:14:04+0200
-----------------------------------------------------------------------

luaotfload          = luaotfload or { }
local version       = "2.5-3" --- <int: major>.<int: minor>-<int: fixes>
luaotfload.version  = version
luaotfload.self     = "luaotfload-tool"

--[[doc--

luaotfload-tool(1)

This file was originally written (as \fileent{mkluatexfontdb.lua}) by
Elie Roux and Khaled Hosny and, as a derived work of ConTeXt, is
provided under the terms of the GPL v2.0 license as printed in full
text in the manual (luaotfload.pdf).

    \url{http://www.gnu.org/licenses/old-licenses/gpl-2.0.html}.

This file is a wrapper for the luaotfload font names module
(luaotfload-database.lua). It is part of the luaotfload bundle, please
see the luaotfload documentation for more info. Report bugs to

    \url{https://github.com/lualatex/luaotfload/issues}.

--doc]]--

kpse.set_program_name "luatex"

--[[doc--

    We test for Lua 5.1 by means of capability detection to see if
    we’re running an outdated Luatex.  If so, we bail.

    \url{http://lua-users.org/wiki/LuaVersionCompatibility}

--doc]]--


local iowrite         = io.write
local kpsefind_file   = kpse.find_file
local mathfloor       = math.floor
local next            = next
local osdate          = os.date
local ostype          = os.type
local stringexplode   = string.explode
local stringformat    = string.format
local stringlower     = string.lower
local stringrep       = string.rep
local tableconcat     = table.concat
local texiowrite_nl   = texio.write_nl
local texiowrite      = texio.write
local tonumber        = tonumber
local type            = type

local runtime
if _G.getfenv ~= nil then -- 5.1 or LJ
    if _G.jit ~= nil then
        runtime = { "jit", jit.version }
    else
        runtime = { "stock", _VERSION }
        print "FATAL ERROR"
        print "Luaotfload requires a Luatex version >=0.76."
        print "Please update your TeX distribution!"
        os.exit (-1)
    end
else -- 5.2
    runtime = { "stock", _VERSION }
end


local C, Ct, P, S  = lpeg.C, lpeg.Ct, lpeg.P, lpeg.S
local lpegmatch    = lpeg.match

local loader_file = "luatexbase.loader.lua"
local loader_path = assert(kpsefind_file(loader_file, "lua"),
                           "File '"..loader_file.."' not found")


string.quoted = string.quoted or function (str)
  return string.format("%q",str) 
end

require (loader_path)

--[[doc--

    XXX:
        Creating the config table will be moved to the common
        initialization when the times comes.

--doc]]--

config                          = config or { }
local config                    = config
config.luaotfload               = config.luaotfload or { }

config.lualibs                  = config.lualibs or { }
config.lualibs.verbose          = false
config.lualibs.prefer_merged    = true
config.lualibs.load_extended    = true

require "lualibs"
local iosavedata                = io.savedata
local lfsisdir                  = lfs.isdir
local lfsisfile                 = lfs.isfile
local stringsplit               = string.split
local tableserialize            = table.serialize
local tablesortedkeys           = table.sortedkeys
local tabletohash               = table.tohash

--[[doc--
\fileent{luatex-basics-gen.lua} calls functions from the
\luafunction{texio.*} library; too much for our taste.
We intercept them with dummies.

Also, it sets up dummies in place of the tables created by the Context
libraries. Since we have loaded the lualibs already this would cause
collateral damage for some libraries whose namespace would be
overridden. We employ our usual backup-restore strategy to work around
this. (Postponing the loading of the lualibs code is not an option
because the functionality is needed by basics-gen itself.)
--doc]]--

local dummy_function = function ( ) end
local backup = {
    write     = texio.write,
    write_nl  = texio.write_nl,
    utilities = utilities,
}

texio.write, texio.write_nl          = dummy_function, dummy_function
require"luaotfload-basics-gen.lua"

texio.write, texio.write_nl          = backup.write, backup.write_nl
utilities                            = backup.utilities

require "luaotfload-log.lua"       --- this populates the luaotfload.log.* namespace
require "luaotfload-parsers"       --- fonts.conf, configuration, and request syntax
require "luaotfload-configuration" --- configuration file handling
require "luaotfload-database"
require "alt_getopt"

local names                          = fonts.names
local sanitize_fontname              = names.sanitize_fontname

local log                            = luaotfload.log
local report                         = log.report

local help_messages = {
    ["luaotfload-tool"] = [[

Usage: %s [OPTIONS...]

    Luaotfload font management and diagnostic utility.
    This program is part of the Luaotfload package.

    Valid options are:

-------------------------------------------------------------------------------
                           VERBOSITY AND DIAGNOSTICS

  -q --quiet                   don't output anything
  -v --verbose=LEVEL           be more verbose (print the searched directories)
  -vv                          print the loaded fonts
  -vvv                         print all steps of directory searching
  --log=stdout                 redirect log output to stdout

  -V --version                 print version and exit
  -h --help                    print this message
  --diagnose=CHECK             run a self test procedure; one of "files",
                               "environment", "index", "permissions", or
                               "repository"

-------------------------------------------------------------------------------
                                   DATABASE

  -u --update                  update the database
  -n --no-reload               suppress db update
  --no-strip                   keep redundant information in db
  -f --force                   force re-indexing all fonts
  -L --local                   scan font files in $PWD
  -c --no-compress             do not gzip index file (text version only)
  -l --flush-lookups           empty lookup cache of font requests
  -D --dry-run                 skip loading of fonts, just scan
  --formats=[+|-]EXTENSIONS    set, add, or subtract formats to index
  -p --prefer-texmf            prefer fonts in the TEXMF over system fonts
  --max-fonts=N                process at most N font files

  --find="font name"           query the database for a font name
  -F --fuzzy                   look for approximate matches if --find fails
  --limit=n                    limit display of fuzzy matches to <n>
                               (default: n = 1)

  -i --info                    display basic font metadata
  -I --inspect                 display detailed font metadata
  -w --warnings                display warnings generated by the
                               fontloader library

  --list=<criterion>           output list of entries by field <criterion>
  --list=<criterion>:<value>   restrict to entries with <criterion>=<value>
  --fields=<f1>,<f2>,…,<fn>    which fields <f> to print with --list
  -b --show-blacklist          show blacklisted files

  --bisect=<directive>         control database bisection: valid
                               directives are "start", "stop", "run", "status",
                               "good", "bad"

The font database will be saved to
   %s
   %s

-------------------------------------------------------------------------------
                                   FONT CACHE

  --cache=<directive>          operate on font cache, where <directive> is
                               "show", "purge", or "erase"

The font cache will be written to
   %s

]],
    mkluatexfontdb = [[
FATAL ERROR
As of Luaotfload v2.5, legacy behavior is not supported anymore. Please
update your scripts and/or habits! Kthxbye.
]],
    short = [[
Usage: luaotfload-tool [--help] [--version] [--verbose=<lvl>]
                       [--update] [--force] [--prefer-texmf] [--local]
                       [--dry-run] [--formats=<extension list>]
                       [--find=<font name>] [--fuzzy] [--info] [--inspect]
                       [--list=<criterion>] [--fields=<field list>]
                       [--cache=<directive>] [--flush-lookups]
                       [--show-blacklist] [--diagnose=<procedure>]
                       [--no-compress] [--no-strip] [--local]
                       [--max-fonts=<n>] [--bisect=<directive>]

Enter 'luaotfload-tool --help' for a larger list of options.
]]
}

local help_msg = function (version)
    local template      = help_messages[version]
    local paths         = config.luaotfload.paths
    local names_plain   = paths.index_path_lua
    local names_gzip    = names_plain .. ".gz"
    local names_bin     = paths.index_path_luc

    iowrite(stringformat(template,
                         luaotfload.self,
                         names_gzip,
                         names_bin,
                         caches.getwritablepath (config.luaotfload.cache_dir)))
end

local about = [[
%s:
        Luaotfload font management and diagnostic utility.
        License: GNU GPL v2.0.
        Report problems to <https://github.com/lualatex/luaotfload/issues>
]]

local version_msg = function ( )
    local out   = function (...) texiowrite_nl (stringformat (...)) end
    local uname = os.uname ()
    local meta  = names.getmetadata ()
    out (about, luaotfload.self)
    out ("%s version: %q", luaotfload.self, version)
    out ("Revision: %q", config.luaotfload.status.notes.revision)
    out ("Lua interpreter: %s; version %q", runtime[1], runtime[2])
    out ("Luatex SVN revision: %d", status.luatex_svn)
    out ("Luatex version: %.2f.%d",
         status.luatex_version / 100,
         status.luatex_revision)
    out ("Platform: type=%s name=%s", os.type, os.name)

    local uname_vars = tablesortedkeys (uname)
    for i = 1, #uname_vars do
        local var = uname_vars[i]
        out ("    + %8s: %s", var, uname[var])
    end
    out ("Index: version=%q created=%q modified=%q",
         config.luaotfload.status.notes.revision,
         meta.created or "ages ago",
         meta.modified or "ages ago")
    out ""
end


--- makeshift formatting

local head_adornchars = {
    [1] = "*", [2] = "=", [3] = "~", [4] = "-", [5] = "·",
}

local textwidth         = 80
local wd_leftcolumn     = mathfloor(textwidth * .25)
local key_fmt           = stringformat([[%%%ds]], wd_leftcolumn)
local val_fmt           = [[%s]]
local fieldseparator    = ":"
local info_fmt          = key_fmt .. fieldseparator .. " " .. val_fmt

local currentdepth      = 0
local counterstack      = { } -- counters per level
local counterformat     = "%d"

local format_counter = function (stack)
    local acc = { }
    for lvl=1, #stack do
        acc[#acc+1] = stringformat(counterformat, stack[lvl])
    end
    return tableconcat(acc, ".")
end

local print_heading = function (title, level)
    local structuredata
    if currentdepth == level then -- top is current
        counterstack[#counterstack] = counterstack[#counterstack] + 1
    elseif currentdepth < level then -- push new
        counterstack[#counterstack+1] = 1
    else -- pop
        local diff = currentdepth - level
        while diff > 0 do
            counterstack[#counterstack] = nil
            diff = diff - 1
        end
        counterstack[#counterstack] = counterstack[#counterstack] + 1
    end
    currentdepth = level

    texiowrite_nl ""
    if not level or level > #head_adornchars then
        level = #head_adornchars
    end
    local adornchar = head_adornchars[level]

    local counter = format_counter(counterstack)

    local s = adornchar .. adornchar .. " "
           .. counter .. " "
           .. title .. " "
    texiowrite_nl (s .. stringrep(adornchar, textwidth-utf.len(s)))
end

local baseindent = "    "

--[[doc--

    show_info_items -- Together with show_info_table prints the table returned by
    fontloader.info(), recursing into nested tables if appropriate (as necessitated
    by Luatex versions 0.78+ which include the pfminfo table in the result.

--doc]]--

local show_info_table show_info_table = function (t, depth)
    depth           = depth or 0
    local indent    = stringrep (baseindent, depth)
    local keys      = tablesortedkeys (t)
    for n = 1, #keys do
        local key = keys [n]
        local val = t [key]
        if type (val) == "table" then
            texiowrite_nl (indent .. stringformat (info_fmt, key, "<table>"))
            show_info_table (val, depth + 1)
        else
            texiowrite_nl (indent .. stringformat (info_fmt, key, val))
        end
    end
end

local show_info_items = function (fontinfo)
    print_heading (fontinfo.fullname, 1)
    texiowrite_nl ""
    show_info_table (fontinfo)
    texiowrite_nl ""
end

local p_eol     = S"\n\r"^1
local p_space   = S" \t\v"^0
local p_line    = p_space * C((1 - p_eol)^1)^-1
local p_lines   = Ct(p_line * (p_eol^1 * p_line^-1)^0)

local show_fontloader_warnings = function (ws)
    local nws = #ws
    print_heading(stringformat(
        [[the fontloader emitted %d warnings]],
        nws), 2)
    texiowrite_nl ""
    for i=1, nws do
        local w = ws[i]
        texiowrite_nl (stringformat("%d:", i))
        local lines = lpegmatch(p_lines, w)
        for i=1, #lines do
            local line = lines[i]
            texiowrite_nl("  · " .. line)
        end
        texiowrite_nl ""
    end
end

local p_spacechar  = S" \n\r\t\v"
local p_wordchar   = (1 - p_spacechar)
local p_whitespace = p_spacechar^1
local p_word       = C(p_wordchar^1)
local p_words      = Ct(p_word * (p_whitespace * p_word)^0)

--- string -> int -> string list
local reflow = function (text, width)
    local words
    if type(text) == "string" then
        words = lpegmatch(p_words, text)
        if #words < 2 then
            return { text }
        end
    else
        words = text
        if #words < 2 then
            return words
        end
    end

    local space     = " "
    local utflen    = utf.len
    local reflowed  = { }

    local first     = words[1]
    local linelen   = #first
    local line      = { first }

    for i=2, #words do
        local word  = words[i]
        local lword = utflen(word)
        linelen = linelen + lword + 1
        if linelen > width then
            reflowed[#reflowed+1] = tableconcat(line)
            linelen = #word
            line = { word }
        else
            line[#line+1] = space
            line[#line+1] = word
        end
    end
    reflowed[#reflowed+1] = tableconcat(line)
    return reflowed
end

--- string -> 'a -> string list
local print_field = function (key, val)
    val = tostring(val)
    local lhs    = stringformat(key_fmt, key) .. fieldseparator .. " "
    local wd_lhs = #lhs
    local lines  = reflow(val, textwidth - wd_lhs)

    texiowrite_nl(lhs)
    texiowrite(lines[1])
    if #lines > 1 then
        local indent = stringrep(" ", wd_lhs)
        for i=2, #lines do
            texiowrite_nl(indent)
            texiowrite   (lines[i])
        end
    end
end

local display_names = function (names)
    print_heading("Font Metadata", 2)
    for i=1, #names do
        local lang, namedata = names[i].lang, names[i].names
        print_heading(stringformat("Language: %s ", i, lang), 3)
        texiowrite_nl ""
        if namedata then
            for field, value in next, namedata do
                print_field(field, value)
            end
        end
    end
end

--- see luafflib.c
local general_fields = {
    --- second: l -> literal | n -> length | d -> date
    { "fullname",            "l", "font name"           },
    { "version",             "l", "font version"        },
    { "creationtime",        "d", "creation time"       },
    { "modificationtime",    "d", "modification time"   },
    { "subfonts",            "n", "number of subfonts"  },
    { "glyphcnt",            "l", "number of glyphs"    },
    { "weight",              "l", "weight indicator"    },
    { "design_size",         "l", "design size"         },
    { "design_range_bottom", "l", "design size min"     },
    { "design_range_top",    "l", "design size max"     },
    { "fontstyle_id",        "l", "font style id"       },
    { "fontstyle_name",      "S", "font style name"     },
    { "strokewidth",         "l", "stroke width"        },
    { "units_per_em",        "l", "units per em"        },
    { "ascent",              "l", "ascender height"     },
    { "descent",             "l", "descender height"    },
    { "comments",            "l", "comments"            },
    { "os2_version",         "l", "os2 version"         },
    { "sfd_version",         "l", "sfd version"         },
}

local display_general = function (fullinfo)
    texiowrite_nl ""
    print_heading("General Information", 2)
    texiowrite_nl ""
    for i=1, #general_fields do
        local field = general_fields[i]
        local key, mode, desc  = unpack(field)
        local val
        if mode == "l" then
            val = fullinfo[key]
        elseif mode == "S" then --- style names table
            local data = fullinfo[key]
            if type (data) == "table" then
                if #data > 0 then
                    for n = 1, #data do
                        local nth = data[n]
                        if nth.lang == 1033 then
                            val = nth.name
                            goto found
                        end
                    end
                    val = next (data).name
                else
                    val = ""
                end
                ::found::
            else
                val = data
            end
        elseif mode == "n" then
            local v = fullinfo[key]
            if v then
                val = #fullinfo[key]
            end
        elseif mode == "d" then
            if ostype == "unix" then
                val = osdate("%F %T", fullinfo[key])
            else
                --- the MS compiler doesn’t support C99, so
                --- strftime is missing some functionality;
                --- see loslib.c for details.
                val = osdate("%Y-%m-d %H:%M:%S", fullinfo[key])
            end
        end
        if not val then
            val = "<none>"
        end
        print_field(desc, val)
    end
end

local print_features = function (features)
    for tag, data in next, features do
        print_heading(tag, 4)
        for script, languages in next, data do
            local field     = stringformat(key_fmt, script).. fieldseparator .. " "
            local wd_field  = #field
            local lines     = reflow(languages.list, textwidth - wd_field)
            local indent    = stringrep(" ", wd_field)
            texiowrite_nl(field)
            texiowrite(lines[1])
            if #lines > 1 then
                for i=1, #lines do
                    texiowrite_nl(indent .. lines[i])
                end
            end
        end
    end
end

local extract_feature_info = function (set)
    local collected = { }
    for i=1, #set do
        local features = set[i].features
        if features then
            for j=1, #features do
                local feature   = features[j]
                local scripts   = feature.scripts
                local tagname   = stringlower(feature.tag)
                local entry     = collected[tagname] or { }

                for k=1, #scripts do
                    local script     = scripts[k]
                    local scriptname = stringlower(script.script)
                    local c_script   = entry[scriptname] or {
                                            list = { },
                                            set  = { },
                                        }
                    local list, set  = c_script.list, c_script.set

                    for l=1, #script.langs do
                        local langname = stringlower(script.langs[l])
                        if not set[langname] then
                            list[#list+1] = langname
                            set[langname] = true
                        end
                    end
                    entry[scriptname] = c_script
                end
                collected[tagname]  = entry
            end
        end
    end
    return collected
end

local display_feature_set = function (set)
    local collected = extract_feature_info(set)
    print_features(collected)
end

local display_features = function (gsub, gpos)
    texiowrite_nl ""

    if gsub or gpos then
        print_heading("Features", 2)

        if gsub then
            print_heading("GSUB Features", 3)
            display_feature_set(gsub)
        end

        if gpos then
            print_heading("GPOS Features", 3)
            display_feature_set(gpos)
        end
    end
end

local show_full_info = function (path, subfont, warnings)
    local rawinfo, warn = fontloader.open(path, subfont)
    if warnings then
        show_fontloader_warnings(warn)
    end
    if not rawinfo then
        texiowrite_nl(stringformat([[cannot open font %s]], path))
        return
    end
    local fontdata = { }
    local fullinfo = fontloader.to_table(rawinfo)
    local fields = fontloader.fields(rawinfo)
    fontloader.close(rawinfo)
    display_names(fullinfo.names)
    display_general(fullinfo)
    display_features(fullinfo.gsub, fullinfo.gpos)
end

--- Subfonts returned by fontloader.info() do not correspond
--- to the actual indices required by fontloader.open(), so
--- we try and locate the correct one by matching the request
--- against the full name.

local subfont_by_name
subfont_by_name = function (lst, askedname, n)
    if not n then
        return subfont_by_name (lst, askedname, 1)
    end

    local font = lst[n]
    if font then
        if sanitize_fontname (font.fullname) == askedname then
            return font
        end
        return subfont_by_name (lst, askedname, n+1)
    end
    return false
end

--[[doc--
The font info knows two levels of detail:

    a)  basic information returned by fontloader.info(); and
    b)  detailed information that is a subset of the font table
        returned by fontloader.open().
--doc]]--

local show_font_info = function (basename, askedname, detail, warnings)
    local filenames = names.data().files
    local index     = filenames.base[basename]
    local fullname  = filenames.full[index]
    askedname = sanitize_fontname (askedname)
    if not fullname then -- texmf
        fullname = resolvers.findfile(basename)
    end
    if fullname then
        local shortinfo = fontloader.info(fullname)
        local nfonts   = #shortinfo
        if nfonts > 0 then -- true type collection
            local subfont
            if askedname then
                report (true, 1, "resolve",
                        [[%s is part of the font collection %s]],
                        askedname, basename)
                subfont = subfont_by_name(shortinfo, askedname)
            end
            if subfont then
                show_info_items(subfont)
                if detail == true then
                    show_full_info(fullname, subfont, warnings)
                end
            else -- list all subfonts
                report (true, 1, "resolve",
                        [[%s is a font collection]], basename)
                for subfont = 1, nfonts do
                    report (true, 1, "resolve",
                            [[Showing info for font no. %d]], n)
                    show_info_items(shortinfo[subfont])
                    if detail == true then
                        show_full_info(fullname, subfont, warnings)
                    end
                end
            end
        else
            show_info_items(shortinfo)
            if detail == true then
                show_full_info(fullname, subfont, warnings)
            end
        end
    else
        report (true, 1, "resolve", "Font %s not found", filename)
    end
end

--[[--
Running the scripts triggers one or more actions that have to be
executed in the correct order. To avoid duplication we track them in a
set.
--]]--

local action_sequence = {
    "config",   "loglevel",  "help",  "version",
    "diagnose", "blacklist", "cache", "flush",
    "bisect",   "generate",  "list",  "query",
}

local action_pending  = tabletohash(action_sequence, false)

action_pending.config   = true  --- always read the configuration
action_pending.loglevel = true  --- always set the loglevel
action_pending.generate = false --- this is the default action

local actions = { } --- (jobspec -> (bool * bool)) list

actions.loglevel = function (job)
    local lvl = job.log_level
    if lvl then
        log.set_loglevel(lvl)
        report ("info", 3, "util", "Setting the log level to %d.", lvl)
        report ("log", 2, "util", "Lua=%q", _VERSION)
    end
    return true, true
end

actions.config = function (job)
    local defaults            = luaotfload.default_config
    local vars                = config.actions.read (job.extra_config)
    config.luaotfload         = config.actions.apply (defaults, vars)
    config.luaotfload         = config.actions.apply (config.luaotfload, job.config)

    --inspect(config.luaotfload)
    --os.exit()
    if not config.actions.reconfigure () then
        return false, false
    end
    return true, true
end

actions.version = function (job)
    version_msg()
    return true, false
end

actions.help = function (job)
    help_msg (job.help_version or "luaotfload-tool")
    return true, false
end

actions.blacklist = function (job)
    names.read_blacklist()
    local n = 0
    for n, entry in next, tablesortedkeys(names.blacklist) do
        iowrite (stringformat("(%d %s)\n", n, entry))
    end
    return true, false
end

actions.generate = function (job)
    local _ = names.update (fontnames, job.force_reload, job.dry_run)
    local namedata = names.data ()
    if namedata then
        report ("info", 2, "db", "Fonts in the database: %i", #namedata.mappings)
        return true, true
    end
    return false, false
end

-------------------------------------------------------------------------------
--- bisect mode
-------------------------------------------------------------------------------

local bisect_status_path = caches.getwritablepath "bisect"
local bisect_status_file = bisect_status_path .."/" .. "luaotfload-bisect-status.lua"
local bisect_status_fmt  = [[
--[==[-------------------------------------------------------------------------
    This file is generated by Luaotfload. It can be safely deleted.
    Creation date: %s.
-------------------------------------------------------------------------]==]--

%s

--- vim:ft=lua:ts=8:et:sw=2
]]

--[[doc--

    write_bisect_status -- Write the history of the current bisection to disk.

--doc]]--

--- state list -> bool
local write_bisect_status = function (data)
    local payload = tableserialize (data, true)
    local status  = stringformat (bisect_status_fmt,
                                  osdate ("%Y-%m-d %H:%M:%S", os.time ()),
                                  payload)
    if status and iosavedata (bisect_status_file, status) then
        report ("info", 4, "bisect",
                "Bisection state written to %s.", bisect_status_file)
        return true
    end
    report ("info", 0, "bisect",
            "Failed to write bisection state to %s.", bisect_status_file)
    return false
end

--[[doc--

    read_bisect_status -- Read the bisect log from disk.

--doc]]--

--- unit -> state list
local read_bisect_status = function ()
    report ("info", 4, "bisect", "Testing for status file: %q.", bisect_status_file)
    if not lfsisfile (bisect_status_file) then
        report ("info", 2, "bisect", "No such file: %q.", bisect_status_file)
        report ("info", 0, "bisect", "Not in bisect mode.")
        return false
    end
    report ("info", 4, "bisect", "Reading status file: %q.", bisect_status_file)
    local success, status = pcall (dofile, bisect_status_file)
    if not success then
        report ("info", 0, "bisect", "Could not read status file.")
        return false
    end
    return status
end

--[[doc--

    bisect_start -- Begin a bisect session. Determines the number of
    fonts and sets the initial high, low, and pivot values.

--doc]]--

local bisect_start = function ()
    if lfsisfile (bisect_status_file) then
        report ("info", 0, "bisect",
                "Bisect session in progress.",
                bisect_status_file)
        report ("info", 0, "bisect",
                "Use --bisect=stop to erase it before starting over.")
        return false, false
    end
    report ("info", 2, "bisect",
            "Starting bisection of font database %q.", bisect_status_file)
    local n     = names.count_font_files ()
    local pivot = mathfloor (n / 2)
    local data  = { { 1, n, pivot } }
    report ("info", 0, "bisect", "Initializing pivot to %d.", pivot)
    if write_bisect_status (data) then
        return true, false
    end
    return false, false
end

--[[doc--

    bisect_stop -- Terminate bisection session by removing all state info.

--doc]]--

local bisect_stop = function ()
    report ("info", 3, "bisect", "Erasing bisection state at %s.", bisect_status_file)
    if lfsisfile (bisect_status_file) then
        local success, msg = os.remove (bisect_status_file)
        if not success then
            report ("info", 2, "bisect",
                    "Failed to erase file %s (%s).",
                     bisect_status_file, msg)
        end
    end
    if lfsisdir (bisect_status_path) then
        local success, msg = os.remove (bisect_status_path)
        if not success then
            report ("info", 2, "bisect",
                    "Failed to erase directory %s (%s).",
                     bisect_status_path, msg)
        end
    end
    if lfsisfile (bisect_status_file) then
        return false, false
    end
    return true, false
end

--[[doc--

    bisect_terminate -- Wrap up a bisect session by printing the
    offending font and removing the state file.

--doc]]--

local bisect_terminate = function (nsteps, culprit)
    report ("info", 1, "bisect",
            "Bisection completed after %d steps.", nsteps)
    report ("info", 0, "bisect",
            "Bad file: %s.", names.nth_font_filename (culprit))
    report ("info", 0, "bisect",
            "Run with --bisect=stop to finish bisection.")
    return true, false
end

--[[doc--

    list_remainder -- Show remaining fonts in bisect slice.

--doc]]--

local list_remainder = function (lo, hi)
    local fonts = names.font_slice (lo, hi)
    report ("info", 0, "bisect", "%d fonts left.", hi - lo + 1)
    for i = 1, #fonts do
        report ("info", 1, "bisect", "   · %2d: %s", lo, fonts[i])
        lo = lo + 1
    end
end

--[[doc--

    bisect_set -- Prepare the next bisection step by setting high, low,
    and pivot to new values.

    The “run” directive always picks the segment below the pivot so we
    can rely on the “outcome parameter” to be referring to that.

--doc]]--

local bisect_set = function (outcome)
    local status = read_bisect_status ()
    if not status then
        return false, false
    end

    local nsteps        = #status
    local previous      = status[nsteps]
    if previous == true then
        --- Bisection already completed; we exit early through
        --- bisect_terminate() to avoid further writes to the
        --- state files that mess up step counting.
        nsteps = nsteps - 1
        return bisect_terminate (nsteps, status[nsteps][1])
    end

    local lo, hi, pivot = unpack (previous)

    report ("info", 3, "bisect", "Previous step %d: lo=%d, hi=%d, pivot=%d.",
            nsteps, lo, hi, pivot)

    if outcome == "bad" then
        hi = pivot
        if lo >= hi then --- complete
            status[nsteps + 1] = { lo, lo, lo }
            status[nsteps + 2] = true
            write_bisect_status (status)
            return bisect_terminate (nsteps, lo)
        end
        pivot = mathfloor ((lo + hi) / 2)
        report ("info", 0, "bisect",
                "Continuing with the lower segment: lo=%d, hi=%d, pivot=%d.",
                lo, hi, pivot)
    elseif outcome == "good" then
        lo = pivot + 1
        if lo >= hi then --- complete
            status[nsteps + 1] = { lo, lo, lo }
            write_bisect_status (status)
            status[nsteps + 2] = true
            return bisect_terminate (nsteps, lo)
        end
        pivot = mathfloor ((lo + hi) / 2)
        report ("info", 0, "bisect",
                "Continuing with the upper segment: lo=%d, hi=%d, pivot=%d.",
                lo, hi, pivot)
    else -- can’t happen
        report ("info", 0, "bisect", "What the hell?", lo, hi, pivot)
        return false, false
    end

    status[nsteps + 1] = { lo, hi, pivot }
    write_bisect_status (status)
    if hi - lo <= 10 then
        list_remainder (lo, hi)
    end
    return true, false
end

--[[doc--

    bisect_status -- Output information about the current bisect session.

--doc]]--

local bisect_status = function ()
    local status = read_bisect_status ()
    if not status then
        return false, false
    end
    local nsteps = #status
    if nsteps > 1 then
        for i = nsteps - 1, 1, -1 do
            local step = status[i]
            report ("info", 2, "bisect", "Step %d: lo=%d, hi=%d, pivot=%d.",
                    i, unpack (step))
        end
    end
    local current = status[nsteps]
    report ("info", 0, "bisect", "Step %d: lo=%d, hi=%d, pivot=%d.",
            nsteps, unpack (current))
    return true, false
end

--[[doc--

    bisect_run -- Run Luaotfload utilizing the current bisection state.
    This should be combined with the --update mode, possibly with the
    --force option.

    Luaotfload always tests the segment below the pivot first.

--doc]]--

local bisect_run = function ()
    local status = read_bisect_status ()
    if not status then
        return false, false
    end
    local nsteps        = #status
    local currentstep   = nsteps + 1
    local current       = status[nsteps]
    if current == true then -- final step
        current = status[nsteps - 1]
    end
    local lo, hi, pivot = unpack (current)
    report ("info", 3, "bisect", "Previous step %d: lo=%d, hi=%d, pivot=%d.",
            nsteps, lo, hi, pivot)
    report ("info", 1, "bisect", "Step %d: Testing fonts from %d to %d.",
            currentstep, lo, pivot)
    config.luaotfload.misc.bisect = { lo, pivot }
    return true, true
end

local bisect_modes = {
    start   = bisect_start,
    good    = function () return bisect_set "good" end,
    bad     = function () return bisect_set "bad"  end,
    stop    = bisect_stop,
    status  = bisect_status,
    run     = bisect_run,
}

actions.bisect = function (job)
    local mode   = job.bisect
    local runner = bisect_modes[mode]
    if not runner then
        report ("info", 0, "bisect", "Unknown directive %q.", mode)
        return false, false
    end
    return runner (job)
end

actions.flush = function (job)
    local success = names.flush_lookup_cache()
    if success then
        local success = names.save_lookups()
        if success then
            report ("info", 2, "cache", "Lookup cache emptied")
            return true, true
        end
    end
    return false, false
end

local cache_directives = {
    ["purge"] = names.purge_cache,
    ["erase"] = names.erase_cache,
    ["show"]  = names.show_cache,
}

actions.cache = function (job)
    local directive = cache_directives[job.cache]
    if not directive or type(directive) ~= "function" then
        report ("info", 2, "cache",
                "Invalid font cache directive %s.", job.cache)
        return false, false
    end
    if directive() then
        return true, true
    end
    return false, false
end

actions.query = function (job)

    require "luaotfload-features"

    local query = job.query

    local tmpspec = {
        name          = query,
        lookup        = "name",
        specification = query,
        optsize       = 0,
        features      = { },
    }

    tmpspec = names.handle_request (tmpspec)

    if not tmpspec.size then
        tmpspec.size = 655360 --- assume 10pt
    end

    local foundname, subfont, success

    if tmpspec.lookup == "name"
    or tmpspec.lookup == "anon" --- not *exactly* as resolvers.anon
    then
        foundname, subfont = names.resolve_name (tmpspec)
        if foundname then
            foundname, _, success = names.font_file_lookup (foundname)
        end
    elseif tmpspec.lookup == "file" then
        foundname, _, success =
            names.font_file_lookup (tmpspec.name)
    end

    if success then
        report (false, 0, "resolve", "Font %q found!", query)
        if subfont then
            report (false, 0, "resolve",
                    "Resolved file name %q, subfont nr. %q",
                foundname, subfont)
        else
            report (false, 0, "resolve",
                    "Resolved file name %q", foundname)
        end
        if job.show_info then
            show_font_info (foundname, query, job.full_info, job.warnings)
            iowrite "\n"
        end
    else
        report (false, 0, "resolve", "Cannot find %q in index.", query)
        report (false, 0, "resolve",
                "Hint: use the --fuzzy option to display suggestions.",
                query)
        if job.fuzzy == true then
            report (false, 0, "resolve",
                    "Looking for close matches, this may take a while ...")
            local _success = names.find_closest(query, job.fuzzy_limit)
        end
    end
    return true, true
end

---         --list=<criterion>
---         --list=<criterion>:<value>
---
---         --list=<criterion>          --fields=<f1>,<f2>,<f3>,...<fn>

local get_fields get_fields = function (entry, fields, acc, n)
    if not acc then
        return get_fields (entry, fields, { }, 1)
    end

    local field = fields [n]
    if field then
        local chain = stringsplit (field, "->")
        local tmp   = entry
        for i = 1, #chain - 1 do
            tmp = tmp [chain [i]]
            if not tmp then
                --- invalid field
                break
            end
        end
        if tmp then
            local value = tmp [chain [#chain]]
            acc[#acc+1] = value or false
        else
            acc[#acc+1] = false
        end
        return get_fields (entry, fields, acc, n+1)
    end
    return acc
end

local separator = "\t" --- could be “,” for csv

local format_fields format_fields = function (fields, acc, n)
    if not acc then
        return format_fields(fields, { }, 1)
    end

    local field = fields[n]
    if field ~= nil then
        if field == false then
            acc[#acc+1] = "<none>"
        else
            acc[#acc+1] = tostring(field)
        end
        return format_fields(fields, acc, n+1)
    end
    return tableconcat(acc, separator)
end

local set_primary_field
set_primary_field = function (fields, addme, acc, n)
    if not acc then
        return set_primary_field(fields, addme, { addme }, 1)
    end

    local field = fields[n]
    if field then
        if field ~= addme then
            acc[#acc+1] = field
        end
        return set_primary_field(fields, addme, acc, n+1)
    end
    return acc
end

local splitcomma = luaotfload.parsers.splitcomma

actions.list = function (job)
    local criterion     = job.criterion
    local asked_fields  = job.asked_fields
    local name_index    = names.data ()

    if asked_fields then
        asked_fields = lpegmatch(splitcomma, asked_fields)
    end

    if not asked_fields then
        --- some defaults
        asked_fields = { "plainname", "version", }
    end

    if not name_index then
        name_index = names.load()
    end

    local mappings  = name_index.mappings
    local nmappings = #mappings

    if criterion == "*" then
        report (false, 1, "list", "All %d entries", nmappings)
        for i=1, nmappings do
            local entry     = mappings[i]
            local fields    = get_fields(entry, asked_fields)
            --- we could collect these instead ...
            local formatted = format_fields(fields)
            texiowrite_nl(formatted)
        end

    else
        criterion = stringexplode(criterion, ":") --> { field, value }
        local asked_value  = criterion[2]
        criterion          = criterion[1]
        asked_fields       = set_primary_field(asked_fields, criterion)

        report (false, 1, "list", "By %s", criterion)

        --- firstly, build a list of fonts to operate on
        local targets = { }
        if asked_value then --- only those whose value matches
            report (false, 2, "list", "Restricting to value %s", asked_value)
            for i=1, nmappings do
                local entry = mappings[i]
                if  entry[criterion]
                and tostring(entry[criterion]) == asked_value
                then
                    targets[#targets+1] = entry
                end
            end

        else --- whichever have the field, sorted
            local categories, by_category = { }, { }
            for i=1, nmappings do
                local entry = mappings[i]
                local tmp   = entry
                local chain = stringsplit (criterion, "->")
                for i = 1, #chain - 1 do
                    tmp = tmp [chain [i]]
                    if not tmp then
                        break
                    end
                end
                local value = tmp and tmp [chain [#chain]] or "<none>"
                if value then
                    --value = tostring(value)
                    local entries = by_category[value]
                    if not entries then
                        entries = { entry }
                        categories[#categories+1] = value
                    else
                        entries[#entries+1] = entry
                    end
                    by_category[value] = entries
                end
            end
            table.sort(categories)

            for i=1, #categories do
                local entries = by_category[categories[i]]
                for j=1, #entries do
                    targets[#targets+1] = entries[j]
                end
            end
        end
        local ntargets = #targets
        report (false, 2, "list", "%d entries", ntargets)

        --- now, output the collection
        for i=1, ntargets do
            local entry         = targets[i]
            local fields        = get_fields(entry, asked_fields)
            local formatted     = format_fields(fields)
            texiowrite_nl(formatted)
        end
    end

    texiowrite_nl ""

    return true, true
end

actions.diagnose = function (job)
    --- diagnostics are loaded on demand
    local diagnose = require "luaotfload-diagnostics.lua"
    return diagnose (job)
end

--- stuff to be carried out prior to exit

local finalizers = { }

--- returns false if at least one of the actions failed, mainly
--- for closing io channels
local finalize = function ()
    local success = true
    for _, fun in next, finalizers do
        if type (fun) == "function" then
            if fun () == false then success = false end
        end
    end
    return success
end

--[[--
Command-line processing.
luaotfload-tool relies on the script alt_getopt to process argv and
analyzes its output.

TODO with extended lualibs we have the functionality from the
environment.* namespace that could eliminate the dependency on
alt_getopt.
--]]--

local process_cmdline = function ( ) -- unit -> jobspec
    local result = { -- jobspec
        force_reload = nil,
        full_info    = false,
        warnings     = false,
        criterion    = "",
        query        = "",
        log_level    = nil,
        bisect       = nil,
        config       = { db = { }, misc = { }, run = { live = false }, paths = { } },
    }

    local long_options = {
        ["bisect"]         = 1,
        cache              = 1,
        conf               = 1,
        diagnose           = 1,
        ["dry-run"]        = "D",
        ["flush-lookups"]  = "l",
        fields             = 1,
        find               = 1,
        force              = "f",
        formats            = 1,
        fuzzy              = "F",
        help               = "h",
        info               = "i",
        inspect            = "I",
        limit              = 1,
        list               = 1,
        ["local"]          = "L",
        log                = 1,
        ["max-fonts"]      = 1,
        ["no-compress"]    = "c",
        ["no-reload"]      = "n",
        ["no-strip"]       = 0,
        ["skip-read"]      = "R",
        ["prefer-texmf"]   = "p",
        ["print-conf"]     = 0,
        quiet              = "q",
        ["show-blacklist"] = "b",
        stats              = "S",
        update             = "u",
        verbose            = 1,
        version            = "V",
        warnings           = "w",
    }

    local short_options = "bcDfFiIlLnpqRSuvVhw"

    local options, _, optarg =
        alt_getopt.get_ordered_opts (arg, short_options, long_options)

    local nopts = #options
    for n=1, nopts do
        local v = options[n]
        if     v == "q" then
            result.log_level = 0
        elseif v == "u" then
            action_pending["generate"] = true
        elseif v == "v" then
            local lvl = result.log_level
            if not lvl or lvl < 1 then
                lvl = 1
            else
                lvl = lvl + 1
            end
            result.log_level = lvl
        elseif v == "V" then
            action_pending["version"] = true
        elseif v == "h" then
            action_pending["help"] = true
        elseif v == "f" then
            result.update       = true
            result.force_reload = 1
        elseif v == "verbose" then
            local lvl = optarg[n]
            if lvl then
                lvl = tonumber(lvl)
                result.log_level = lvl
                if lvl > 2 then
                    result.warnings = true
                end
            end
        elseif v == "w" then
            result.warnings = true
        elseif v == "log" then
            local str = optarg[n]
            if str then
                finalizers = log.set_logout(str, finalizers)
            end
        elseif v == "find" then
            action_pending["query"] = true
            result.query = optarg[n]
        elseif v == "F" then
            result.fuzzy = true
        elseif v == "limit" then
            local lim = optarg[n]
            if lim then
                result.fuzzy_limit = tonumber(lim)
            end
        elseif v == "i" then
            result.show_info = true
        elseif v == "I" then
            result.show_info = true
            result.full_info = true
        elseif v == "l" then
            action_pending["flush"] = true
        elseif v == "L" then
            action_pending["generate"] = true
            result.config.db.scan_local = true
        elseif v == "list" then
            action_pending["list"] = true
            result.criterion = optarg[n]
        elseif v == "fields" then
            result.asked_fields = optarg[n]
        elseif v == "cache" then
            action_pending["cache"] = true
            result.cache = optarg[n]
        elseif v == "D" then
            result.dry_run = true
        elseif v == "p" then
            names.set_location_precedence {
                "local", "texmf", "system"
            }
        elseif v == "b" then
            action_pending["blacklist"] = true
        elseif v == "diagnose" then
            action_pending["diagnose"] = true
            result.asked_diagnostics = optarg[n]
        elseif v == "formats" then
            result.config.db.formats = optarg[n]
            --names.set_font_filter (optarg[n])
        elseif v == "n" then
            result.config.db.update_live = false
        elseif v == "S" then
            result.config.misc.statistics = true
        elseif v == "R" then
            ---  dev only, undocumented
            result.config.db.skip_read = true
        elseif v == "c" then
            result.config.db.compress = false
        elseif v == "no-strip" then
            result.config.db.strip = false
        elseif v == "max-fonts" then
            local n = optarg[n]
            if n then
                n = tonumber(n)
                if n and n > 0 then
                    result.config.db.max_fonts = n
                end
            end
        elseif v == "bisect" then
            result.bisect         = optarg[n]
            action_pending.bisect = true
        elseif v == "conf" then
            local extra = stringexplode (optarg[n], ",+")
            if extra then
                local extra_config = result.extra_config
                if extra_config then
                    table.append (extra_config, extra)
                else
                    result.extra_config = extra
                end
            end
        elseif v == "print-conf" then
            result.print_config = true
        end
    end

    if nopts == 0 then
        action_pending["help"] = true
        result.help_version = "short"
    end
    return result
end

local main = function ( ) -- unit -> int
    local retval    = 0
    local job       = process_cmdline()

--    inspect(action_pending)
--    inspect(job)

    for i=1, #action_sequence do
        local actionname = action_sequence[i]
        local exit       = false
        if action_pending[actionname] then
            report ("log", 3, "util", "Preparing for task", "%s", actionname)

            local action             = actions[actionname]
            local success, continue  = action(job)

            if not success then
                report (false, 0, "util",
                        "Failed to execute task.", "%s", actionname)
                retval = -1
                exit   = true
            elseif not continue then
                report (false, 3, "util",
                        "Task completed, exiting.", "%s", actionname)
                exit = true
            else
                report (false, 3, "util",
                        "Task completed successfully.", "%s", actionname)
            end
        end
        if exit then break end
    end

    if finalize () == false then
        retval = -1
    end

    --texiowrite_nl""
    return retval
end

return main()

-- vim:tw=71:sw=4:ts=4:expandtab
