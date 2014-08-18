#!/usr/bin/env texlua
--
-- getmapdl [options]
--
-- downloads an OpenStreetMap, Google Maps or Google Street View map
-- specified by [options] or parses gpx or gps files to create encoded polylines
--
-- License: LPPL
--
local http = require("socket.http");
local ltn12 = require("ltn12")
local url = require("socket.url")

local OSMURL = "http://open.mapquestapi.com/staticmap/v4/getplacemap"
local GMURL = "http://maps.googleapis.com/maps/api/staticmap"
local GSVURL = "http://maps.googleapis.com/maps/api/streetview"
local KEY = ""
local MODE = ""
local LOCATION = ""
local XSIZE = ""
local CENTER = ""
local YSIZE = ""
local SIZE = ""
local SCALE = ""
local ZOOM = ""
local TYPE = ""
local IMAGETYPE = ""
local COLOR = ""
local NUMBER = ""
local VISIBLE = ""
local IPATH = ""
local FPATH = ""
local MARKERS = ""
local HEADING = ""
local FOV = ""
local PITCH = ""
local LANGUAGE = ""
local GPFILE = ""
local OFILE = "getmap"
local QUIET = "false"
local VERSION = "v1.3 (16/08/2014)"

function pversion()
  print("getmapdl.lua " .. VERSION)
  print("(C) Josef Kleber   License: LPPL")
  os.exit(0)
end

function phelp()
  print([[
getmapdl.lua [options]

 downloads an OpenStreetMap, Google Maps or Google Street View map
 specified by [options] or parses gpx or gps files to create
 encoded polylines

 Options:

 -m specify the mode (osm|gm|gsv|gpx2epl|gps2epl|gpx2gps)

 -l  specify a location
     e.g. 'Bergheimer Straße 110A, 69115 Heidelberg, Germany'

 -x  specify a xsize (600)

 -y  specify a ysize (400)

 -S  short form to specify a size, e.g. 600,400 (osm) or 600x400 (gm)

 -s  specify a scale factor in the range 1692-221871572 (osm) or
     1-2 (gm)

 -z  specify a zoom in the range 1-18 (osm) or 0-21 (17) (gm)

 -t  specify map type {map|sathyb} (map) (osm) or
     {roadmap|satellite|hybrid|terrain} (roadmap) (gm)

 -i  specify image type {png|gif|jpg|jpeg} (png) (osm) or
     {png|png8|png32|gif|jpg|jpg-baseline} (png) (gm)

 -c  specify icon color (yelow_1) (osm) or (blue) (gm)
     see: http://open.mapquestapi.com/staticmap/icons.html
          https://developers.google.com/maps/documentation/staticmaps/#MarkerStyles

 -n  specify the icon number (1)

 -o  specify output basename without file extension (getmap.IMAGETYPE)

 -q  quiet; no output!

 -v  prints version information

 -h  prints help information

 gm mode only:

 -L  specify the language of map labels (xx language code (en,de,fr,...))

 -M  specify markers; see:
     https://developers.google.com/maps/documentation/staticmaps/index#Markers
     e.g.: &markers=size:mid|color:blue|label:1|address or more of these
     location and zoom will be ignored if used!

 -C  specify center of the map

 -V  specify a list of visible locations (loc1|loc2)

 -P  specify path from location to location
     e.g.: &path=weight:7|color:purple|loc1|loc2

 -p  specify a file holding the path specification
     (maybe needed for encoded polylines)

 gsv mode only:

 -H  specify heading (view) (0) (0 -- 360) (east: 90, ...)

 -T  specify the pitch (angle) (0) (-90 -- 90)

 -F  specify horizontal field of view (90) (0 -- 120)
     The field of view is expressed in degrees and a kind of zoom!

 gpx2epl, gps2epl and gpx2gps mode only:

 -G  specify the gpx or gps file

]])
  pversion()
end

function getmap_error(exitcode, errortext)
  io.stderr:write ("Error (" .. exitcode .. "): " .. errortext .. "\n")
  os.exit(exitcode)
end

function getmap_warning(warningtext)
  io.stderr:write("WARNING: " .. warningtext .. "\n")
end

function check_number(var, varname)
  if not(string.match(var, '^[-]?[0-9]+$')) then
    getmap_error(2, varname .. " can't be " .. var .. "! Not a number!")
  end
end

