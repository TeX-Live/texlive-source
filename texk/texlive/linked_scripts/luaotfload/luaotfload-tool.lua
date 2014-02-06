#!/usr/bin/env texlua
-----------------------------------------------------------------------
--         FILE:  luaotfload-tool.lua
--  DESCRIPTION:  database functionality
-- REQUIREMENTS:  luaotfload 2.4
--       AUTHOR:  Khaled Hosny, Élie Roux, Philipp Gesang
--      VERSION:  2.4-3
--      LICENSE:  GPL v2
--     MODIFIED:  2013-07-28 13:12:04+0200
-----------------------------------------------------------------------

local version = "2.4-3" --- <int: major>.<int: minor><alpha: fixes>

--[[doc--

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
    we’re running an outdated Luatex.  If so, we hand over control to
    the legacy db runner.

    \url{http://lua-users.org/wiki/LuaVersionCompatibility}

--doc]]--


local ioopen          = io.open
local iowrite         = io.write
local kpsefind_file   = kpse.find_file
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
        local oldscript = kpsefind_file "luaotfload-legacy-tool.lua"
        return require (oldscript)
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

require(loader_path)

--[[doc--
Depending on how the script is called we change its behavior.
For backwards compatibility, moving or symlinking the script to a
file name starting with \fileent{mkluatexfontdb} will cause it to
trigger a database update on every run.
Running as \fileent{luaotfload-tool} -- the new name -- will do this upon
request only.

There are two naming conventions followed here: firstly that of
utilities such as \fileent{mktexpk}, \fileent{mktexlsr} and the likes,
and secondly that of \fileent{fmtutil}.
After support for querying the database was added, the latter appeared
to be the more appropriate.
--doc]]--

config                        = config or { }
local config                  = config
local luaotfloadconfig        = config.luaotfload or { }
config.luaotfload             = luaotfloadconfig
luaotfloadconfig.version      = luaotfloadconfig.version   or version
luaotfloadconfig.names_dir    = luaotfloadconfig.names_dir or "names"
luaotfloadconfig.cache_dir    = luaotfloadconfig.cache_dir or "fonts"
luaotfloadconfig.index_file   = luaotfloadconfig.index_file
                             or "luaotfload-names.lua"
luaotfloadconfig.formats      = luaotfloadconfig.formats
                             or "otf,ttf,ttc,dfont"
luaotfloadconfig.reload       = false
if not luaotfloadconfig.strip then
    luaotfloadconfig.strip = true
end

do -- we don’t have file.basename and the likes yet, so inline parser ftw
    local slash        = P"/"
    local dot          = P"."
    local noslash      = 1 - slash
    local slashes      = slash^1
    local path         =  slashes^-1 * (noslash^1 * slashes)^1
    local thename      = (1 - slash - dot)^1
    local extension    = dot * (1 - slash - dot)^1
    local p_basename   = path^-1 * C(thename) * extension^-1 * P(-1)

    local self = lpegmatch(p_basename, stringlower(arg[0]))
    if self == "luaotfload-tool" then
        luaotfloadconfig.self = "luaotfload-tool"
    else
        luaotfloadconfig.self = "mkluatexfontdb"
    end
end

config.lualibs                  = config.lualibs or { }
config.lualibs.verbose          = false
config.lualibs.prefer_merged    = true
config.lualibs.load_extended    = true

require "lualibs"
local tabletohash               = table.tohash
local stringsplit               = string.split

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

require"luaotfload-override.lua"  --- this populates the logs.* namespace
require"luaotfload-database"
require"alt_getopt"

local names = fonts.names

local status_file                    = "luaotfload-status"
local luaotfloadstatus               = require (status_file)
luaotfloadconfig.status              = luaotfloadstatus

local sanitize_fontname              = names.sanitize_fontname

local pathdata      = names.path
local names_plain   = pathdata.index.lua
local names_gzip    = names_plain .. ".gz"
local names_bin     = pathdata.index.luc

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

  --alias=<name>               force behavior of "luaotfload-tool" or legacy
                               "mkluatexfontdb"

-------------------------------------------------------------------------------
                                   DATABASE

  -u --update                  update the database
  -n --no-reload               suppress db update
  --no-strip                   keep redundant information in db
  -f --force                   force re-indexing all fonts
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

Usage: %s [OPTION]...

Rebuild or update the Luaotfload font names database.

Valid options:
  -f --force                   force re-indexing all fonts
  -q --quiet                   don't output anything
  -v --verbose=LEVEL           be more verbose (print the searched directories)
  -vv                          print the loaded fonts
  -vvv                         print all steps of directory searching
  -V --version                 print version and exit
  -h --help                    print this message
  --alias=<name>               force behavior of "luaotfload-tool" or legacy
                               "mkluatexfontdb"

The font database will be saved to
   %s
   %s

]],
    short = [[
Usage: luaotfload-tool [--help] [--version] [--verbose=<lvl>]
                       [--update] [--force] [--prefer-texmf]
                       [--dry-run] [--formats=<extension list>]
                       [--find=<font name>] [--fuzzy] [--info] [--inspect]
                       [--list=<criterion>] [--fields=<field list>]
                       [--cache=<directive>] [--flush-lookups]
                       [--show-blacklist] [--diagnose=<procedure>]

Enter 'luaotfload-tool --help' for a larger list of options.
]]
}

