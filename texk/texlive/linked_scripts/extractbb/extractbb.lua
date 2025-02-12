#!/usr/bin/env texlua
-- extractbb-lua
-- https://github.com/gucci-on-fleek/extractbb
-- SPDX-License-Identifier: MPL-2.0+
-- SPDX-FileCopyrightText: 2024--2025 Max Chernoff
--
-- Inclusion Methods
-- =================
--
-- This script can use two different methods to extract bounding boxes from
-- images: the "img" module and the "pdfe" module. The "img" module will be
-- automatically selected in most cases and supports all image types that are
-- supported by the original "extractbb" program. If and only if the "img"
-- module fails to load, the "pdfe" module will be used as a fallback. However,
-- the "pdfe" module only supports PDF files. Both modules are built in to the
-- LuaTeX binaries, however due to some technical issues, the "img" module may
-- fail to load on some more exotic platforms.
--
--
-- Compatibility
-- =============
--
-- Based off of my testing, this Lua script is 100% compatible with the original
-- C-based "extractbb" program, with the following exceptions:
--
--   * When running in "img" mode, the PDF version is always reported as "1.5".
--
--   * When running in "img" mode, if the requested bounding box is not found,
--     the script will fallback to the Crop box or the Media box, instead of
--     following the original fallback order. (In practice, almost all PDFs set
--     all their bounding boxes equal to each other, and even if the boxes are
--     set to different values, the script will still return the requested box,
--     provided that it is set in the PDF.)
--
--   * When running in "pdfe" mode, only PDF files are supported.
--
-- All of these issues are very unlikely to affect any real-world documents.
--
--
-- Security
-- ========
--
-- This script is designed to be safely ran from restricted shell escape. A few
-- security features:
--
--   * The majority of this script runs inside a sandboxed Lua environment,
--     which only exposes a very restricted set of functions.
--
--   * All file-related functions available inside the sandbox first check with
--     kpathsea to ensure that the file is allowed to be opened.
--
--   * In the event of any errors, the script immediately exits.
--
--   * This script does not run (fork/exec) any external programs.
--
--   * This script is written entirely in Lua, so overflow/use-after-free
--     vulnerabilities are not possible.
--
-- Some potential security concerns:
--
--   * This script has not been audited or reviewed by anyone other than myself.
--
--   * The underlying LuaTeX modules may themselves have security
--     vulnerabilities, which would be inherited by this script.


----------------------
--- Initialization ---
----------------------

-- Pre-sandbox variables/constants
local show_errors = true
local SOURCE_DATE_EPOCH = tonumber(os.getenv("SOURCE_DATE_EPOCH"))
local version = "extractbb.lua v1.1.0 (2025-02-11)" --%%version %%dashdate

-- Required for any kpathsea calls to work.
kpse.set_program_name("texlua", "extractbb")

-- Required to use the "img" module from texlua, but only works for LuaTeX
-- versions >= 1.21.0.
if not (status.development_id >= 7661) then
    error("LuaTeX version is too old, cannot proceed.")
end
texconfig.texlua_img = true

-- We need to set \outputmode to PDF to be able to use most of the "img" module
-- functions, but to set \outputmode, we need to initialize the TeX interpreter.
tex.initialize()
_G.tex = package.loaded.tex
tex.enableprimitives("", tex.extraprimitives())
tex.outputmode = 1
tex.interactionmode = 0

-- "pdf" module
_G.pdf = package.loaded.pdf
pdf.setignoreunknownimages(1)
pdf.setmajorversion(2)
pdf.setminorversion(0)


------------------
--- Sandboxing ---
------------------

-- Prepare the sandbox for the rest of the script.
local env = {
    arg      = arg,
    io       = { stdout = io.stdout, },
    ipairs   = ipairs,
    math     = math,
    os       = { date = os.date, exit = os.exit, },
    pairs    = pairs,
    pdfe     = pdfe,
    print    = print,
    select   = select,
    table    = table,
    tonumber = tonumber,
    type     = type,
}