function check_range(var,min,max,exitcode,varname)
  check_number(var,varname)
  if (tonumber(var) < tonumber(min) or tonumber(var) > tonumber(max)) then
    getmap_error(exitcode, varname .. " = " .. var .. "; must be in the range of " .. min .. "-" .. max)
  end
end

function round(number, precision)
   return math.floor(number*math.pow(10,precision)+0.5) / math.pow(10,precision)
end

function encodeNumber(number)
  local num = number
  num = num * 2
  if num < 0
  then
    num = (num * -1) - 1
  end
  local t = {}
  while num >= 32
  do
    local num2 = 32 + (num % 32) + 63
    table.insert(t,string.char(num2))
    num = math.floor(num / 32) -- use floor to keep integer portion only
  end
  table.insert(t,string.char(num + 63))
  return table.concat(t)
end

do
  local newarg = {}
  local i, limit = 1, #arg
  while (i <= limit) do
    if arg[i] == "-l" then
      LOCATION = arg[i+1]
      i = i + 1
    elseif arg[i] == "-C" then
      CENTER = arg[i+1]
      i = i + 1
    elseif arg[i] == "-m" then
      MODE = arg[i+1]
      i = i + 1
    elseif arg[i] == "-k" then
      KEY = arg[i+1]
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
    elseif arg[i] == "-L" then
      LANGUAGE = arg[i+1]
      i = i + 1
    elseif arg[i] == "-M" then
      MARKERS = arg[i+1]
      i = i + 1
    elseif arg[i] == "-H" then
      HEADING = arg[i+1]
      i = i + 1
    elseif arg[i] == "-T" then
      PITCH = arg[i+1]
      i = i + 1
    elseif arg[i] == "-F" then
      FOV = arg[i+1]
      i = i + 1
    elseif arg[i] == "-V" then
      VISIBLE = arg[i+1]
      i = i + 1
    elseif arg[i] == "-P" then
      IPATH = arg[i+1]
      i = i + 1
    elseif arg[i] == "-p" then
      FPATH = arg[i+1]
      i = i + 1
    elseif arg[i] == "-G" then
      GPFILE = arg[i+1]
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
  getmap_warning("-q option currently not supported!")
end

if MODE == "gpx2epl" then
  local file = io.open(GPFILE, "r")
  local Olatitude = 0
  local Olongitude = 0
  local epl = {}

  io.input(file)
  while true do
    local line = io.read()
    if line == nil
    then
      break
    end
    if string.match(line, "trkpt") then
      local latitude
      local longitude
      local encnum
      latitude = string.match(line, 'lat="(.-)"')
      longitude = string.match(line, 'lon="(.-)"')
      latitude = round(latitude,5)*100000
      longitude = round(longitude,5)*100000
      encnum = encodeNumber(latitude - Olatitude)
      table.insert(epl,encnum)
      encnum = encodeNumber(longitude - Olongitude)
      table.insert(epl,encnum)
      Olatitude = latitude
      Olongitude = longitude
    end
  end
  local string = table.concat(epl)
  -- sometimes the sting contains unwanted control characters
  local stingwithoutcontrolcharacters = string:gsub("%c", "")
  print(stingwithoutcontrolcharacters)
  os.exit(0)
end

if MODE == "gpx2gps" then
  local file = io.open(GPFILE, "r")
  io.input(file)
  while true do
    local line = io.read()
    if line == nil
    then
      break
    end
    if string.match(line, "trkpt") then
      local latitude
      local longitude
      latitude = string.match(line, 'lat="(.-)"')
      longitude = string.match(line, 'lon="(.-)"')
      print(latitude .. "," .. longitude)
    end
  end
  os.exit(0)
end

if MODE == "gps2epl" then
  local file = io.open(GPFILE, "r")
  local Olatitude = 0
  local Olongitude = 0
  local epl = {}

  io.input(file)
  while true do
    local line = io.read()
    if line == nil
    then
      break
    end
    local latitude
    local longitude
    local encnum
    latitude, longitude = line:match("([^,]+),([^,]+)")
    latitude = round(latitude,5)*100000
    longitude = round(longitude,5)*100000
    encnum = encodeNumber(latitude - Olatitude)
    table.insert(epl,encnum)
    encnum = encodeNumber(longitude - Olongitude)
    table.insert(epl,encnum)
    Olatitude = latitude
    Olongitude = longitude
  end
  local string = table.concat(epl)
  -- sometimes the sting contains unwanted control characters
  local stingwithoutcontrolcharacters = string:gsub("%c", "")
  print(stingwithoutcontrolcharacters)
  os.exit(0)