local help_msg = function (version)
    local template = help_messages[version]
    iowrite(stringformat(template,
                         luaotfloadconfig.self,
--                         names_plain,
                         names_gzip,
                         names_bin,
                         caches.getwritablepath (
                         luaotfloadconfig.cache_dir)))
end

local about = [[
%s:
        Luaotfload font management and diagnostic utility.
        License: GNU GPL v2.0.
        Report problems to <https://github.com/lualatex/luaotfload/issues>
]]

local version_msg = function ( )
    local out = function (...) texiowrite_nl (stringformat (...)) end
    out (about, luaotfloadconfig.self)
    out ("%s version %q", luaotfloadconfig.self, version)
    out ("revision %q", luaotfloadstatus.notes.revision)
    out ("database version %q", names.version)
    out ("Lua interpreter: %s; version %q", runtime[1], runtime[2])
    out ("Luatex SVN revision %d", status.luatex_svn)
    out ("Luatex version %.2f.%d",
         status.luatex_version / 100,
         status.luatex_revision)
    out ""
end


--- makeshift formatting

local head_adornchars = {
    [1] = "*", [2] = "=", [3] = "~", [4] = "-", [5] = "·",
}

local textwidth         = 80
local wd_leftcolumn     = math.floor(textwidth * .25)
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