do
    -- Saved global functions
    local debug_traceback  = debug.traceback
    local find_file        = kpse.find_file
    local img_scan         = img.scan
    local io_open          = io.open
    local io_stderr        = io.stderr
    local kpse_in_name_ok  = kpse.in_name_ok
    local kpse_out_name_ok = kpse.out_name_ok
    local kpse_var_value   = kpse.var_value
    local lfs_attributes   = lfs.attributes
    local os_exit          = os.exit
    local os_setenv        = os.setenv
    local pdfe_open        = pdfe.open
    local select           = select
    local tostring         = tostring

    -- Error messages
    local function error(...)
        if show_errors then
            -- Header
            io_stderr:write("! extractbb ERROR: ")

            -- Message
            for i = 1, select("#", ...) do
                io_stderr:write(tostring(select(i, ...)), " ")
            end

            -- Traceback
            io_stderr:write("\n", "\n")
            io_stderr:write(debug_traceback(nil, 2), "\n")
        end

        -- Flush and exit
        io_stderr:flush()
        os_exit(1)
    end

    env.error = error

    -- Make sure that "openin_any" is at least "restricted", and that
    -- "openout_any" is at least "paranoid".
    local initial_openin  = kpse_var_value("openin_any")
    local initial_openout = kpse_var_value("openout_any")

    if (initial_openin ~= "r") or (initial_openout ~= "p") then
        os_setenv("openin_any",  "r")
    end

    if (initial_openout ~= "p") then
        os_setenv("openout_any", "p")
    end

    -- Check the input paths.
    local function resolve_input_name(file_name)
        local file_path = find_file(file_name, "graphic/figure", true)
        if not file_path then
            error("Cannot find input file:", file_name)
        end

        local allowed = kpse_in_name_ok(file_path)
        if not allowed then
            error("Input file is not allowed:", file_path)
        end

        local mode = lfs_attributes(file_path, "mode")
        if mode ~= "file" then
            error("Input file is not a regular file:", file_path)
        end

        return file_path
    end

    -- Check the output paths.
    local function resolve_output_name(file_name)
        local allowed = kpse_out_name_ok(file_name)
        if not allowed then
            error("Output file is not allowed:", file_name)
        end

        local name, extension = file_name:match("(.+)%.([^.]-)$")

        if (not name) or (not extension) or
           (name == "") or (extension == "")
        then
            error("Output file has no extension:", file_name)
        end

        if (extension ~= "xbb") and (extension ~= "bb") then
            error("Output file has an invalid extension:", file_name)
        end

        -- We shouldn't allow files with weird characters in their names.
        if name:match("[%c%%\t\r\n><*|]") then
            error("Output file has an invalid name:", file_name)
        end

        return file_name
    end

    -- Opens a file.
    function env.open_file(file_name, read_write, binary_text)
        local file_path, mode
        if read_write == "read" then
            file_path = resolve_input_name(file_name)
            mode = "r"
        elseif read_write == "write" then
            file_path = resolve_output_name(file_name)
            mode = "w"
        else
            error("Invalid read/write mode:", read_write)
        end

        if binary_text == "binary" then
            mode = mode .. "b"
        elseif binary_text == "text" then
            mode = mode .. ""
        else
            error("Invalid binary/text mode:", binary_text)
        end

        local file, message = io_open(file_path, mode)

        if not file then
            error("Cannot open file:", file_path, message)
        end

        return file
    end

    -- Open an PDF file.
    function env.pdfe.open(file_name)
        local file_path = resolve_input_name(file_name)
        return pdfe_open(file_path)
    end

    -- Open an image file.
    function env.open_image(file_name, page, box)
        local file_path = resolve_input_name(file_name)
        return img_scan {
            filename = file_path,
            filepath = file_path,
            page     = page,
            pagebox  = box,
        }
    end

    if not img_scan then
        env.open_image = false
    end
end

-- Prevent trying to change the environment.
local function bad_index(...)
    env.error("Attempt to access an undefined index:", select(2, ...))
end

setmetatable(env, {
    __index     = bad_index,
    __metatable = false,
    __newindex  = bad_index,
})

-- Set the environment.
_ENV = env


-----------------------------------
--- Post-Sandbox Initialization ---
-----------------------------------

-- Constants
local BP_TO_SP    = 65781.76
local IN_TO_BP    = 72
local DATE_FORMAT = "%a %b %d %H:%M:%S %Y" -- "%c"

-- Save often-used globals for a slight speed boost.
local floor            = math.floor
local insert           = table.insert
local remove           = table.remove
local script_arguments = arg
local unpack           = table.unpack

-- General-purpose functions
local function round(number)
    return floor(number +0.5)
end


-------------------------
--- Argument Handling ---
-------------------------

-- Define the argument handling functions.
local process_arguments = {}

-- > Specify a PDF pagebox for bounding box
-- > pagebox=cropbox, mediabox, artbox, trimbox, bleedbox
local bbox_option = "auto"
function process_arguments.B(script_arguments)
    bbox_option = remove(script_arguments, 1)
