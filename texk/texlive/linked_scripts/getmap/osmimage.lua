#!/usr/bin/env texlua
--
-- osmimage [options]
--
-- downloads an OpenStreetMap map specified by [options]
-- by using http://developer.mapquest.com/ web service
--
-- recoding the osmimage bash script to lua was mostly done
-- by Norbert Preining with valuable inputs from
-- Taco Hoekwater, Reinhard Kotucha and Heiko Oberdiek
-- Message-ID: <537A098C.8000902@gmx.de> ff
--
-- License: LPPL
--
local http = require("socket.http");
local ltn12 = require("ltn12")
local url = require("socket.url")

local URL = "http://open.mapquestapi.com/staticmap/v4/getplacemap"
local KEY = ""
local LOCATION = ""
local XSIZE = ""
local YSIZE = ""
local SIZE = ""
local SCALE = ""
local ZOOM = ""
local TYPE = ""
local IMAGETYPE = ""
local COLOR = ""
local NUMBER = ""
local OFILE = "osmimage"
local QUIET = "false"
local VERSION = "v1.2 (23/05/2014)"

function pversion()
  print("osmimage.lua " .. VERSION)
  print("(C) Josef Kleber   License: LPPL")
  os.exit(0)
end

function phelp()
  print([[
osmimage.lua [options]

 downloads an OpenStreetMap map specified by [options]
 by using http://developer.mapquest.com/ web service

 Options:

 -k  key registered at http://developer.mapquest.com/
     default (Example key from web site)!
     Please register and use your own key!

 -l  specify a location
     e.g. 'Bergheimer Straße 110A, 69115 Heidelberg, Germany'

 -x  specify a xsize (800)

 -y  specify a ysize (400)

 -S  short form to specify a size, e.g. 800,400

 -s  specify a scale factor in the range 1692-221871572 (3385 = -z 17)
     see: http://open.mapquestapi.com/staticmap/zoomToScale.html

 -z  specify a zoom in the range 1-18 (zoom overrides scale!)

 -t  specify map type {map|sat|hyb} (map)

 -i  specify image type {jpeg|jpg|gif|png} (png)

 -c  specify icon color (yellow_1)
     see: http://open.mapquestapi.com/staticmap/icons.html

 -n  specify the icon number (1)

 -o  specify output basename without file extension (osmimage.IMAGETYPE)

 -q  quiet; no output!

 -v  prints version information

 -h  prints help information

]])
  pversion()
end

function osmimage_error(exitcode, errortext)
  io.stderr:write ("Error (" .. exitcode .. "): " .. errortext .. "\n")
  os.exit(exitcode)
end

function osmimage_warning(warningtext)
  io.stderr:write("WARNING: " .. warningtext .. "\n")
end

function check_number(var, varname)
  local number='^[0-9]+$'
  if not(string.match(var, '^[0-9]+$')) then
    osmimage_error(2, varname .. " can't be " .. var .. "! Not a number!")
  end
end

function check_range(var,min,max,exitcode,varname)
  check_number(var,varname)
  if (tonumber(var) < tonumber(min) or tonumber(var) > tonumber(max)) then
    osmimage_error(exitcode, varname .. " = " .. var .. "; must be in the range of " .. min .. "-" .. max)
  end
end