local show_info_items = function (fontinfo)
    local items = table.sortedkeys(fontinfo)
    print_heading(fontinfo.fullname, 1)
    texiowrite_nl ""
    for n = 1, #items do
        local item = items[n]
        texiowrite_nl(stringformat(
            info_fmt, item, fontinfo[item]))
    end
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
                logs.names_report(true, 1, "resolve",
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
                logs.names_report(true, 1, "resolve",
                    [[%s is a font collection]], basename)
                for subfont = 1, nfonts do
                    logs.names_report(true, 1, "resolve",
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
        logs.names_report(true, 1, "resolve",
            "Font %s not found", filename)
    end
end

--[[--
Running the scripts triggers one or more actions that have to be
executed in the correct order. To avoid duplication we track them in a
set.
--]]--

local action_sequence = {
    "loglevel",  "help",  "version", "diagnose",
    "blacklist", "cache", "flush",   "generate",
    "list",      "query",
}

local action_pending  = tabletohash(action_sequence, false)

action_pending.loglevel = true  --- always set the loglevel
action_pending.generate = false --- this is the default action

local actions = { } --- (jobspec -> (bool * bool)) list

actions.loglevel = function (job)
    logs.set_loglevel(job.log_level)
    logs.names_report("info", 3, "util",
                      "Setting log level", "%d", job.log_level)
    logs.names_report("log", 2, "util", "Lua=%s", _VERSION)
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
    for n, entry in next, table.sortedkeys(names.blacklist) do
        iowrite (stringformat("(%d %s)\n", n, entry))
    end
    return true, false
end

actions.generate = function (job)
    local fontnames, savedname
    fontnames = names.update(fontnames, job.force_reload, job.dry_run)
    logs.names_report("info", 2, "db",
        "Fonts in the database: %i", #fontnames.mappings)
    if names.data() then
        return true, true
    end
    return false, false
end

actions.flush = function (job)
    local success, lookups = names.flush_lookup_cache()
    if success then
        local success = names.save_lookups()
        if success then
            logs.names_report("info", 2, "cache", "Lookup cache emptied")
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
        logs.names_report("info", 2, "cache",
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
            foundname, _, success = names.crude_file_lookup (foundname)
        end
    elseif tmpspec.lookup == "file" then
        foundname, _, success =
            names.crude_file_lookup (tmpspec.name)
    end

    if success then
        logs.names_report(false, 0,
            "resolve", "Font %q found!", query)
        if subfont then
            logs.names_report(false, 0, "resolve",
                "Resolved file name %q, subfont nr. %q",
                foundname, subfont)
        else
            logs.names_report(false, 0, "resolve",
                              "Resolved file name %q", foundname)
        end
        if job.show_info then
            show_font_info (foundname, query, job.full_info, job.warnings)
            iowrite "\n"
        end
    else
        logs.names_report(false, 0,
            "resolve", "Cannot find %q in index.", query)
        logs.names_report(false, 0,
            "resolve", "Hint: use the --fuzzy option to display suggestions.", query)
        if job.fuzzy == true then
            logs.names_report(false, 0,
                "resolve", "Looking for close matches, this may take a while ...")
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

local splitcomma = names.patterns.splitcomma

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
        logs.names_report(false, 1, "list", "All %d entries", nmappings)
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

        logs.names_report(false, 1, "list", "By %s", criterion)

        --- firstly, build a list of fonts to operate on
        local targets = { }
        if asked_value then --- only those whose value matches
            logs.names_report(false, 2, "list", "Restricting to value %s", asked_value)
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
        logs.names_report(false, 2, "list", "%d entries", ntargets)

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
mkluatexfontdb.lua relies on the script alt_getopt to process argv and
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
        log_level    = 0, --- 2 is approx. the old behavior
    }

    local long_options = {
        alias              = 1,
        cache              = 1,
        ["no-compress"]    = "c",
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
        log                = 1,
        ["max-fonts"]      = 1,
        ["no-reload"]      = "n",
        ["no-strip"]       = 0,
        ["skip-read"]      = "R",
        ["prefer-texmf"]   = "p",
        quiet              = "q",
        ["show-blacklist"] = "b",
        stats              = "S",
        update             = "u",
        verbose            = 1,
        version            = "V",
        warnings           = "w",
    }

    local short_options = "bcDfFiIlnpqRSuvVhw"

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
            if result.log_level > 0 then
                result.log_level = result.log_level + 1
            else
                result.log_level = 1
            end
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
                finalizers = logs.set_logout(str, finalizers)
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
        elseif v == "alias" then
            luaotfloadconfig.self = optarg[n]
        elseif v == "l" then
            action_pending["flush"] = true
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
            names.set_font_filter (optarg[n])
        elseif v == "n" then
            luaotfloadconfig.update_live = false
        elseif v == "S" then
            luaotfloadconfig.statistics = true
        elseif v == "R" then
            ---  dev only, undocumented
            luaotfloadconfig.skip_read = true
        elseif v == "c" then
            luaotfloadconfig.compress = false
        elseif v == "no-strip" then
            luaotfloadconfig.strip = false
        elseif v == "max-fonts" then
            local n = optarg[n]
            if n then
                n = tonumber(n)
                if n and n > 0 then
                    luaotfloadconfig.max_fonts = n
                end
            end
        end
    end

    if luaotfloadconfig.self == "mkluatexfontdb" then --- TODO drop legacy ballast after 2.4
        result.help_version = "mkluatexfontdb"
        action_pending["generate"] = true
        result.log_level = math.max(1, result.log_level)
        logs.set_logout("stdout", finalizers)
    elseif nopts == 0 then
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
            logs.names_report("log", 3, "util", "Preparing for task",
                              "%s", actionname)

            local action             = actions[actionname]
            local success, continue  = action(job)

            if not success then
                logs.names_report(false, 0, "util",
                    "Could not finish task", "%s", actionname)
                retval = -1
                exit   = true
            elseif not continue then
                logs.names_report(false, 3, "util",
                    "Task completed, exiting", "%s", actionname)
                exit   = true
            else
                logs.names_report(false, 3, "util",
                    "Task completed successfully", "%s", actionname)
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
