#!/usr/bin/env texlua
-----------------------------------------------------------------------
--         FILE:  luaotfload-tool.lua
--  DESCRIPTION:  database functionality
-- REQUIREMENTS:  luaotfload 2.2
--       AUTHOR:  Khaled Hosny, Élie Roux, Philipp Gesang
--      VERSION:  2.2
--      LICENSE:  GPL v2
--      CREATED:  2013-05-06 13:37:12+0200
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

kpse.set_program_name"luatex"

local stringformat    = string.format
local texiowrite_nl   = texio.write_nl
local stringlower     = string.lower


local loader_file = "luatexbase.loader.lua"
local loader_path = assert(kpse.find_file(loader_file, "lua"),
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

config              = config or { }
local config        = config
config.luaotfload   = config.luaotfload or { }

do -- we don’t have file.basename and the likes yet, so inline parser ftw
    local C, P         = lpeg.C, lpeg.P
    local lpegmatch    = lpeg.match
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
config.lualibs.load_extended    = false

require"lualibs"

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

local version  = "2.2" -- same version number as luaotfload
local names    = fonts.names

local db_src_out = names.path.dir.."/"..names.path.basename
local db_bin_out = file.replacesuffix(db_src_out, "luc")

local help_messages = {
    ["luaotfload-tool"] = [[

Usage: %s [OPTION]...
    
Operations on the LuaTeX font database.

This tool is part of the luaotfload package. Valid options are:

-------------------------------------------------------------------------------
                             VERBOSITY AND LOGGING

  -q --quiet                   don't output anything
  -v --verbose=LEVEL           be more verbose (print the searched directories)
  -vv                          print the loaded fonts
  -vvv                         print all steps of directory searching
  -V --version                 print version and exit
  -h --help                    print this message

  --alias=<name>               force behavior of “luaotfload-tool” or legacy
                               “mkluatexfontdb”
-------------------------------------------------------------------------------
                                   DATABASE

  -u --update                  update the database
  -f --force                   force re-indexing all fonts
  -c --flush-cache             empty cache of font requests

  --find="font name"           query the database for a font name
  -F --fuzzy                   look for approximate matches if --find fails
  --limit=n                    limit display of fuzzy matches to <n>
                               (default: n = 1)
  -i --info                    display font metadata

  --log=stdout                 redirect log output to stdout

The font database will be saved to
   %s
   %s

]],
    mkluatexfontdb = [[

Usage: %s [OPTION]...
    
Rebuild the LuaTeX font database.

Valid options:
  -f --force                   force re-indexing all fonts
  -q --quiet                   don't output anything
  -v --verbose=LEVEL           be more verbose (print the searched directories)
  -vv                          print the loaded fonts
  -vvv                         print all steps of directory searching
  -V --version                 print version and exit
  -h --help                    print this message
  --alias=<name>               force behavior of “luaotfload-tool” or legacy
                               “mkluatexfontdb”

The font database will be saved to
   %s
   %s

]],
}

local help_msg = function ( )
    local template = help_messages[config.luaotfload.self]
                  or help_messages["luaotfload-tool"]
    texiowrite_nl(stringformat(template, config.luaotfload.self, db_src_out, db_bin_out))
end

local version_msg = function ( )
    texiowrite_nl(stringformat(
        "%s version %s, database version %s.\n",
        config.luaotfload.self, version, names.version))
end

local show_info_items = function (fontinfo)
    local items    = table.sortedkeys(fontinfo)
    for n = 1, #items do
        local item = items[n]
        texiowrite_nl(stringformat(
            [[  %11s:  %s]], item, fontinfo[item]))
    end
end

local show_font_info = function (filename)
    local fullname = resolvers.findfile(filename)
    if fullname then
        local fontinfo = fontloader.info(fullname)
        local nfonts   = #fontinfo
        if nfonts > 0 then -- true type collection
            logs.names_report(true, 1, "resolve",
                [[%s is a font collection]], filename)
            for n = 1, nfonts do
                logs.names_report(true, 1, "resolve",
                    [[showing info for font no. %d]], n)
                show_info_items(fontinfo[n])
            end
        else
            show_info_items(fontinfo)
        end
    else
        logs.names_report(true, 1, "resolve",
            "font %s not found", filename)
    end
end

--[[--
Running the scripts triggers one or more actions that have to be
executed in the correct order. To avoid duplication we track them in a
set.
--]]--

local action_sequence = {
    "loglevel", "help", "version", "flush", "generate", "query"
}
local action_pending  = table.tohash(action_sequence, false)

action_pending.loglevel = true  --- always set the loglevel
action_pending.generate = false --- this is the default action

local actions = { } --- (jobspec -> (bool * bool)) list

actions.loglevel = function (job)
    logs.set_loglevel(job.log_level)
    logs.names_report("info", 3, "util",
                      "setting log level", "%d", job.log_level)
    return true, true
end

actions.version = function (job)
    version_msg()
    return true, false
end

actions.help = function (job)
    help_msg()
    return true, false
end

actions.generate = function (job)
    local fontnames, savedname
    fontnames = names.update(fontnames, job.force_reload)
    logs.names_report("info", 2, "db",
        "Fonts in the database: %i", #fontnames.mappings)
    savedname = names.save(fontnames)
    if savedname then --- FIXME have names.save return bool
        return true, true
    end
    return false, false
end

actions.flush = function (job)
    local success, lookups = names.flush_cache()
    if success then
        local savedname = names.save_lookups()
        logs.names_report("info", 2, "cache", "Cache emptied")
        if savedname then
            return true, true
        end
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
        logs.names_report(false, 1,
            "resolve", "Font “%s” found!", query)
        if subfont then
            logs.names_report(false, 1, "resolve",
                "Resolved file name “%s”, subfont nr. “%s”",
                foundname, subfont)
        else
            logs.names_report(false, 1,
                "resolve", "Resolved file name “%s”", foundname)
        end
        if job.show_info then
            show_font_info(foundname)
        end
    else
        logs.names_report(false, 1,
            "resolve", "Cannot find “%s”.", query)
        if job.fuzzy == true then
            logs.names_report(false, 1,
                "resolve", "Looking for close matches, this may take a while ...")
            local success = fonts.names.find_closest(query, job.fuzzy_limit)
        end
    end
    return true, true
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
        query        = "",
        log_level    = 1, --- 2 is approx. the old behavior
    }

    local long_options = {
        alias            = 1,
        ["flush-cache"]  = "c",
        find             = 1,
        force            = "f",
        fuzzy            = "F",
        help             = "h",
        info             = "i",
        limit            = 1,
        log              = 1,
        quiet            = "q",
        update           = "u",
        verbose          = 1  ,
        version          = "V",
    }

    local short_options = "cfFiquvVh"

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
                result.log_level = tonumber(lvl)
            end
        elseif v == "log" then
            local str = optarg[n]
            if str then
                logs.set_logout(str)
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
        elseif v == "alias" then
            config.luaotfload.self = optarg[n]
        elseif v == "c" then
            action_pending["flush"] = true
        end
    end

    if config.luaotfload.self == "mkluatexfontdb" then
        action_pending["generate"] = true
        result.log_level = math.max(2, result.log_level)
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
            logs.names_report("log", 3, "util", "preparing for task",
                              "%s", actionname)

            local action             = actions[actionname]
            local success, continue  = action(job)

            if not success then
                logs.names_report(false, 0, "util",
                    "could not finish task", "%s", actionname)
                retval = -1
                exit   = true
            elseif not continue then
                logs.names_report(false, 3, "util",
                    "task completed, exiting", "%s", actionname)
                exit   = true
            else
                logs.names_report(false, 3, "util",
                    "task completed successfully", "%s", actionname)
            end
        end
        if exit then break end
    end

    texiowrite_nl""
    return retval
end

return main()

-- vim:tw=71:sw=4:ts=4:expandtab