end

-- > Show this help message and exit
function process_arguments.h(script_arguments)
    print [[
Usage: extractbb [-B pagebox] [-p page] [-q|-v] [-O] [-m|-x] FILE...
       extractbb --help|--version
Extract bounding box from PDF, PNG, JPEG, JP2, or BMP file; default output below.

Options:
  -B pagebox    Specify a PDF pagebox for bounding box
                pagebox=cropbox, mediabox, artbox, trimbox, bleedbox
  -h | --help   Show this help message and exit
  --version     Output version information and exit
  -p page       Specify a PDF page to extract bounding box
  -q            Be quiet
  -v            Be verbose
  -O            Write output to stdout
  -m            Output .bb  file used in DVIPDFM (default)
  -x            Output .xbb file used in DVIPDFMx
]]
    os.exit(0)
end

process_arguments["-help"] = process_arguments.h

-- > Output version information and exit
function process_arguments.V(script_arguments)
    print(version)
    os.exit(0)
end

process_arguments["-version"] = process_arguments.V

-- > Specify a PDF page to extract bounding box
local page_number = 1
function process_arguments.p(script_arguments)
    page_number = tonumber(remove(script_arguments, 1))
end

-- > Be quiet
function process_arguments.q(script_arguments)
    show_errors = false
end

-- > Be verbose
function process_arguments.v(script_arguments)
    show_errors = true
end

-- > Write output to stdout
local output_file
function process_arguments.O(script_arguments)
    output_file = io.stdout
end

-- Output format
local output_format = "xbb"

if script_arguments[0]:match("ebb") then
    output_format = "bb"
end

-- > Output .bb  file used in DVIPDFM (default)
function process_arguments.m(script_arguments)
    output_format = "bb"
end

-- > Output .xbb file used in DVIPDFMx
function process_arguments.x(script_arguments)
    output_format = "xbb"
end

-- Get the input file name.
local input_name
function process_arguments.i(script_arguments)
    input_name = remove(script_arguments, 1)
end

process_arguments["-input-name"] = process_arguments.i

-- Clear the interpreter and script names.
script_arguments[-1] = nil
script_arguments[0]  = nil

-- Process the arguments.
while script_arguments[1] do
    -- Get the next argument.
    local arg = remove(script_arguments, 1)
    local cmd = arg:match("^%-(.*)$")

    -- Default to "--input-name" if no command is given.
    if not cmd then
        insert(script_arguments, 1, arg)
        cmd = "-input-name"
    end

    -- Handle multi-character arguments.
    if (cmd:len() >= 2) and (not cmd:match("^%-")) then
        local i = 0
        for char in cmd:gmatch(".") do
            i = i + 1
            insert(script_arguments, i, "-" .. char)
        end

        goto continue
    end

    -- Get the function to process the argument and run it.
    local func = process_arguments[cmd]

    if not func then
        error("Invalid argument:", arg)
    end

    func(script_arguments)

    ::continue::
end

-- Validate the arguments.
if not type(page_number) == "number" then
    error("Invalid page number:", page_number)
end

if not input_name then
    error("No input file specified.")
end

-- Validate the bounding box type. We need this rather crazy fallback scheme
-- to match the behaviour of "extractbb".
local bbox_orders = {}
bbox_orders.mediabox = {
    { img = "media", pdfe = "MediaBox" },
}
bbox_orders.cropbox = {
    { img = "crop", pdfe = "CropBox" }, unpack(bbox_orders.mediabox)
}
bbox_orders.artbox = {
    { img = "art", pdfe = "ArtBox" }, unpack(bbox_orders.cropbox)
}
bbox_orders.trimbox = {
    { img = "trim", pdfe = "TrimBox" }, unpack(bbox_orders.artbox)
}
bbox_orders.bleedbox = {
    { img = "bleed", pdfe = "BleedBox" }, unpack(bbox_orders.trimbox)
}
bbox_orders.auto = {
    bbox_orders.cropbox[1], bbox_orders.artbox[1], bbox_orders.trimbox[1],
    bbox_orders.bleedbox[1], bbox_orders.mediabox[1],
}

local bbox_order = bbox_orders[bbox_option]

if not bbox_order then
    error("Invalid PDF box type:", bbox_option)
end

-- Set the default pixel resolution.
local default_dpi
if output_format == "xbb" then
    default_dpi = 72
elseif output_format == "bb" then
    default_dpi = 100
else
    error("Invalid output format:", output_format)
end