do
  local newarg = {}
  local i, limit = 1, #arg
  while (i <= limit) do
    if arg[i] == "-k" then
      KEY = arg[i+1]
      i = i + 1
    elseif arg[i] == "-l" then
      LOCATION = arg[i+1]
      i = i + 1
    elseif arg[i] == "-x" then
      XSIZE = arg[i+1]
      i = i + 1
    elseif arg[i] == "-y" then
      YSIZE = arg[i+1]
      i = i + 1
    elseif arg[i] == "-S" then
      SIZE = arg[i+1]
      i = i + 1
    elseif arg[i] == "-s" then
      SCALE = arg[i+1]
      i = i + 1
    elseif arg[i] == "-z" then
      ZOOM = arg[i+1]
      i = i + 1
    elseif arg[i] == "-t" then
      TYPE = arg[i+1]
      i = i + 1
    elseif arg[i] == "-i" then
      IMAGETYPE = arg[i+1]
      i = i + 1
    elseif arg[i] == "-c" then
      COLOR = arg[i+1]
      i = i + 1
    elseif arg[i] == "-n" then
      NUMBER = arg[i+1]
      i = i + 1
    elseif arg[i] == "-o" then
      OFILE = arg[i+1]
      i = i + 1
    elseif arg[i] == "-q" then
      QUIET = 1
    elseif arg[i] == "-v" then
      pversion()
    elseif arg[i] == "-h" then
      phelp()
    else
      newarg[#newarg+1] = arg[i]
    end
    i = i + 1
  end
  arg = newarg
end

if QUIET == 1 then
  osmimage_warning("-q option currently not supported!")
end

if KEY == "" then
  -- KEY="Kmjtd%7Cluu7n162n1%2C22%3Do5-h61wh"
  KEY="Kmjtd|luu7n162n1,22=o5-h61wh"
  osmimage_warning("KEY not specified; using mapquest example key as default!")
end

if LOCATION == "" then
  LOCATION = "Bergheimer Straße 110A, 69115 Heidelberg, Germany"
  osmimage_warning("LOCATION not specified; using Dante e.V. Office as default!")
end

if XSIZE == "" then
  XSIZE=800
  osmimage_warning("XSIZE not specified; using XSIZE=800 as default!")
end

check_range(XSIZE,1,3840,11,"XSIZE")

if YSIZE == "" then
  YSIZE=400
  osmimage_warning("YSIZE not specified; using YSIZE=400 as default!")
end
check_range(YSIZE,1,3840,12,"YSIZE")

if SIZE == "" then
  SIZE = XSIZE .. "," .. YSIZE
end

if SCALE == "" then
  if ZOOM == "" then
    SCALE=3385
    osmimage_warning("SCALE not specified, using SCALE=3385 as default!")
  end
else
  check_range(SCALE,1692,221871572,13,"SCALE")
end

if TYPE == "" then
  TYPE = "map"
  osmimage_warning("TYPE not specified; using map as default!")
end

if IMAGETYPE == "" then
  IMAGETYPE="png"
  osmimage_warning("IMAGETYPE not specified; using png as default!")
end

if COLOR == "" then
  COLOR="yellow_1"
  osmimage_warning("COLOR not specified; using yellow_1 as default!")
end

if NUMBER == "" then
  NUMBER=1
  osmimage_warning("NUMBER not specified; using 1 as default!")
end
check_number(NUMBER,"NUMBER")

local UKEY = "?key=" .. url.escape(KEY)
local ULOCATION = "&location=" .. url.escape(LOCATION)
local USIZE = "&size=" .. url.escape(SIZE)
local USCALEZOOM
if ZOOM == "" then
  USCALEZOOM = "&scale=" .. url.escape(SCALE)
else
  check_range(ZOOM,1,18,14,"ZOOM")
  USCALEZOOM = "&zoom=" .. url.escape(ZOOM)
end
local UTYPE = "&type=" .. url.escape(TYPE)
local UIMAGETYPE = "&imagetype=" .. url.escape(IMAGETYPE)
local USHOWICON = "&showicon=" .. url.escape(COLOR) .. "-" .. url.escape(NUMBER)
local UOFILE = OFILE .. "." .. IMAGETYPE

local IMGURL = URL .. UKEY .. ULOCATION .. USIZE .. USCALEZOOM .. UTYPE .. UIMAGETYPE .. USHOWICON

local ret, msg
local ofile
ofile, msg = io.open(UOFILE, "wb")
if not ofile then
  osmimage_error(21, msg)
end
print("\n\nosmimage.lua:")
print("url = " .. IMGURL)
print("output = " .. UOFILE)
ret, msg = http.request{
  url = IMGURL,
  sink = ltn12.sink.file(ofile)
}
if not ret then
  osmimage_error(22, msg)
end