end

print("\n")

if KEY == "" then
  if MODE == "osm" then
    KEY="Kmjtd|luu7n162n1,22=o5-h61wh"
    getmap_warning("KEY not specified; using mapquest example key as default!")
  end
end

if LOCATION == "" then
  LOCATION = "Bergheimer Straße 110A, 69115 Heidelberg, Germany"
  getmap_warning("LOCATION not specified; using Dante e.V. Office as default!")
end

if MODE == "gm" then
  if ZOOM == "" then
    ZOOM=17
    getmap_warning("ZOOM not specified; using ZOOM=17 as default!")
  end
end

if XSIZE == "" then
  XSIZE=600
  getmap_warning("XSIZE not specified; using XSIZE=600 as default!")
end

if YSIZE == "" then
  YSIZE=400
  getmap_warning("YSIZE not specified; using YSIZE=400 as default!")
end

if SIZE == "" then
  if MODE == "gm" then
    SIZE = XSIZE .. "x" .. YSIZE
  elseif MODE == "gsv" then
    SIZE = XSIZE .. "x" .. YSIZE
  elseif MODE == "osm" then
    SIZE = XSIZE .. "," .. YSIZE
  end
end

if SCALE == "" then
  if MODE == "gm" then
    SCALE=1
    getmap_warning("SCALE not specified, using SCALE=1 as default!")
  elseif MODE == "osm" then
    if ZOOM == "" then
      SCALE=3385
      getmap_warning("SCALE not specified, using SCALE=3385 as default!")
    end
  end
end

if TYPE == "" then
  if MODE == "gm" then
    TYPE = "roadmap"
    getmap_warning("TYPE not specified; using roadmap as default!")
  elseif MODE == "osm" then
    TYPE = "map"
    getmap_warning("TYPE not specified; using map as default!")
  end
end

if IMAGETYPE == "" then
  if MODE == "gsv" then
  else
    IMAGETYPE="png"
    getmap_warning("IMAGETYPE not specified; using png as default!")
  end
end

if COLOR == "" then
  if MODE == "gm" then
    COLOR="blue"
    getmap_warning("COLOR not specified; using blue as default!")
  elseif MODE == "osm" then
    COLOR="yellow_1"
    getmap_warning("COLOR not specified; using yellow_1 as default!")
  end
end

if NUMBER == "" then
  if MODE == "gsv" then
  else
    NUMBER=1
    getmap_warning("NUMBER not specified; using 1 as default!")
  end
end

if MODE == "gsv" then
  if HEADING == "" then
    HEADING=0
    getmap_warning("HEADING not specified; using 0 as default!")
  end

  if FOV == "" then
    FOV=90
    getmap_warning("FOV not specified; using 90 as default!")
  end

  if PITCH == "" then
    PITCH=0
    getmap_warning("PITCH not specified; using 0 as default!")
  end
end

if MODE == "gm" then
  if ZOOM == "" then
    ZOOM = 17
  else
    check_range(ZOOM,0,21,11,"ZOOM")
  end
  check_range(XSIZE,1,640,12,"XSIZE")
  check_range(YSIZE,1,640,13,"YSIZE")
  check_range(SCALE,1,2,14,"SCALE")
elseif MODE == "gsv" then
  check_range(XSIZE,1,640,12,"XSIZE")
  check_range(YSIZE,1,640,13,"YSIZE")
  check_range(HEADING,0,360,15,"HEADING")
  check_range(FOV,0,120,16,"FOV")
  check_range(PITCH,-90,90,17,"PITCH")
elseif MODE == "osm" then
  check_range(XSIZE,1,3840,11,"XSIZE")
  check_range(YSIZE,1,3840,12,"YSIZE")
  if ZOOM == "" then
    check_range(SCALE,1692,221871572,13,"SCALE")
  else
    check_range(ZOOM,1,18,14,"ZOOM")
  end
  check_number(NUMBER,"NUMBER")
end

local UKEY = ""
local ULOCATION = ""
local UZOOM = ""
local USCALEZOOM = ""
local UMARKERS = ""
local USIZE = ""
local USCALE = ""
local UTYPE = ""
local USHOWICON = ""
local UIMAGETYPE = ""
local UVISIBLE = ""
local UIPATH = ""
local UFPATH = ""
local EPLFILE = ""
local UHEADING = ""
local UFOV = ""
local UPITCH = ""
local ULANGUAGE = ""
local UOFILE = ""
local IMGURL = ""

