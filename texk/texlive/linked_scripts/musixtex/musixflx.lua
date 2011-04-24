#!/usr/bin/env texlua

-- The MUSIXFLXVERSION below is the one checked against.

MUSIXFLXVERSION = "0.83"
VERSION = "0.83.3.lua7" 

--[[

Line breaking program for MusiXTeX.

 (c) Copyright Ross Mitchell 1992-1997 ross.mitchell@csiro.au
 (c) Copyright J. Hunsberger 1997 jhunsberger@i2k.co
 (c) Copyright 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
 (c) Copyright 2011 Nikhil Helferty  6nh14@queensu.ca 
 (c) Copyright 2011 Bob Tennent rdt@cs.queensu.ca

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

--]]

--[[

Conversion to Lua by Nikhil Helferty, 2011. 
Minor corrections by Bob Tennent rdt@cs.queensu.ca.

NH: Comments marked NH (such as this one) are comments new to the Lua
version. Most have to do with explaining any additions or changes made
necessary due to differences between Lua and C. Comments not so marked
are retained from C version.

Almost all comments from the C version have been put in this version as
well. Note that Lua variables are dynamically typed, and Lua has only
one data structure called "tables" which, though more flexible than
arrays, are essentially just used as arrays here.

--]]

--[[  

  ChangeLog:

  lua7   2011/04/21   RDT
         Decode option argument for debugging.
         Revert to accepting .mx1 or .tex filenames.
         Rationalize file-name variables.

  lua6   2011/04/18   RDT
         Revert to stand-alone use.
 
  lua5   2011/04/13   RDT
         Allow .mx1 file to have a version number only.

  lua4   2011/04/12   RDT
         Adaptation to being called from musixtex.tex

  lua3   2011/04/11   RDT
         Replaced body of eightfivefloat by string.format("%8.5f", ... )

  lua2   2011/04/11   RDT
         Change PRINT to print for dbug output.

  lua1   2011/04/08   RDT
         Strip off pt for L line.
   
--]]

-- max sizes
-- NH: these aren't needed for array sizes in Lua (tables grow dynamically)
-- but I am keeping the checks in case the maximums are necessary for
-- TeX processing

MAX_SIGNS = 128; MAX_SECTIONS = 128; MAX_BARS = 2048

function GETLINE()
  linebuf = infile:read()
  return linebuf
end

-- NH: declare tables

-- NH: the following tables were all arrays of type int in C
-- the following tables were size MAX_BARS in C (2048)
zbar = {}; lr_repeat = {}; raggedline = {}
l_repeat = {}; barno = {}

-- NH: the following tables were size MAX_SECTIONS in C (128)
autolines = {}; bars = {}; mulooseness = {}

-- NH: the following was of size MAX_SIGNS in C (128)
signchange = {}

-- NH: the following were counters declared for xbar sign change detection
-- xbar of size MAX_BARS, linegoal MAX_SECTIONS
-- xbar_count, xbar_flag, softspace_count were integers declared here
-- as they aren't tables they'll be declared upon use
xbar = {}; linegoal = {}

-- NH: the following tables were all arrays of type double in C
-- the following were size MAX_BARS
hardbarlength = {}; softbarlength = {}
width_leftrightrepeat = {}; width_leftrepeat = {}

-- NH: the following were size MAX_SECTIONS
eff_hardlength = {}; eff_softlength = {}

-- NH: the following were size MAX_SIGNS
oldsignskip = {}; signskip = {}; futuresignskip = {}


-- NH: (no need to add last \n as print does this)
function error_exit (error_number)
  -- NH: the following if/elseif/else construction mirrors case structure

  if error_number == 0 then
    print("\nFile error.")
  elseif error_number == 1 then
    print("\nUsage: [texlua] musixflx.lua basename[.tex | .mx1] [ d | m | f | s ]")
  elseif error_number == 2 then
    print("\nThis shouldn't happen ! Too few bars or \\mulooseness too large ?")
  elseif error_number == 3 then
    print("\nThis shouldn't happen ! Too few bars in section !")
  elseif error_number == 4 then
    print("\nMissing endmark ! Forgotten \\stop[end]piece ?")
  elseif error_number == 5 then
    print("\nDivision by zero ! Ask a wizard !")
  elseif error_number == 6 then
    print("\nVersion mis-match:")
    print("MusiXTeX version " .. linebuf)
    print("musixflx expected version " .. MUSIXFLXVERSION)
  elseif error_number == 7 then
    print("\nFile not found: " .. infilename)
  elseif error_number == 8 then
    print("\nCorrupted " .. infilename)
  elseif error_number == 9 then
    print("\nToo many sections, maximum number of sections: " .. MAX_SECTIONS)
  elseif error_number == 10 then
    print("\nError in " .. infilename .. " at line " .. currentline)
  elseif error_number == 11 then
    print("\nToo many bars, maximum number of bars: " ..  MAX_BARS)
  else
    print("!!! Can't go on !!!")
  end
  os.exit(3)

