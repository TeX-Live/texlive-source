#!/usr/bin/env texlua
-----------------------------------------------------------------------
--         FILE:  luaotfload-tool.lua
--  DESCRIPTION:  database functionality
-- REQUIREMENTS:  luaotfload 2.2
--       AUTHOR:  Khaled Hosny, Élie Roux, Philipp Gesang
--      VERSION:  2.3b
--      LICENSE:  GPL v2
--     MODIFIED:  2013-06-02 19:23:54+0200
-----------------------------------------------------------------------

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
local lfsattributes   = lfs.attributes
local lfsisfile       = lfs.isfile
local lfsreadlink     = lfs.readlink
local md5sumhexa      = md5.sumhexa
local next            = next
local osdate          = os.date
local osremove        = os.remove
local ostype          = os.type
local stringexplode   = string.explode
local stringformat    = string.format
local stringlower     = string.lower
local stringrep       = string.rep
local stringsub       = string.sub
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


local C, Cg, Ct, P, S  = lpeg.C, lpeg.Cg, lpeg.Ct, lpeg.P, lpeg.S
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
config.luaotfload             = config.luaotfload or { }
config.luaotfload.names_dir   = config.luaotfload.names_dir or "names"
config.luaotfload.cache_dir   = config.luaotfload.cache_dir or "fonts"
config.luaotfload.index_file  = config.luaotfload.index_file
                             or "luaotfload-names.lua"

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
        config.luaotfload.self = "luaotfload-tool"
    else
        config.luaotfload.self = "mkluatexfontdb"
    end
end

config.lualibs                  = config.lualibs or { }
config.lualibs.verbose          = false
config.lualibs.prefer_merged    = true
config.lualibs.load_extended    = true

require "lualibs"
--- dofile "util-jsn.lua" --- awaiting fix

local lua_of_json               = utilities.json.tolua
local ioloaddata                = io.loaddata
local tabletohash               = table.tohash
local fileiswritable            = file.iswritable
local fileisreadable            = file.isreadable

--[[doc--
\fileent{luatex-basics-gen.lua} calls functions from the
\luafunction{texio.*} library; too much for our taste.
We intercept them with dummies.
--doc]]--

local dummy_function = function ( ) end
local backup_write, backup_write_nl  = texio.write, texio.write_nl

texio.write, texio.write_nl          = dummy_function, dummy_function
require"luaotfload-basics-gen.lua"
texio.write, texio.write_nl          = backup_write, backup_write_nl

require"luaotfload-override.lua"  --- this populates the logs.* namespace
require"luaotfload-database"
require"alt_getopt"

local names = fonts.names

local status_file = "luaotfload-status"
local status      = require (status_file)

local version  = "2.3b"

local sanitize_string = names.sanitize_string

local pathdata      = names.path
local names_plain   = pathdata.index.lua
local names_bin     = pathdata.index.luc