if MODE == "gm" then
  ULOCATION = "center=" .. url.escape(LOCATION)
  if MARKERS == "" then
    UMARKERS = "&markers=size:mid|color:" .. COLOR .. "|label:" .. NUMBER .. "|" .. url.escape(LOCATION)
    UZOOM = "&zoom=" .. url.escape(ZOOM)
  else
    UMARKERS = "" .. url.escape(MARKERS)
    -- correct cruft escaping of "&markers="
    UMARKERS = UMARKERS:gsub("%%26markers%%3d","&markers=")
    UZOMM = ""
    if CENTER == "" then
      ULOCATION = ""
    else
      ULOCATION = "center=" .. url.escape(CENTER)
    end
  end
  USIZE = "&size=" .. url.escape(SIZE)
  USCALE = "&scale=" .. url.escape(SCALE)
  UTYPE = "&maptype=" .. url.escape(TYPE)
  UIMAGETYPE = "&format=" .. url.escape(IMAGETYPE)
  if IMAGETYPE == "jpg-baseline" then
    IMAGETYPE = "jpg"
  end
  if VISIBLE == "" then
    UVISIBLE = ""
  else
    UVISIBLE = "&visible=" .. url.escape(VISIBLE)
  end
  if IPATH == "" then
    UIPATH = ""
  else
    UIPATH = "" .. url.escape(IPATH)
    -- correct cruft escaping of "&path="
    UIPATH = UIPATH:gsub("%%26path%%3d","&path=")
  end
  if FPATH == "" then
    UFPATH = ""
  else
    EPLFILE = io.open(FPATH, "r")
    local contents = EPLFILE:read()
    EPLFILE:close()
    UFPATH = "" .. url.escape(contents)
    -- correct cruft escaping of "&path="
    UFPATH = UFPATH:gsub("%%26path%%3d","&path=")
  end
  if LANGUAGE == "" then
    ULANGUAGE=""
  else
    ULANGUAGE="&language=" .. LANGUAGE
  end
  UOFILE = OFILE .. "." .. IMAGETYPE
  IMGURL = GMURL .. "?" .. ULOCATION .. USIZE .. UZOOM .. UMARKERS .. UTYPE .. USCALE .. UIMAGETYPE .. UVISIBLE .. UIPATH .. UFPATH .. ULANGUAGE .. "&sensor=false"
elseif MODE == "gsv" then
  ULOCATION = "location=" .. url.escape(LOCATION)
  USIZE = "&size=" .. url.escape(SIZE)
  UHEADING = "&heading=" .. url.escape(HEADING)
  UFOV = "&fov=" .. url.escape(FOV)
  UPITCH = "&pitch=" .. url.escape(PITCH)
  UOFILE = OFILE .. ".jpg"
  IMGURL = GSVURL .. "?" .. ULOCATION .. USIZE .. UHEADING .. UFOV .. UPITCH .. "&sensor=false"
elseif MODE == "osm" then
  UKEY = "?key=" .. url.escape(KEY)
  ULOCATION = "&location=" .. url.escape(LOCATION)
  USIZE = "&size=" .. url.escape(SIZE)
  if ZOOM == "" then
    USCALEZOOM = "&scale=" .. url.escape(SCALE)
  else
    USCALEZOOM = "&zoom=" .. url.escape(ZOOM)
  end
  UTYPE = "&type=" .. url.escape(TYPE)
  UIMAGETYPE = "&imagetype=" .. url.escape(IMAGETYPE)
  USHOWICON = "&showicon=" .. url.escape(COLOR) .. "-" .. url.escape(NUMBER)
  UOFILE = OFILE .. "." .. IMAGETYPE
  IMGURL = OSMURL .. UKEY .. ULOCATION .. USIZE .. USCALEZOOM .. UTYPE .. UIMAGETYPE .. USHOWICON
end

local ret, msg
local ofile
ofile, msg = io.open(UOFILE, "wb")
if not ofile then
  getmap_error(21, msg)
end
print("\n\ngetmapdl.lua:")
print("url = " .. IMGURL)
print("output = " .. UOFILE)
ret, msg = http.request{
  url = IMGURL,
  sink = ltn12.sink.file(ofile)
}
if not ret then
  getmap_error(22, msg)
end
os.exit(0)