end
-- end error_exit

-- NH: there is really no analogue to the strrchr function in C, which is frequently used
-- to locate the location of a last occurrence of a string
-- so I coded a similar function for Lua
-- takes two strings (str and pat), and returns everything after the last occurence of pat
-- in str - if no occurrences of pat, then returns nil
function strrchr(str, pat)
  notfound = true
  currentIndex = 0
  lastIndex = 0
  while notfound do
    currentIndex = string.find(str, pat, lastIndex+1, true)
    if (not currentIndex) then -- NH: returned nil - no occurence past lastIndex
      notfound = false
    else  -- NH: got a hit for an occurence - store this value as lastIndex
      lastIndex = currentIndex
    end
  end
  if lastIndex == 0 then return nil end -- NH: no occurence found
  return string.sub(str, lastIndex+1)
end


-- NH: start "main chunk"

io.write(string.format("Musixflx-%s", VERSION))

-- initialize variables
junk = -9999
dbug = false
dbug_lines = false
dbug_logfile = false
showresult = false
detectraggedline = false
currentline = 1
samechapter = true
line_number = 0
chapterno = 1

lthick = .4

if dbug then
  print("\n ... decoding command line")
end

filename = arg[1]
if not filename then
   error_exit(1)
end

extension = string.sub(filename, -4, -1)
if extension == ".tex" or extension == ".mx1" then
  basename = string.sub(filename, 1, -5)
else
  basename = filename
end

--[[

debugging options:
 d output debugging information to the screen
 m output line numbers to the screen 
 f output debugging information to file basename.mxl (not mx1)
 s output computed lines to the screen

 --]]