local help_messages = {
    ["luaotfload-tool"] = [[

Usage: %s [OPTIONS...]

Operations on the LuaTeX font database.

This tool is part of the luaotfload package. Valid options are:

-------------------------------------------------------------------------------
                             VERBOSITY AND LOGGING

  -q --quiet                   don't output anything
  -v --verbose=LEVEL           be more verbose (print the searched directories)
  -vv                          print the loaded fonts
  -vvv                         print all steps of directory searching
  --log=stdout                 redirect log output to stdout

  -V --version                 print version and exit
  -h --help                    print this message
  --diagnose=CHECK             run a self test procedure; one of "files",
                               "permissions", or "repository"

  --alias=<name>               force behavior of "luaotfload-tool" or legacy
                               "mkluatexfontdb"

-------------------------------------------------------------------------------
                                   DATABASE

  -u --update                  update the database
  -f --force                   force re-indexing all fonts
  -l --flush-lookups           empty lookup cache of font requests
  -D --dry-run                 skip loading of fonts, just scan
  -p --prefer-texmf            prefer fonts in the TEXMF over system fonts

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

Rebuild or update the LuaTeX font database.

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
                         config.luaotfload.self,
                         names_plain,
                         names_bin,
                         caches.getwritablepath (
                         config.luaotfload.cache_dir)))
end

local version_msg = function ( )
    texiowrite_nl(stringformat(
        "%s version %q\n" .. -- no \z due to 5.1 compatibility
        "revision %q\n" ..
        "database version %q\n" ..
        "Lua interpreter: %s; version %q\n",
        config.luaotfload.self,
        version,
        status.notes.revision,
        names.version,
        runtime[1],
        runtime[2]))
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
    { "fontstyle_name",      "l", "font style name"     },
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
        elseif mode == "n" then
            local v = fullinfo[key]
            if v then
                val = #fullinfo[key]
            end
        elseif mode == "d" then
            val = osdate("%F %T", fullinfo[key])
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
            --inspect(languages.list)
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
    print_heading("Features", 2)
    print_heading("GSUB Features", 3)
    display_feature_set(gsub)
    print_heading("GPOS Features", 3)
    display_feature_set(gpos)
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
        if sanitize_string(font.fullname) == askedname then
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
    local filenames = names.data.filenames
    local index     = filenames.base[basename]
    local fullname  = filenames.full[index]
    askedname = sanitize_string(askedname)
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
    for n, entry in next, table.sortedkeys(fonts.names.blacklist) do
        texiowrite_nl(stringformat("(%d %s)", n, entry))
    end
    return true, false
end

actions.generate = function (job)
    local fontnames, savedname
    fontnames = names.update(fontnames, job.force_reload, job.dry_run)
    logs.names_report("info", 2, "db",
        "Fonts in the database: %i", #fontnames.mappings)
    local success = names.save(fontnames)
    if success then
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

    local query = job.query
    local tmpspec = {
        name          = query,
        lookup        = "name",
        specification = "name:" .. query,
        optsize       = 0,
    }

    local foundname, subfont, success =
        fonts.names.resolve(nil, nil, tmpspec)

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
            show_font_info(foundname, query, job.full_info, job.warnings)
        end
    else
        logs.names_report(false, 0,
            "resolve", "Cannot find %q.", query)
        if job.fuzzy == true then
            logs.names_report(false, 0,
                "resolve", "Looking for close matches, this may take a while ...")
            local success = fonts.names.find_closest(query, job.fuzzy_limit)
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
        return get_fields(entry, fields, { }, 1)
    end

    local field = fields[n]
    if field then
        local value = entry[field]
        acc[#acc+1] = value or false
        return get_fields(entry, fields, acc, n+1)
    end
    return acc
end

local comma       = P","
local noncomma    = 1-comma
local split_comma = Ct((C(noncomma^1) + comma)^1)

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

actions.list = function (job)
    local criterion     = job.criterion

    local asked_fields  = job.asked_fields
    if asked_fields then
        asked_fields = lpegmatch(split_comma, asked_fields)
    else
        --- some defaults
        asked_fields = { "fullname", "version", }
    end

    if not names.data then
        names.data = names.load()
    end

    local mappings  = names.data.mappings
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
                local value = entry[criterion]
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

    return true, true
end

do
    local out = function (...)
        logs.names_report (false, 0, "diagnose", ...)
    end

    local verify_files = function (errcnt, status)
        out "================ verify files ================="
        local hashes = status.hashes
        local notes  = status.notes
        if not hashes or #hashes == 0 then
            out ("FAILED: cannot read checksums from %s.", status_file)
            return 1/0
        elseif not notes then
            out ("FAILED: cannot read commit metadata from %s.",
                 status_file)
            return 1/0
        end

        out ("Luaotfload revision %s.", notes.revision)
        out ("Committed by %s.",        notes.committer)
        out ("Timestamp %s.",           notes.timestamp)

        local nhashes = #hashes
        out ("Testing %d files for integrity.", nhashes)
        for i = 1, nhashes do
            local fname, canonicalsum = unpack (hashes[i])
            local location = kpsefind_file (fname)
                          or kpsefind_file (fname, "texmfscripts")
            if not location then
                errcnt = errcnt + 1
                out ("FAILED: file %s missing.", fname)
            else
                out ("File: %s.", location)
                local raw = ioloaddata (location)
                if not raw then
                    errcnt = errcnt + 1
                    out ("FAILED: file %d not readable.", fname)
                else
                    local sum = md5sumhexa (raw)
                    if sum ~= canonicalsum then
                        errcnt = errcnt + 1
                        out ("FAILED: checksum mismatch for file %s.",
                             fname)
                        out ("Expected %s.", canonicalsum)
                        out ("Got      %s.", sum)
                    else
                        out ("Ok, %s passed.", fname)
                    end
                end
            end
        end
        return errcnt
    end

    local get_tentative_attributes = function (file)
        if not lfsisfile (file) then
            local chan = ioopen (file, "w")
            if chan then
                chan:close ()
                local attributes = lfsattributes (file)
                os.remove (file)
                return attributes
            end
        end
    end

    local p_permissions = Ct(Cg(Ct(C(1) * C(1) * C(1)), "u")
                           * Cg(Ct(C(1) * C(1) * C(1)), "g")
                           * Cg(Ct(C(1) * C(1) * C(1)), "o"))

    local analyze_permissions = function (raw)
        return lpegmatch (p_permissions, raw)
    end

    local trailingslashes   = P"/"^1 * P(-1)
    local stripslashes      = C((1 - trailingslashes)^0)

    local get_permissions = function (t, location)
        if stringsub (location, #location) == "/" then
            --- strip trailing slashes (lfs idiosyncrasy on Win)
            location = lpegmatch (stripslashes, location)
        end
        local attributes = lfsattributes (location)

        if not attributes and t == "f" then
            attributes = get_tentative_attributes (location)
            if not attributes then
                return false
            end
        end

        local permissions

        if fileisreadable (location) then
            --- link handling appears to be unnecessary because
            --- lfs.attributes() will return the information on
            --- the link target.
            if mode == "link" then --follow and repeat
                location = lfsreadlink (location)
                attributes = lfsattributes (location)
            end
        end

        permissions = analyze_permissions (attributes.permissions)

        return {
            location    = location,
            mode        = attributes.mode,
            owner       = attributes.uid, --- useless on windows
            permissions = permissions,
            attributes  = attributes,
        }
    end

    local check_conformance = function (spec, permissions, errcnt)
        local uid = permissions.attributes.uid
        local gid = permissions.attributes.gid
        local raw = permissions.attributes.permissions

        out ("Owner: %d, group %d, permissions %s.", uid, gid, raw)
        if ostype == "unix" then
            if uid == 0 or gid == 0 then
                out "Owned by the superuser, permission conflict likely."
                errcnt = errcnt + 1
            end
        end

        local user = permissions.permissions.u
        if spec.r == true then
            if user[1] == "r" then
                out "Readable: ok."
            else
                out "Not readable: permissions need fixing."
                errcnt = errcnt + 1
            end
        end

        if spec.w == true then
            if user[2] == "w"
            or  fileiswritable (permissions.location) then
                out "Writable: ok."
            else
                out "Not writable: permissions need fixing."
                errcnt = errcnt + 1
            end
        end

        return errcnt
    end

    local path = names.path

    local desired_permissions = {
        { "d", {"r","w"}, function () return caches.getwritablepath () end },
        { "d", {"r","w"}, path.globals.prefix },
        { "f", {"r","w"}, path.index.lua },
        { "f", {"r","w"}, path.index.luc },
        { "f", {"r","w"}, path.lookups.lua },
        { "f", {"r","w"}, path.lookups.luc },
    }

    local check_permissions = function (errcnt)
        out [[=============== file permissions ==============]]
        for i = 1, #desired_permissions do
            local t, spec, path = unpack (desired_permissions[i])
            if type (path) == "function" then
                path = path ()
            end

            spec = tabletohash (spec)

            out ("Checking permissions of %s.", path)

            local permissions = get_permissions (t, path)
            if permissions then
                --inspect (permissions)
                errcnt = check_conformance (spec, permissions, errcnt)
            else
                errcnt = errcnt + 1
            end
        end
        return errcnt
    end

    local check_upstream

    if kpsefind_file ("https.lua", "lua") == nil then
        check_upstream = function (errcnt)
            out       [[============= upstream repository =============
                        WARNING: Cannot retrieve repository data.
                        Github API access requires the luasec library.
                        Grab it from <https://github.com/brunoos/luasec>
                        and retry.]]
            return errcnt
        end
    else
    --- github api stuff begin
        local https = require "ssl.https"

        local gh_api_root     = [[https://api.github.com]]
        local release_url     = [[https://github.com/lualatex/luaotfload/releases]]
        local luaotfload_repo = [[lualatex/luaotfload]]
        local user_agent      = [[lualatex/luaotfload integrity check]]
        local shortbytes = 8

        local gh_shortrevision = function (rev)
            return stringsub (rev, 1, shortbytes)
        end

        local gh_encode_parameters = function (parameters)
            local acc = {}
            for field, value in next, parameters do
                --- unsafe, non-urlencoded coz it’s all ascii chars
                acc[#acc+1] = field .. "=" .. value
            end
            return "?" .. tableconcat (acc, "&")
        end

        local gh_make_url = function (components, parameters)
            local url = tableconcat ({ gh_api_root,
                                       unpack (components) },
                                     "/")
            if parameters then
                url = url .. gh_encode_parameters (parameters)
            end
            return url
        end

        local alright = [[HTTP/1.1 200 OK]]

        local gh_api_request = function (...)
            local args    = {...}
            local nargs   = #args
            local final   = args[nargs]
            local request = {
                url     = "",
                headers = { ["user-agent"] = user_agent },
            }
            if type (final) == "table" then
                args[nargs] = nil
                request = gh_make_url (args, final)
            else
                request = gh_make_url (args)
            end

            out ("Requesting <%s>.", request)
            local response, code, headers, status
                = https.request (request)
            if status ~= alright then
                out "Request failed!"
                return false
            end
            return response
        end

        local gh_api_checklimit = function (headers)
            local rawlimit  = gh_api_request "rate_limit"
            local limitdata = lua_of_json (rawlimit)
            if not limitdata and limitdata.rate then
                out "Cannot parse API rate limit."
                return false
            end
            limitdata = limitdata.rate

            local limit = tonumber (limitdata.limit)
            local left  = tonumber (limitdata.remaining)
            local reset = tonumber (limitdata.reset)

            out ("%d of %d Github API requests left.", left, limit)
            if left == 0 then
                out ("Cannot make any more API requests.")
                out ("Try again later at %s.", osdate ("%F %T", reset))
            end
            return true
        end

        local gh_tags = function ()
            out "Fetching tags from repository, please stand by."
            local rawtags = gh_api_request ("repos",
                                            luaotfload_repo,
                                            "tags")
            local taglist = lua_of_json (rawtags)
            if not taglist or #taglist == 0 then
                out "Cannot parse response."
                return false
            end

            local ntags = #taglist
            out ("Repository contains %d tags.", ntags)
            local _idx, latest = next (taglist)
            out ("The most recent release is %s (revision %s).",
                 latest.name,
            gh_shortrevision (latest.commit.sha))
            return latest
        end

        local gh_compare = function (head, base)
            if base == nil then
                base = "HEAD"
            end
            out ("Fetching comparison between %s and %s, \z
                  please stand by.",
                 gh_shortrevision (head),
                 gh_shortrevision (base))
            local comparison = base .. "..." .. head
            local rawstatus = gh_api_request ("repos",
                                              luaotfload_repo,
                                              "compare",
                                              comparison)
            local status = lua_of_json (rawstatus)
            if not status then
                out "Cannot parse response for status request."
                return false
            end
            return status
        end

        local gh_news = function (since)
            local compared  = gh_compare (since)
            if not compared then
                return false
            end
            local behind_by = compared.behind_by
            local ahead_by  = compared.ahead_by
            local status    = compared.status
            out ("Comparison state: %s.", status)
            if behind_by > 0 then
                out ("Your Luaotfload is %d \z
                      revisions behind upstream.",
                     behind_by)
                return behind_by
            elseif status == "ahead" then
                out "Since you are obviously from the future \z
                     I assume you already know the repository state."
            else
                out "Everything up to date. \z
                     Luaotfload is in sync with upstream."
            end
            return false
        end

        local gh_catchup = function (current, latest)
            local compared = gh_compare (latest, current)
            local ahead_by = tonumber (compared.ahead_by)
            if ahead_by > 0 then
                local permalink_url = compared.permalink_url
                out ("Your Luaotfload is %d revisions \z
                      behind the most recent release.",
                     ahead_by)
                out ("To view the commit log, visit <%s>.",
                     permalink_url)
                out ("You can grab an up to date tarball at <%s>.",
                     release_url)
                return true
            else
                out "There weren't any new releases in the meantime."
                out "Luaotfload is up to date."
            end
            return false
        end

        check_upstream = function (current)
            out "============= upstream repository ============="
            local _succ  = gh_api_checklimit ()
            local behind = gh_news (current)
            if behind then
                local latest  = gh_tags ()
                local _behind = gh_catchup (current,
                                            latest.commit.sha,
                                            latest.name)
            end
        end

        --- trivium: diff since the first revision as pushed by Élie
        --- in 2009
        --- local firstrevision = "c3ccb3ee07e0a67171c24960966ae974e0dd8e98"
        --- check_upstream (firstrevision)
    end
    --- github api stuff end

    local anamneses   = { "files", "repository", "permissions" }

    actions.diagnose = function (job)
        local errcnt = 0
        local asked  = job.asked_diagnostics
        if asked == "all" or asked == "thorough" then
            asked = tabletohash (anamneses, true)
        else
            asked = lpegmatch(split_comma, asked)
            asked = tabletohash (asked, true)
        end

        if asked.files == true then
            errcnt = verify_files (errcnt, status)
            asked.files = nil
        end
        if asked.permissions == true then
            errcnt = check_permissions (errcnt)
            asked.permissions = nil
        end
        if asked.repository == true then
            check_upstream (status.notes.revision)
            asked.repository = nil
        end

        local rest = next (asked)
        if rest ~= nil then --> something unknown
            out ("Unknown diagnostic %q.", rest)
        end
        if errcnt == 0 then --> success
            out ("Everything appears to be in order, \z
                  you may sleep well.")
            return true, false
        end
        out (         [[===============================================
                                            WARNING
                        ===============================================

                        The diagnostic detected %d errors.

                        This version of luaotfload may have been
                        tampered with. Modified versions of the
                        luaotfload source are unsupported. Read the log
                        carefully and get a clean version from CTAN or
                        github:

                            × http://ctan.org/tex-archive/macros/luatex/generic/luaotfload
                            × https://github.com/lualatex/luaotfload/releases

                        If you are uncertain as to how to proceed, then
                        ask on the lualatex mailing list:

                            http://www.tug.org/mailman/listinfo/lualatex-dev

                        ===============================================
]],          errcnt)
        return true, false
    end
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
        diagnose           = 1,
        ["dry-run"]        = "D",
        ["flush-lookups"]  = "l",
        fields             = 1,
        find               = 1,
        force              = "f",
        fuzzy              = "F",
        help               = "h",
        info               = "i",
        inspect            = "I",
        limit              = 1,
        list               = 1,
        log                = 1,
        ["prefer-texmf"]   = "p",
        quiet              = "q",
        ["show-blacklist"] = "b",
        update             = "u",
        verbose            = 1  ,
        version            = "V",
        warnings           = "w",
    }

    local short_options = "bDfFiIlpquvVhw"

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
                result.log_level = 2
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
            config.luaotfload.self = optarg[n]
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
            config.luaotfload.prioritize = "texmf"
        elseif v == "b" then
            action_pending["blacklist"] = true
        elseif v == "diagnose" then
            action_pending["diagnose"] = true
            result.asked_diagnostics = optarg[n]
        end
    end

    if config.luaotfload.self == "mkluatexfontdb" then
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
