#!/usr/bin/env texlua

-- Must be the first line of any texlua script: LuaTeX doesn't add the
-- kpse-aware require() searcher until this has run, and other things
-- are half-broken without it too.
kpse.set_program_name("luatex")

local SPITOOL_VERSION = "1.1"
local lfs = require("lfs")

-- OS detection and Quoting helper (POSIX vs Windows)
local is_windows = package.config:sub(1, 1) == '\\'

local function quote_arg(v)
    if is_windows then
        if v:find("%%") then
            io.stderr:write("[WARNING] argument contains '%': "
                .. v .. " -- cmd.exe may expand it as an environment variable.\n")
        end
        return '"' .. v:gsub('"', '\\"') .. '"'
    else
        return "'" .. v:gsub("'", "'\\''") .. "'"
    end
end

-- Helper: Check executable presence in system PATH
local function has_cmd(cmd)
    local null_dev = is_windows and "NUL" or "/dev/null"
    local check = is_windows and ("where " .. cmd .. " > " .. null_dev .. " 2>&1")
                             or ("command -v " .. cmd .. " > " .. null_dev .. " 2>&1")
    local res = os.execute(check)
    return (res == true or res == 0)
end

-- Input filename safety validation (prevents command & TeX injection)
local function is_safe_filename(filename)
    if filename:match("[%s\"'%%&|<>^`$%{%}%[%]!#~]") then
        return false
    end
    return true
end