if (#arg == 2) then
  if (arg[2] == "d") then 
    dbug = true
  elseif (arg[2] == "m") then 
    dbug_lines = true
  elseif (arg[2] == "f") then
    dbug = true
    dbug_logfile = true
    dbug_lines = true
  elseif (arg[2] == "s") then
    showresult = true
  else
    error_exit(1)
  end
end

infilename = basename .. ".mx1"

-- open the .mx1 file containing bar length information

if dbug then
  print(" ... opening " .. infilename .. " for input")
-- NH: io.open analagous to fopen C function
-- will return nil value if not successful
-- nil is taken as false in if statements
end
infile = io.open(infilename, "r")
if (not infile) then 
  error_exit(7)
end
io.write(string.format(" (%s", infilename))
if showresult then
  print("")
end

currentline = currentline + 1
if GETLINE() and (linebuf ~= MUSIXFLXVERSION) then 
  error_exit(6)
end


-- Open the output file
outfilename = basename .. ".mx2"
if dbug then
  print(" ... opening " .. outfilename .. " for output")
end
outfile = io.open(outfilename, "w")
if (not outfile) then
  print("\nCan't create: " .. outfilename)
  os.exit(3)
end

-- Open the logfile
if dbug_logfile then
  logfilename = basename .. ".mxl"
  print(" ... open " .. logfilename .. " for debugging")
  logfile = io.open(logfilename, "w")
  if (not logfile) then
    print("\nCan't create: " .. logfilename)
    os.exit(3)
  end
  logfile:write("Version ", VERSION, "\n")
end

-- skip startindicator

if GETLINE() and (linebuf ~= "S") then
  error_exit(8)
end

--[[
do...while loop for
 moretimes call of \startpiece
 >>>>>>>>>>>>>>>>>>>>>>>>>>>

 Do while equivalent is repeat...until
 --]]

 GETLINE()

 repeat

-- reset all arrays

if dbug then print("\n------- Chapter " .. chapterno .. " -------\n"); end
if dbug_logfile then logfile:write("\n------- Chapter ", chapterno, " -------\n\n"); end
-- NH: copying C method of loops but this means will be initializing the max spaces every time
-- ... not taking advantage of dynamic sizes of tables in Lua
-- however necessary to initialize since the starting values are used in operations
-- note upper bounds for these loops are the max-1, as for syntax in lua will include the upper (<= instead of <)

for i = 0, MAX_SIGNS-1 do
  signchange[i] = junk
  oldsignskip[i] = 0
  futuresignskip[i] = 0 -- jh-1
  signskip[i] = 0
end

for i = 0, MAX_BARS-1 do
  hardbarlength[i] = 0
  softbarlength[i] = 0
  width_leftrightrepeat[i] = 0
  lr_repeat[i] = false
  width_leftrepeat[i] = 0
  xbar[i] = 0 -- jh-1
  l_repeat[i] = false
  zbar[i] = false
  raggedline[i] = false
  barno[i] = 0
end

for i = 0, MAX_SECTIONS-1 do
  eff_hardlength[i] = 0
  eff_softlength[i] = 0
  bars[i] = 0
  autolines[i] = false
  linegoal[i] = 0
  mulooseness[i] = 0
end

--[[

 Read and decode header items:

 1. Linewidth;
 2. Parindent;
 3. Beforeruleskip;
 4. Afterruleskip;
 5. Elemskip;
 6. Clefskip;
 7. Signskip;
 --]]

 -- NH: for those curious why it is necessary to take a substring of the line, it is to strip the "pt" off of the string
 -- to make them valid strings for coercion to numbers later on (something that the atof does automatically)
 -- RDT: linebuf tests are to avoid taking a substring of an empty string
if linebuf then linewidth = string.sub(linebuf, 1, -3); currentline=currentline+1 end
GETLINE(); if linebuf then parindent = string.sub(linebuf, 1, -3); currentline=currentline+1 end
GETLINE(); if linebuf then beforerule = string.sub(linebuf, 1, -3); currentline=currentline+1 end
GETLINE(); if linebuf then afterrule = string.sub(linebuf, 1, -3); currentline=currentline+1 end
GETLINE(); if linebuf then elemskip = string.sub(linebuf, 1, -3); currentline=currentline+1 end
GETLINE(); if linebuf then clefskip = string.sub(linebuf, 1, -3); currentline=currentline+1 end
GETLINE(); if linebuf then signskip[0] = string.sub(linebuf, 1, -3); currentline=currentline+1 end

futuresignskip[0] = signskip[0] -- Initialize for xbar signs, jh-1

--[[
 Read the records specifying contributions to bar length.
 Lengths are of two types:
 (a) Hard or unscaleable, eg barlines, clef or meter changes (typ=0).
 (b) Soft or scaleable, eg noteboxes which scale with \elemskip (typ=1).
 Note that \afterruleskip and \beforerulskip are considered soft.
 --]]

jbar = junk
i = 0
sign = 0
xbar_count = 0 -- Used to detect presence of xbars jh-1
xbar_flag = false -- True allows check for xbar jh-1
hardsign = 0 -- Accumulator for hard skip from sign change jh-1
softspace_count = 0 -- Watch for softspace after bars are posted jh-1

all_section = 0

if dbug then
  print(" ... reading")
end

while (samechapter and GETLINE()) do

  currentline = currentline + 1
  local firstChar = string.sub(linebuf, 1, 1)

  -- NH: use if/elseif/else to mirror switch/case statements

--[[
'\startpiece'
 stop reading,
 compute,
 write
 and start again
 --]]

  if (firstChar == "S") then
    samechapter = false

--[[
 End of section. Action:
 Right justify the material ending at the previous bar.
 Read the number following the *, which is the 'looseness'
 parameter of the section just ended.
 Reset the bar test integer to JUNK in case the bar number
 was reset between sections.
 --]]

  elseif (firstChar == "*") then
    local firstSpace = string.find(linebuf, " ")
    local secondSpace = string.find(linebuf, " ", firstSpace+1)
    if not secondSpace then -- NH: possibility that there is no second number in line
      mulooseness[all_section] = string.sub(linebuf, firstSpace+1)
    else
      mulooseness[all_section] = string.sub(linebuf, firstSpace+1, secondSpace-1)
    end
    -- NH: above firstSpace/secondSpace mathematics were to mimic function of atol use in original C function
    linegoal[all_section] = 0 -- jh-2 + WS-1
    if secondSpace then
      linegoal[all_section] = string.sub(linebuf, secondSpace+1) -- NH: this my interpretation of an sscanf line signed jh-2 + WS-1 in C
    end
    linegoal[all_section] = linegoal[all_section] + 0 -- NH: this is a fairly silly trick to force coercion for valid comparison to numbers, could also use tonumber function
    mulooseness[all_section] = mulooseness[all_section] + 0
    if ((mulooseness[all_section] ~= 0) and (linegoal[all_section] > 0)) then
      print("\\linegoal{" .. linegoal[all_section] .. "} ignored because \\mulooseness not equal to zero")
      print("   for section " .. all_section+1 .. " in chapter " .. chapterno)
      linegoal[all_section] = 0 -- Reset to ignore it. jh-2 + WS-1
    end
    all_section = all_section + 1
    if all_section > (MAX_SECTIONS - 1) then
      error_exit(9)
    end
    jbar=junk

--[[
 Right justify the material ending at the previous bar.
 Set flag.
 Reset the bar test integer to JUNK in case the bar number
 was reset between sections.
 ]]


  elseif (firstChar == "a") then
    autolines[all_section] = true
    all_section = all_section + 1
    if (all_section > (MAX_SECTIONS - 1)) then
      error_exit(9)
    end
    jbar=junk

--[[
 found a raggedline, let's set a flag
 I think, I'll hardly get a Nobel-Award for coding this
 What a pity! :-(
 but perhaps a Pulitzer-Award for my comments. :-)
--]]

  elseif (firstChar == "r") then
    raggedline[i+1] = true


  -- found \zbar, set flag and store barno
  elseif (firstChar == "z") then
    zbar[i] = true
    barno[i] = strrchr(linebuf, " ")
    xbar_count = xbar_count + 1 -- Help detect xbars, track zbar offsets. jh-1
    hardsign = 0 -- reset any sign change skip accumulated. jh-1
    softspace_count = 0 -- Reset for next bar. jh-1


--[[
 found a leftrightrepeat
 set a flag and store the different widths
 ]]

  elseif (firstChar == "l") then
    -- NH: there are some commented out printf lines in C code in this case
    -- I'm assuming they were used at one point for debugging purposes and
    -- am not translating them, but if should be reinstated they exist at lines
    -- 380-387 in C program
    lr_repeat[i] = true
    local firstSpace = string.find(linebuf, " ")
    local secondSpace = string.find(linebuf, " ", firstSpace + 1)
    width_leftrightrepeat[i] = string.sub(linebuf, firstSpace+1, secondSpace-3) -- NH: secondspace-3 as secondspace-1 will include the "pt"
    width_leftrepeat[i] = string.sub(linebuf, secondSpace+1, -3) -- NH: again, -3 to chop off the "pt"


--[[
 found a leftrepeat
 set a flag and store width
 ]]

  elseif (firstChar == "L") then
    l_repeat[i] = true
    local firstSpace = string.find(linebuf, " ")
    width_leftrepeat[i] = string.sub(linebuf, firstSpace+1, -3) -- RDT: again, -3 to chop off the "pt"


  -- store barno
  elseif (firstChar == "b") then
    barno[i] = strrchr(linebuf, " ")
    xbar_count = xbar_count+1 -- Track possible xbars. jh-1
    xbar_flag = true -- Allow check of next line to detect xbar. jh-1
    hardsign = 0 -- Reset accumulated hard sign skip. jh-1
    softspace_count = 0 -- Reset for next bar. jh-1

--[[
 enabling the use of 'hard' offsets
 advance current hardwith
 reduce current softwidth
--]]

  elseif (firstChar == "h") then
    x = string.sub(strrchr(linebuf, " "), 1, -3) -- -3 is to chop off "pt" in string to allow x to be valid for arithmetic
    softbarlength[i] = softbarlength[i] - x
    hardbarlength[i] = hardbarlength[i] + x
    eff_softlength[all_section] = eff_softlength[all_section] - x
    eff_hardlength[all_section] = eff_hardlength[all_section] + x


--[[
 This record began with 's' and specifies a key signature change
 store the signskip, s.b.
 ]]

  elseif (firstChar == "s") then -- Changes to detect xbar and signchange interaction, jh-1

    local firstSpace = string.find(linebuf, " ")
    local secondSpace = string.find(linebuf, " ", firstSpace+1)
    if not secondSpace then
      tempholdskip = string.sub(linebuf, firstSpace+1, -3)
    else
      tempholdskip = string.sub(linebuf, firstSpace+1, secondSpace-3)
    end
    -- NH: all of above to mimic atof statement
    -- wasn't sure if statements with s would have a second space and more info
    -- so accounted for either possibility

    -- We might be in the middle of an xbar setup, and we only want to increment
    -- the sign pointer if this is the first sign change. jh-1
    if (not (signchange[sign] == i)) then -- first time for this bar set jh-1
      sign = sign + 1
      signchange[sign] = i
      signskip[sign] = tempholdskip
      oldsignskip[sign] = hardsign -- Capture accumulated hard space. jh-1
      xbar[i] = xbar[i] + 1 -- Increment to detect xbars with sign changes. jh-1
      --[[
      Housekeeping is done... Now, one more condition to check. If there
      has been any soft space since the bar was declared, then musixtex
      will NOT publish a sign change notice at the end of the line for
      this sign change when there is a line break in this bar.
      In that case, signal that this should be treated as an xbar, which
      will effectively suppress the transfer of hardspace for the sign
      change notice. jh-1
      --]]
      if softspace_count > 0 then xbar[i] = 2 end-- Suppress space move jh-1
    end
    -- Always update the futuresignskip value in case this is the last
    futuresignskip[sign] = tempholdskip

  -- comment, do nothing
  elseif (firstChar == "%") then

  -- This is an 'ordinary' line, listing a contribution to the barlength.
  else
    if (not (tonumber(firstChar))) then
      error_exit(10)
    end
    local firstSpace = string.find(linebuf, " ")
    local secondSpace = string.find(linebuf, " ", firstSpace + 1)
    bar = string.sub(linebuf, 1, firstSpace-1)
    typ = string.sub(linebuf, firstSpace+1, secondSpace-1)
    x = string.sub(linebuf, secondSpace+1, -3) -- the -3 is to strip the "pt" off of the x
    if (typ ~= "0") then eff_softlength[all_section] = eff_softlength[all_section] + x
    else eff_hardlength[all_section] = eff_hardlength[all_section] + x
    end

--[[
 Increment bar number if the bar number
 read from the file has changed.
 Accumulate current bar length.
 --]]
    bar = bar + 0 -- NH: forcing coercion to number type from string
    if tonumber(bar) > jbar then
      i = i + 1
      bars[all_section] = bars[all_section] + 1
      if i > MAX_BARS then
        error_exit(11)
      end
    end

      -- At this point, can check if this is an xbar...Only check once. jh-1
    if xbar_flag then
      if ((xbar_count - bar - 1) == 0) then -- find an xbar. jh-1
        xbar_count = xbar_count-1 -- adjust offset to stay on track

        --[[
        To handle the special conditions caused by possible xbars and
        sign changes, the xbar logic state has to be examined and changed
        only if this is the first xbar, and not for subsequent ones. jh-1
        --]]
        if (xbar[i] == 0) then -- then this is the first xbar in the setup
          -- or there has already been a sign change
          xbar[i] = 1 -- jh-1
        end
      end
    end

    xbar_flag = false -- Reset to prevent checks until next bar. jh-1

    if (typ ~= "0")  then
      softbarlength[i] = softbarlength[i] + x
      -- Count softspace entries to help in sign/linebreak decisions jh-1
      softspace_count = softspace_count+1
      hardsign = 0 -- Safety - just be sure in case line break jh-1
    else -- jh-1
      hardbarlength[i] = hardbarlength[i] + x
      hardsign = hardsign + x -- accumulate hardspace, there may be a sign change
    end

    jbar = bar

  end -- end if statements

end -- end while loop

--[[
Decrement the number of sections if the final section is void.
 This will be the usual case where the input file ends with
 an end of section record.
 If this record has been omitted, stop going on to avoid
 'You can't use \raise....'.
 ]]

if dbug then
  print(" ... compute")
end

if (bars[all_section] == 0) then
  all_section = all_section - 1
else
  error_exit(4)
end

-- Summarize sectioning information

if dbug then
  print("\nNumber of sections        : "  .. (all_section+1) .."\n")

  for section = 0, all_section do
    if (autolines[section]) then
      print("---- autoline section ----")
    end
    print("Section                   : " .. (section+1))
    print("Number of bars in section : " .. bars[section])
    print("Length(hard) of section " .. (section+1) .. " : " .. eff_hardlength[section])
    print("Length(soft) of section " .. (section+1) .. " : " .. eff_softlength[section])
    if (linegoal[section] ~= 0) then -- linegoal applies, signed jh-2 in  C
      print("Section line goal was determined by \\linegoal value...")
      print("Line Goal for section     : " .. linegoal[section])
    else -- mulooseness applies, signed jh-2 in C
      print("Looseness of section      : " .. mulooseness[section])
    end

    io.read() -- NH: mimics getchar()
  end -- end for

end -- end if

if dbug_logfile then
  logfile:write("\nNumber of sections        : ", all_section+1, "\n\n")

  for section = 0, all_section do
    if (autolines[section]) then
      logfile:write("---- autoline section ----\n")
    end
    logfile:write("Section                   : ", section+1, "\n")
    logfile:write("Number of bars in section : ", bars[section], "\n")
    logfile:write("Length(hard) of section ", section+1, " : ", eff_hardlength[section], "\n")
    logfile:write("Length(soft) of section ", section+1, " : ", eff_softlength[section], "\n")
    if (linegoal[section] ~= 0) then -- line goal applies. signed jh-2 in C
      logfile:write("Section line goal was determined by \\linegoal value...\n")
      logfile:write("Line goal for section     : ", linegoal[section], "\n")
    else -- mulooseness applies, signed jh-2 in C
      logfile:write("Looseness of section      : ", mulooseness[section], "\n")
    end
  end
end

--[[
C Comments:
Loop over the sections defined in the input file.
 Each section must be right justified.
 LAST is the absolute number of the last bar
   in the current section.
--]]

sign = 0
mark = 0
lastbarnumber = 0


for section = 0, all_section do
  line_in_section = 1
  lastbarnumber = lastbarnumber + bars[section]

  -- Find number of lines to work towards
  lines = math.floor(((eff_hardlength[section] + eff_softlength[section] + parindent)/
        (linewidth-(clefskip+signskip[sign]))) + .5) -- need to use floor function as lua has no "integer" type, just numbers
  if (lines == 0) then lines = 1 end -- safety

  natural_lines = lines -- Keep this for debug report. Signed jh-2 in C
  lines = lines + mulooseness[section]
  if ((mulooseness[section] ~= 0) and (linegoal[section] > 0)) then -- signed jh-2 in C
    print("Unexpected line goal reset occured for section " .. section+1)
    linegoal[section] = 0 -- Zero it, Safety, should not happen, signed jh-2 in C
  end
  if (linegoal[section] > 0) then lines = linegoal[section] end -- Signed jh-2 in C
  if lines < 1 then
    lines = 1
    print("Don't stress \\mulooseness too much !!!")
  end

--[[
C comments:
 autolinesflag set in current section ?
 iftrue force number of lines to 1
 --]]

  if (autolines[section]) then lines = 1 end

  if dbug then
    print("Section number           : " .. section+1)
    print("Last bar in this section : " .. lastbarnumber)
    print("Number of bars           : " .. bars[section])
    print("Natural number of lines  : " .. natural_lines) -- signed jh-2 in C
    print("Chosen  number of lines  : " .. lines .. "\n")
  end

  if dbug_logfile then
    logfile:write("Section number           : ", section+1, "\n")
    logfile:write("Last bar in this section : ", lastbarnumber, "\n")
    logfile:write("Number of bars           : ", bars[section], "\n")
    logfile:write("Natural number of lines : ", natural_lines, "\n") -- signed jh-2 in C
    logfile:write("Chosen  number of lines : ", lines, "\n\n")
  end

  if (bars[section]<1) then error_exit(3) end

--[[
C comments:
 fill_length is the length of 'bar' material (ie excluding
 signature space) required to fill the remainder
 of the piece. This value will not be exact if there are
 sign changes within the section. However,
 fill_length is used only to keep track of the mean scale factor
 for the remainder of the piece, as opposed to individual lines.

 Loop over lines, working out number of bars
 and revised \elemskip for each line.
 added correct computing of fill_length
 --]]


  for j = 1, lines do
    line_number = line_number + 1
    fill_length = (lines-j+1) * (linewidth - (clefskip + signskip[sign]))


--[[
C comments:
 Work out mean element skip over remaining bars
 in the current section.
 EFFWID is the effective line width once
 key signature have been written.
 Set parindent to zero after it has been used for the
 first line of the first section.
 --]]

    if (eff_softlength[section] == 0) then error_exit(5) end
    spc_factor = (fill_length - eff_hardlength[section])/eff_softlength[section]
    if ((xbar[mark+1] > 1) and (mark>0)) then
      -- The bar is an bar+xbar with a sign change. Signed jh-1 in C
      eff_linewidth = linewidth - (clefskip+signskip[sign-1]) - parindent
    else -- This is a normal bar. Signed jh-1 in C
      eff_linewidth = linewidth - (clefskip+signskip[sign]) - parindent
    end -- Signed jh-1 in C

    signskip[sign] = futuresignskip[sign] -- Supports xbar signs. Signed jh-1 in C

    parindent = 0

    -- Fill the current line by adding bars until overflow.

    i = mark
    firstbarno = barno[mark+1]
    hardlength = 0
    softlength = 0
    x = 0
    lastbar = 0
    detect_end = false

    while (x < eff_linewidth) do
      if detect_end then break end
      i = i + 1
      -- Check for raggedline

      if raggedline[i] then detectraggedline = true end

      -- Check for key signature change at this bar.

      if (i == signchange[sign+1]) then sign = sign + 1 end

      lastbar = hardbarlength[i] + spc_factor*softbarlength[i]
      x = x + lastbar

      -- Enforce termination at last bar and last line

      if (i==lastbarnumber) then detect_end = true
      elseif (line_in_section == lines) then
        detect_end = false
        x = 0
      end

      hardlength = hardlength + hardbarlength[i]
      softlength = softlength + softbarlength[i]

    end

--[[
C comments:
 If the overhang is less than half the barlength,
 include the latest bar in the line,
 and shrink the line accordingly.
--]]

    if ((x-eff_linewidth) < (lastbar/2)) then
      barsinline = i - mark
      mark = i
      lastbarno = barno[mark]

--[[
C comments:
 last bar in line a zbar?
 if true -> add to the first bar in next line
 the amount of afterruleskip
--]]

      if zbar[mark] then
        softbarlength[i+1] = softbarlength[i+1] + afterrule
        eff_softlength[section] = eff_softlength[section] + afterrule
      end

--[[
C comments:
 last bar in line a leftrightrepeat?
 if true -> reduce hardwidth of current line
            advance the hardwidth of next bar
            advance the softwidth of next bar
--]]

      if lr_repeat[mark] then
        -- NH: there are some commented lines of code out here (printf statements) that I have not preserved
        -- if they should be reinstated then they are located at lines 744-746 of the original program
        hardlength = hardlength - (width_leftrightrepeat[i]  - width_leftrepeat[i])
        eff_hardlength[section] = eff_hardlength[section] + (width_leftrightrepeat[i] - width_leftrepeat[i])
        hardbarlength[i+1] = hardbarlength[i+1] + width_leftrepeat[i]
        softbarlength[i+1] = softbarlength[i+1] + (afterrule/2)
        eff_softlength[section] = eff_softlength[section] + (afterrule/2)
      end

--[[
C comments:
 last bar in line a leftrepeat?
 if true -> reduce hardwidth of current line
            advance the hardwidth of next bar
            advance the softwidth of next bar
--]]

      if l_repeat[mark] then
        hardlength = hardlength - (width_leftrepeat[i] - lthick)
        hardbarlength[i+1] = hardbarlength[i+1] + width_leftrepeat[i]
        softbarlength[i+1] = softbarlength[i+1] + (afterrule/2)
        eff_softlength[section] = eff_softlength[section] + (afterrule/2)
      end

      if (signchange[sign+1] == mark+1) then -- signed s.b. in C
        sign = sign + 1
--[[
C comments:
  Because the bar is staying here in the line, we look ahead
  to see if the upcoming bar is a sign change, and adjust space
  to account for the complimentary sign change notice that will
  be posted at the end of this line.  However, if the upcoming
  sign change bar is really a bar+xbar set, where the sign change
  is buried in the xbar, then we don't do the move because the
  change notice really won't be posted in this line.

  Signed jh-1 in C.

--]]
        if (xbar[mark+1] < 2) then -- okay to do the move. signed jh-1 in C
          hardlength = hardlength + oldsignskip[sign]
          hardbarlength[mark+1] = hardbarlength[mark+1] - oldsignskip[sign]
        end
      end

    -- Exclude the latest bar, and stretch the line.

    else
      barsinline=i-1-mark
      if (barsinline < 1) then error_exit(2) end
      mark = i - 1
      lastbarno = barno[mark]
      hardlength = hardlength - hardbarlength[i]
      softlength = softlength - softbarlength[i]

      if zbar[mark] then softbarlength[i] = softbarlength[i] + afterrule end

      if lr_repeat[mark] then
        hardlength = hardlength - (width_leftrightrepeat[i-1]-width_leftrepeat[i-1])
        eff_hardlength[section] = eff_hardlength[section] + (width_leftrightrepeat[i-1] - width_leftrepeat[i-1])
        hardbarlength[i] = hardbarlength[i] + width_leftrepeat[i-1]
        softbarlength[i] = softbarlength[i] + (afterrule/2)
        eff_softlength[section] = eff_softlength[section] + (afterrule/2)
      end

      if l_repeat[mark] then
        hardlength = hardlength - (width_leftrepeat[i-1] - lthick)
        hardbarlength[i] = hardbarlength[i] + width_leftrepeat[i-1]
        softbarlength[i] = softbarlength[i] + (afterrule/2)
        eff_softlength[section] = eff_softlength[section] + (afterrule/2)
      end

--[[
C comments:
 Error (o/u-hbox) occurs only when signature change start in next line
 -> look for signature change in next line
 if true then advance the hardwidth of current line
    reduce next hard barlength by signature change
--]]

      if (signchange[sign] == (mark+1)) then
--[[
C comments:
However, if the next bar is a bar+xbar set where the
sign change comes from the xbar, then don't do this
move, because the extra skip is not really there!

Signed jh-1 in C.
--]]
        if(xbar[mark+1] < 2) then -- alright, do the move. signed jh-1 in C
          hardlength = hardlength + oldsignskip[sign]
          hardbarlength[mark+1] = hardbarlength[mark+1] - oldsignskip[sign]
        end

      end

    end

--[[
C comments:
 Define a flex factor for this line as the ratio
 of soft part of the specified line width,
 to soft width in the approximate line.
 --]]

    if (softlength == 0) then error_exit(5) end
    flexit = (eff_linewidth - hardlength) / softlength
    if detectraggedline then
      flexit = 1
      detectraggedline = false
    end
    cor_elemskip = elemskip * flexit
    cor_afterrule = afterrule * flexit
    cor_beforerule = beforerule * flexit

    if dbug then
      print("Line number             : " .. line_number)
      print("Fill length             : " .. fill_length)
      print("Effective length        : " .. eff_softlength[section] + eff_hardlength[section])
      print("Mean space factor       : " .. spc_factor)
      print("Bars in line            : " .. barsinline)
      print("Effective linewidth     : " .. eff_linewidth)
      print("Uncorrected hard length : " .. hardlength)
      print("Uncorrected soft length : " .. softlength)
      print("fLex factor (soft)      : " .. flexit)
      print("Corrected elemskip      : " .. cor_elemskip)
      print("Corrected afterrule     : " .. cor_afterrule)
      print("Corrected beforerule    : " .. cor_beforerule)

      io.read() -- NH: mimics getchar()
    end

    if dbug_logfile then
      logfile:write("Line number             : ", line_number, "\n")
      logfile:write("Fill length             : ", fill_length, "\n")
      logfile:write("Effective length        : ", eff_softlength[section] + eff_hardlength[section], "\n")
      logfile:write("Mean space factor       : ", spc_factor, "\n")
      logfile:write("Bars in line            : ", barsinline, "\n")
      logfile:write("Effective linewidth     : ", eff_linewidth, "\n")
      logfile:write("Uncorrected hard length : ", hardlength, "\n")
      logfile:write("Uncorrected soft length : ", softlength, "\n")
      logfile:write("Flex factor (soft)      : ", flexit, "\n")
      logfile:write("Corrected elemskip      : ", cor_elemskip, "\n")
      logfile:write("Corrected afterrule     : ", cor_afterrule, "\n")
      logfile:write("Corrected beforerule    : ", cor_beforerule, "\n\n")
    end

    eff_hardlength[section] = eff_hardlength[section] - hardlength
    eff_softlength[section] = eff_softlength[section] - softlength
    fill_length = fill_length - eff_linewidth

    -- Write a record to the output file

    outfile:write(
      string.format("\\lineset{%3d}{%2d}{%8.5fpt}{%8.5fpt}{%8.5fpt}%% %d - %d\n",
      line_number, barsinline, cor_elemskip, cor_afterrule,
      cor_beforerule,
      firstbarno, lastbarno))



    if showresult then
      print(
        string.format("\\lineset{%3d}{%2d}{%8.5fpt}{%8.5fpt}{%8.5fpt}%% %d - %d",
        line_number, barsinline, cor_elemskip, cor_afterrule,
        cor_beforerule, 
        firstbarno, lastbarno))
    end

    if dbug_lines then print(" ... writing line : " .. line_number) end
    line_in_section = line_in_section + 1 -- NH: need to do this in loop, lua syntax doesn't allow for it in for loop spec
  end -- end lines for
end -- end sections for

if dbug_lines then print("") end


samechapter = true
chapterno = chapterno + 1
until (not GETLINE()) -- end repeat loop

-- closing files
infile:close()
io.write(")")
if dbug_logfile then
  logfile:close()
  if (not logfile) then error_exit(0) end
end
outfile:close()
if (not outfile) then error_exit(0) end

if dbug then
  print(" ... that's all, bye")
end
print("")
-- NH: end main chunk and program!