-- Open the output file.
if not output_file then
    local base_name   = input_name:match("(.+)%.([^.]-)$") or input_name
    local output_name = base_name .. "." .. output_format
    output_file = open_file(output_name, "write", "text")
end


------------------------
--- Image Processing ---
------------------------

local x_min, y_min, x_max, y_max
local num_pages, image_type
local pdf_major_version, pdf_minor_version

if open_image then
    -- Check the number of pages.
    local image = open_image(input_name)
    num_pages = image.pages

    if page_number > num_pages then
        error("Invalid page number:", page_number)
    end

    -- Open the image to the specified page and bounding box. If the requested
    -- bounding box is not available, LuaTeX will fall back to the crop box
    -- or the media box.
    image = open_image(input_name, page_number, bbox_order[1].img)

    if not image then
        error("Cannot open image:", input_name)
    end

    -- Get the image metadata.
    image_type   = image.imagetype
    local bounding_box = image.bbox

    if not bounding_box then
        error("Cannot get bounding box:", page_number)
    end

    local x_resolution = image.xres
    local y_resolution = image.yres

    if (x_resolution or 0) == 0 then
        x_resolution = default_dpi
    end

    if (y_resolution or 0) == 0 then
        y_resolution = default_dpi
    end

    -- Convert the bounding box to PostScript points.
    for i, dimen in ipairs(bounding_box) do
        if image_type == "pdf" then
            dimen = dimen / BP_TO_SP
        else
            if i % 2 == 1 then
                dimen = dimen / x_resolution * IN_TO_BP
            else
                dimen = dimen / y_resolution * IN_TO_BP
            end
        end

        bounding_box[i] = dimen
    end

    -- Save the bounding box.
    x_min, y_min, x_max, y_max = unpack(bounding_box)

    -- We can't get the PDF version with the "img" library, so we'll just
    -- pretend that it's v1.5 (which supports most features).
    pdf_major_version = 1
    pdf_minor_version = 5
else
    -- Fallback to PDFs only.
    image_type = "pdf"
    local document = pdfe.open(input_name)

    if pdfe.getstatus(document) ~= 0 then
        error("Cannot open PDF file:", input_name)
    end

    -- Check the number of pages.
    num_pages = pdfe.getnofpages(document)

    if type(num_pages) ~= "number" then
        error("Invalid number of pages:", num_pages)
    end

    if page_number > num_pages then
        error("Invalid page number:", page_number)
    end

    -- Get the page.
    local page = pdfe.getpage(document, page_number)

    if not page then
        error("Cannot get page:", page_number)
    end

    -- Get the bounding box. Here, we check the boxes in the exact same order
    -- that "extractbb" does.
    local bounding_box
    for _, bbox in ipairs(bbox_order) do
        bounding_box = pdfe.getbox(page, bbox.pdfe)

        if bounding_box then
            break
        end
    end

    if not bounding_box then
        error("Cannot get bounding box:", page_number)
    end

    -- Save the bounding box.
    x_min, y_min, x_max, y_max = unpack(bounding_box)

    -- Get the PDF version.
    pdf_major_version, pdf_minor_version = pdfe.getversion(document)
end

-- Validate the bounding box.
for _, dimen in ipairs { x_min, y_min, x_max, y_max } do
    if type(dimen) ~= "number" then
        error("Invalid bounding box:", x_min, y_min, x_max, y_max)
    end
end


--------------
--- Output ---
--------------

-- Get the output fields and values.
local lines = {}

insert(lines, ("Title: %s"):format(input_name))
insert(lines, ("Creator: %s"):format(version))
insert(lines,
       ("BoundingBox: %d %d %d %d")
       :format(round(x_min), round(y_min), round(x_max), round(y_max)))

if output_format == "xbb" then
    insert(lines,
           ("HiResBoundingBox: %0.6f %0.6f %0.6f %0.6f")
           :format(x_min, y_min, x_max, y_max))

    if image_type == "pdf" then
        insert(lines,
               ("PDFVersion: %d.%d")
               :format(pdf_major_version, pdf_minor_version))

        insert(lines, ("Pages: %d"):format(num_pages))
    end

end

insert(lines, ("CreationDate: %s"):format(os.date(DATE_FORMAT, SOURCE_DATE_EPOCH)))

-- Create the output text.
local begin_line = "%%"
local end_line   = "\n"

local text = begin_line ..
             table.concat(lines, end_line .. begin_line) ..
             end_line .. end_line

-- Write the output text.
output_file:write(text)
output_file:close()

-- Everything is done, so now we can exit.
os.exit(0)