-- File name without its directory part (TeX's \jobname never has one)
local function file_name_only(path)
    return path:match("([^/\\]+)$") or path
end

-- Config & CLI Parser State
local config = {
    path = nil,
    strict = false,
    overwrite = false,
    check_only = false,
    engine = nil,
    help = false,
    version = false
}
local input_files = {}

-- Set when any validation or compilation step fails (drives the exit code)
local had_failure = false

-- True only for regular files (directories are rejected)
local function is_regular_file(path)
    local attr = lfs.attributes(path)
    return attr ~= nil and attr.mode == "file"
end

-- Smart file path resolution (allows omitting extensions)
local function resolve_input_file(input)
    if is_regular_file(input) then
        return input
    end

    local candidates
    if config.check_only then
        candidates = { input .. ".pdf" }
    else
        candidates = {
            input .. ".tex",
            input .. ".ltx",
            input .. ".pdf",
            input .. "-luamml-mathml.html",
            input .. ".html"
        }
    end

    for _, candidate in ipairs(candidates) do
        if is_regular_file(candidate) then
            return candidate
        end
    end

    return nil
end

-- Check if output file exists and enforce --overwrite protection
local function check_output_permission(output_path, overwrite_flag)
    local check = io.open(output_path, "r")
    if check then
        check:close()
        if not overwrite_flag then
            io.stderr:write("Error: output file '" .. output_path .. "' already exists. Use -o or --overwrite to replace it.\n")
            return false
        end
    end
    return true
end

-- Docopt-style CLI parser
local i = 1
local parse_flags = true

while i <= #arg do
    local current = arg[i]

    if parse_flags and current == "--" then
        parse_flags = false

    elseif parse_flags and (current == "--help" or current == "-h") then
        config.help = true

    elseif parse_flags and (current == "--version" or current == "-v") then
        config.version = true

    elseif parse_flags and (current == "--strict" or current == "-s") then
        config.strict = true

    elseif parse_flags and (current == "--overwrite" or current == "-o") then
        config.overwrite = true

    elseif parse_flags and (current == "--check-only" or current == "-c") then
        config.check_only = true

    elseif parse_flags and (current == "--path" or current == "-p") then
        if not arg[i+1] then
            io.stderr:write("Error: option '" .. current .. "' requires a value\n")
            os.exit(1)
        end
        i = i + 1
        config.path = arg[i]

    elseif parse_flags and (current:match("^%-%-path=(.*)") or current:match("^%-p=(.*)")) then
        config.path = current:match("^%-%-path=(.*)") or current:match("^%-p=(.*)")

    elseif parse_flags and (current == "--engine" or current == "-e") then
        if not arg[i+1] then
            io.stderr:write("Error: option '" .. current .. "' requires a value\n")
            os.exit(1)
        end
        i = i + 1
        config.engine = arg[i]

    elseif parse_flags and (current:match("^%-%-engine=(.*)") or current:match("^%-e=(.*)")) then
        config.engine = current:match("^%-%-engine=(.*)") or current:match("^%-e=(.*)")

    else
        if parse_flags and current:sub(1,1) == "-" then
            io.stderr:write("Error: unrecognized option '" .. current .. "'\n")
            os.exit(1)
        else
            table.insert(input_files, current)
        end
    end

    i = i + 1
end

-- Zero arguments check: show help if running from interactive terminal (TTY)
if #arg == 0 then
    local ffi = require("ffi")
    ffi.cdef[[
        int isatty(int fd);
        int _isatty(int fd);
    ]]
    local is_tty
    if ffi.os == "Windows" then
        is_tty = ffi.C._isatty(0) ~= 0
    else
        is_tty = ffi.C.isatty(0) ~= 0
    end
    if is_tty then
        config.help = true
    end
end

-- Docopt Help banner
if config.help then
    print("spmmli-tool v" .. SPITOOL_VERSION .. " - Tool for cleaning luamml ghosts and validating LaTeX tagged PDFs")
    print([[
Usage:
  spmmli-tool [options] <file.pdf | file-luamml-mathml.html | file.tex>

Description:
  spmmli-tool prepares the MathML files generated by luamml for mathml-AF: it
  removes the invisible ghost characters (U+2063 with sptmp attributes)
  and writes the result to <name>-mathml.html.

  Depending on the input file:
  .pdf    validates the tag structure (rnv-wrapp) and the PDF/UA-2
          conformance (veraPDF), if the tools exist.
  .html   cleans an existing <name>-luamml-mathml.html.
  .tex    runs the TeX engine once in a temporary directory with AFghost=true to
          obtain the luamml HTML, then cleans it. The temporary PDF is
          discarded and not validated.

Options:
  -c, --check-only    Only validate; the input must be a .pdf file.
  -o, --overwrite     Force replacing existing output files (-mathml.html).
  -p, --path=<path>   Target directory where output files will be created
                      (created if it does not exist).
  -e, --engine=<cmd>  TeX engine used to compile (default: lualatex-dev if
                      available, otherwise lualatex).
  -s, --strict        Stop at the first failure (compilation or validation).
                      Without it, all files are processed; in both cases the
                      exit code is non-zero if anything failed.
  -h, --help          Show this usage summary and exit.
  -v, --version       Show spmmli-tool version number and exit.

Examples:
  $ spmmli-tool -c sample.pdf
  $ spmmli-tool -c -s sample
  $ spmmli-tool sample-luamml-mathml.html
  $ spmmli-tool -o -p build/ sample.tex

Issues and reports:
  Repository : https://github.com/pablgonz/spanish-mathml-intent
  Bug tracker: https://github.com/pablgonz/spanish-mathml-intent/issues
  Copyright(C) 2026 by Pablo González L <pablgonz<at>educarchile.cl>
]])
    os.exit(0)
end

-- Version banner
if config.version then
    print("spmmli-tool v" .. SPITOOL_VERSION)
    os.exit(0)
end

if config.engine and not config.engine:match("^[%w%._%-]+$") then
    io.stderr:write("Error: illegal characters in engine name: " .. config.engine .. "\n")
    os.exit(1)
end

if config.check_only and (config.path or config.overwrite) then
    io.stderr:write("[WARNING] -p/--path and -o/--overwrite have no effect with -c/--check-only.\n")
end

if #input_files == 0 then
    io.stderr:write("Error: no input file specified. Use --help for usage information.\n")
    os.exit(1)
end

-- Recursive directory removal helper
local function remove_dir_recursive(path)
    if not path or path == "" then return end
    local attr = lfs.attributes(path)
    if not attr or attr.mode ~= "directory" then return end

    for entry in lfs.dir(path) do
        if entry ~= "." and entry ~= ".." then
            local full_path = path .. "/" .. entry
            -- symlinkattributes: never follow links out of the temp dir
            local entry_attr = lfs.symlinkattributes(full_path)
            if entry_attr and entry_attr.mode == "directory" then
                remove_dir_recursive(full_path)
            else
                os.remove(full_path)
            end
        end
    end
    lfs.rmdir(path)
end

-- Secure temp directory helpers
local function system_temp_dir()
    if is_windows then
        return os.getenv("TEMP") or os.getenv("TMP") or "C:\\Windows\\Temp"
    else
        return os.getenv("TMPDIR") or "/tmp"
    end
end

local function secure_temp_subdir()
    -- POSIX: mktemp -d creates the directory atomically with mode 700
    if not is_windows and has_cmd("mktemp") then
        local template = system_temp_dir():gsub("/$", "") .. "/spmmli-tool-XXXXXX"
        local p = io.popen("mktemp -d " .. quote_arg(template) .. " 2>/dev/null")
        if p then
            local dir = p:read("*l")
            p:close()
            if dir and dir ~= "" and lfs.attributes(dir, "mode") == "directory" then
                return dir
            end
        end
    end

    -- Fallback (Windows, or no mktemp)
    local base = system_temp_dir()
    local sep = package.config:sub(1, 1)
    math.randomseed(os.time() + math.floor(os.clock() * 100000))
    local unique = string.format("%d_%d", os.time(), math.random(100000, 999999))
    local tmpdir = base .. sep .. "spmmli-tool-" .. unique

    local ok, err = lfs.mkdir(tmpdir)
    if not ok then
        io.stderr:write("Error: failed to create secure temp directory: " .. tostring(err) .. "\n")
        os.exit(1)
    end

    if not is_windows then
        os.execute("chmod 700 " .. quote_arg(tmpdir))
    end

    return tmpdir
end

-- Create the output directory (and parents) if needed
local function ensure_dir(path)
    if lfs.attributes(path, "mode") == "directory" then return true end
    local current = path:sub(1, 1) == "/" and "/" or ""
    for part in path:gmatch("[^/\\]+") do
        current = (current == "" or current:sub(-1) == "/") and (current .. part) or (current .. "/" .. part)
        if lfs.attributes(current, "mode") ~= "directory" then
            local ok, err = lfs.mkdir(current)
            if not ok then return nil, err end
        end
    end
    return true
end

-- Shared cleanup: Strips ghost tags/characters from dirty HTML
--
-- Each ghost is a <mi ... sptmp="outer|inner" ...>U+2063</mi> element. It is
-- removed with a pattern over the whole file content (not line by line), so
-- it works both for indented HTML (one element per line) and for flat HTML
-- (a whole formula on a single line). Elements with an unknown sptmp kind
-- are left untouched.
local function strip_ghosts(input_path, output_path)
    local html_file = io.open(input_path, "rb")
    if not html_file then
        return nil, "failed to open " .. input_path
    end
    local content = html_file:read("*a")
    html_file:close()

    local count_outer, count_inner = 0, 0
    -- U+2063 may appear as the literal character or as a character reference
    local ghost_forms = { "\u{2063}", "&#x2063;", "&#X2063;", "&#8291;" }
    local clean = content
    for _, ghost in ipairs(ghost_forms) do
        -- optional leading newline + indentation is removed together with the tag
        local pattern = "\n?[ \t]*<mi([^>]-)sptmp=\"(%a+)\"([^>]-)>"
            .. ghost:gsub("%p", "%%%0") .. "</mi>"
        clean = clean:gsub(pattern, function(_, kind)
            if kind == "outer" then
                count_outer = count_outer + 1
            elseif kind == "inner" then
                count_inner = count_inner + 1
            else
                return nil -- unknown kind: keep the match as is
            end
            return ""
        end)
    end

    local out = io.open(output_path, "wb")
    if not out then
        return nil, "failed to create " .. output_path
    end
    out:write(clean)
    out:close()

    local function count_lines(s)
        local _, n = s:gsub("\n", "\n")
        return n + ((s ~= "" and s:sub(-1) ~= "\n") and 1 or 0)
    end

    return {
        total_lines = count_lines(content),
        count_outer = count_outer,
        count_inner = count_inner,
        kept_lines  = count_lines(clean),
    }
end

local function report_stats(stats, output_name, empty_hint)
    print(string.format("  -> Lines read: %d", stats.total_lines))
    print(string.format("  -> 'outer' ghosts removed: %d", stats.count_outer))
    print(string.format("  -> 'inner' ghosts removed: %d", stats.count_inner))
    print(string.format("  -> Generated: %s (%d lines)", output_name, stats.kept_lines))
    if stats.count_outer == 0 and stats.count_inner == 0 then
        print("[WARNING] No ghost characters found.")
        print("  " .. empty_hint)
        return false
    end
    return true
end

-- Mode 1: PDF Validation Only (No TeX compilation)
local function run_pdf_mode(pdf_filename)
    local run_rnv = has_cmd("rnv-wrapp") and has_cmd("show-pdf-tags")
    local run_verapdf = has_cmd("verapdf")

    if not run_rnv and not run_verapdf then
        io.stderr:write("Error: no PDF validators (show-pdf-tags/rnv-wrapp or veraPDF) detected in PATH.\n")
        os.exit(1)
    end

    print("==================================================")
    print(" Testing PDF compliance: " .. pdf_filename)
    print("==================================================")

    if run_rnv and run_verapdf then
        print("Validators detected: show-pdf-tags, rnv-wrapp & veraPDF")
    elseif run_rnv then
        print("Validators detected: show-pdf-tags & rnv-wrapp")
    elseif run_verapdf then
        print("Validator detected: veraPDF")
    end

    local null_dev = is_windows and "NUL" or "/dev/null"
    local validation_failed = false

    -- 1. RNC tag structure check
    if run_rnv then
        local cmd_rnv = "show-pdf-tags --xml " .. quote_arg(pdf_filename) .. " | rnv-wrapp"
        local res_rnv = os.execute(cmd_rnv .. " > " .. null_dev .. " 2>&1")
        if res_rnv ~= true and res_rnv ~= 0 then
            print("\n[RNC Validation Error Output]:")
            os.execute(cmd_rnv)
            io.stderr:write("Error: tag structure validation (rnv-wrapp) failed for " .. pdf_filename .. "\n")
            validation_failed = true
        end
    end

    -- 2. PDF/UA-2 compliance check
    if run_verapdf then
        local cmd_vera = "verapdf --flavour ua2 --format text " .. quote_arg(pdf_filename)
        local res_vera = os.execute(cmd_vera .. " > " .. null_dev .. " 2>&1")
        if res_vera ~= true and res_vera ~= 0 then
            print("\n[PDF/UA-2 Validation Error Output]:")
            os.execute(cmd_vera)
            io.stderr:write("Error: veraPDF (PDF/UA-2) validation failed for " .. pdf_filename .. "\n")
            validation_failed = true
        end
    end

    if not validation_failed then
        print("PASS")
    else
        print("FAIL")
        had_failure = true
        if config.strict then
            os.exit(1)
        end
    end
end

-- Mode 2: Clean pre-existing *-luamml-mathml.html
local function run_clean_mode(html_filename)
    local basename = html_filename:match("^(.*)%-luamml%-mathml%.html$")
    if not basename then
        io.stderr:write("Error: expected filename ending in -luamml-mathml.html: " .. html_filename .. "\n")
        os.exit(1)
    end

    -- With -p/--path the output goes to that directory, named after the
    -- file only (the input directory is not repeated inside it)
    local output_name
    if config.path then
        output_name = config.path:gsub("[/\\]$", "") .. "/" .. file_name_only(basename) .. "-mathml.html"
    else
        output_name = basename .. "-mathml.html"
    end

    if not check_output_permission(output_name, config.overwrite) then
        os.exit(1)
    end
    if config.path then
        local ok, derr = ensure_dir(config.path)
        if not ok then
            io.stderr:write("Error: could not create directory '" .. config.path .. "': " .. tostring(derr) .. "\n")
            os.exit(1)
        end
    end

    print("==================================================")
    print(" Processing: " .. html_filename)
    print("==================================================")

    local stats, err = strip_ghosts(html_filename, output_name)
    if not stats then
        io.stderr:write("Error: " .. err .. "\n")
        os.exit(1)
    end

    local found_something = report_stats(stats, output_name,
        "If this .html was not generated via spmmli-tool from a .tex source, "
        .. "it might not contain ghosts -- try passing the .tex/.ltx file directly.")
    if not found_something then
        os.remove(output_name)
        return
    end
end

-- Mode 3: Compile .tex/.ltx and clean the generated HTML
local function run_generate_mode(tex_filename)
    local basename, ext = tex_filename:match("^(.*)%.(%a+)$")
    if not basename or (ext ~= "tex" and ext ~= "ltx") then
        io.stderr:write("Error: expected a .tex or .ltx file: " .. tostring(tex_filename) .. "\n")
        os.exit(1)
    end

    -- TeX names its output files after the jobname, which has no directory
    local jobname = file_name_only(basename)

    local output_name
    if config.path then
        output_name = config.path:gsub("[/\\]$", "") .. "/" .. jobname .. "-mathml.html"
    else
        output_name = basename .. "-mathml.html"
    end

    if not check_output_permission(output_name, config.overwrite) then
        os.exit(1)
    end

    if config.path then
        local ok, derr = ensure_dir(config.path)
        if not ok then
            io.stderr:write("Error: could not create directory '" .. config.path .. "': " .. tostring(derr) .. "\n")
            os.exit(1)
        end
    end

    local source = io.open(tex_filename, "r")
    if not source then
        io.stderr:write("Error: could not open " .. tex_filename .. "\n")
        os.exit(1)
    end
    source:close()

    local tmpdir = secure_temp_subdir()

    -- The temp directory is kept when compilation fails, so the .log
    -- mentioned in the error message is still there to be read
    local keep_tmp = false

    -- Guaranteed cleanup closure
    local function cleanup_and_exit(code)
        if not keep_tmp then
            remove_dir_recursive(tmpdir)
        end
        os.exit(code or 0)
    end

    local engine = config.engine
    if not engine then
        engine = has_cmd("lualatex-dev") and "lualatex-dev" or "lualatex"
    end
    if not has_cmd(engine) then
        io.stderr:write("Error: TeX engine '" .. engine .. "' not found in PATH.\n")
        cleanup_and_exit(1)
    end

    local latex_cmd = string.format(
        '%s --interaction=nonstopmode --output-directory=%s %s',
        engine,
        quote_arg(tmpdir),
        quote_arg("\\PassOptionsToPackage{AFghost=true}{spanish-mathml-intent}\\input{" .. tex_filename .. "}")
    )

    print("==================================================")
    print(" Compiling with AFghost=true for " .. tex_filename)
    print("==================================================")

    local null_dev = is_windows and "NUL" or "/dev/null"
    local res = os.execute(latex_cmd .. " > " .. null_dev .. " 2>&1")
    if res ~= true and res ~= 0 then
        keep_tmp = true
        had_failure = true
        io.stderr:write("Error: compilation failed. Log file available in: " .. tmpdir .. "\n")
        if config.strict then
            cleanup_and_exit(1)
        end
    end

    local generated_html = tmpdir .. "/" .. jobname .. "-luamml-mathml.html"
    local check = io.open(generated_html, "r")
    if not check then
        keep_tmp = true
        io.stderr:write("Error: " .. generated_html .. " was not generated. Verify spanish-mathml-intent package loading in " .. tex_filename .. "\n")
        had_failure = true
        cleanup_and_exit(1)
    end
    check:close()

    print("\n==================================================")
    print(" Cleaning ghost characters")
    print("==================================================")

    local stats, err = strip_ghosts(generated_html, output_name)
    if not stats then
        io.stderr:write("Error: " .. err .. "\n")
        cleanup_and_exit(1)
    end
    local found_something = report_stats(stats, output_name,
        "Forced AFghost=true during compilation -- if no ghosts were found, "
        .. "verify that spanish-mathml-intent is loaded in " .. tex_filename .. ".")
    if not found_something then
        os.remove(output_name)
    end

    -- Guaranteed cleanup of temporary directory upon completion
    -- (no os.exit here: other input files may still be pending)
    if not keep_tmp then
        remove_dir_recursive(tmpdir)
    end
end

-- Main Execution Loop
for _, arg_input in ipairs(input_files) do
    local resolved_file = resolve_input_file(arg_input)

    if not resolved_file then
        io.stderr:write("Error: could not find file '" .. arg_input .. "'\n")
        os.exit(1)
    end

    if not is_safe_filename(resolved_file) then
        io.stderr:write("Error: input filename contains unsafe or illegal characters: " .. resolved_file .. "\n")
        os.exit(1)
    end

    if config.check_only and not resolved_file:match("%.pdf$") then
        io.stderr:write("Error: -c/--check-only expects a .pdf file: " .. resolved_file .. "\n")
        os.exit(1)
    end

    if resolved_file:match("%.pdf$") then
        run_pdf_mode(resolved_file)
    elseif resolved_file:match("%.html$") then
        run_clean_mode(resolved_file)
    elseif resolved_file:match("%.tex$") or resolved_file:match("%.ltx$") then
        run_generate_mode(resolved_file)
    else
        io.stderr:write("Error: unrecognized file extension for '" .. resolved_file .. "'\n")
        os.exit(1)
    end
end

if had_failure then
    os.exit(1)
end
