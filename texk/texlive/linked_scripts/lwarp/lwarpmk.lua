#!/usr/bin/env texlua

-- Copyright 2016-2018 Brian Dunn

printversion = "v0.57"

function printhelp ()
print ("lwarpmk: Use lwarpmk -h or lwarpmk --help for help.") ;
end

function printusage ()
--
-- Print the usage of the lwarpmk command:
--
print ( [[

lwarpmk print [project]: Compile the print version if necessary.
lwarpmk print1 [project]: Forced single compile of the print version.
lwarpmk printindex [project]: Process the index for the print version.
lwarpmk printglossary [project]: Process the glossary for the print version.
lwarpmk html [project]: Compile the HTML version if necessary.
lwarpmk html1 [project]: Forced single compile of the HTML version.
lwarpmk htmlindex [project]: Process the index for the html version.
lwarpmk htmlglossary [project]: Process the glossary for the html version.
lwarpmk again [project]: Touch the source code to trigger recompiles.
lwarpmk limages [project]: Process the "lateximages" created by lwarp.sty.
lwarpmk pdftohtml [project]:
    For use with latexmk or a Makefile:
    Converts project_html.pdf to project_html.html and individual HTML files.
    Finishes the HTML conversion even if there was a compile error.
lwarpmk pdftosvg <list of file names>: Converts each PDF file to SVG.
lwarpmk clean [project]: Remove .aux, .toc, .lof/t, .idx, .ind, .log, *_html_inc.*, .gl*
lwarpmk cleanall [project]: Remove auxiliary files and also project.pdf, *.html
lwarpmk cleanlimages: Removes all images from the "lateximages" directory.
lwarpmk -h: Print this help message.
lwarpmk --help: Print this help message.

]] )
printconf ()
end

function printconf ()
--
-- Print the format of the configuration file lwarpmk.conf:
--
print ( [[
An example lwarpmk.conf or <project>.lwarpmkconf project file:
--
opsystem = "Unix"   (or "Windows")
latexname = "pdflatex"  (or "lualatex", or "xelatex")
sourcename = "projectname"  (the source-code filename w/o .tex)
homehtmlfilename = "index"  (or perhaps the project name)
htmlfilename = ""  (or "projectname" - filename prefix)
latexmk = "false"  (or "true" to use latexmk to build PDFs)
shellescape = "false"
xindylanguge = "english"  (use a language supported by xindy)
xindycodepage = "utf8"  (use a codepage supported by xindy)
xindystyle = "lwarp.xdy" (or a custom file based on lwarp.xdy)
pdftotextenc = "UTF-8"  (use an encoding supported by pdftotext)
--
Filenames must contain only letters, numbers, underscore, or dash.
Values must be in upright "quotes".

]] ) ;
end

function splitfile (destfile,sourcefile)
--
-- Split one large sourcefile into a number of files,
-- starting with destfile.
-- The file is split at each occurance of <!--|Start file|newfilename|*
--
print ("lwarpmk: Splitting " .. sourcefile .. " into " .. destfile) ;
local sfile = io.open(sourcefile)
io.output(destfile)
for line in sfile:lines() do
i,j,copen,cstart,newfilename = string.find (line,"(.*)|(.*)|(.*)|") ;
if ( (i~= nil) and (copen == "<!--") and (cstart == "Start file")) then
    -- split the file
    io.output(newfilename) ;
else
    -- not a splitpoint
    io.write (line .. "\n") ;
end
end -- do
io.close(sfile)
end -- function

function cvalueerror ( line, linenum , cvalue )
--
-- Incorrect value, so print an error and exit.
--
    print ("lwarpmk: ===")
    print ("lwarpmk: " .. linenum .. " : " .. line ) ;
    print (
        "lwarpmk: incorrect variable value \"" .. cvalue ..
        "\" in lwarpmk.conf.\n"
    ) ;
    print ("lwarpmk: ===")
    printconf () ;
    os.exit(1) ;
end

function loadconf ()
--
-- Load settings from the project's "lwarpmk.conf" file:
--
-- Default configuration filename:
local conffile = "lwarpmk.conf"
-- Optional configuration filename:
if ( arg[2] ~= nil ) then conffile = arg[2]..".lwarpmkconf" end
-- Additional defaults:
opsystem = "Unix"
latexmk = "false"
shellescape = "false"
xindylanguage = "english"
xindycodepage = "utf8"
xindystyle = "lwarp.xdy"
pdftotextenc = "UTF-8"
-- Verify the file exists:
if (lfs.attributes(conffile,"mode")==nil) then
    -- file not exists
    print ("lwarpmk: ===")
    print ("lwarpmk: File \"" .. conffile .."\" does not exist.")
    print ("lwarpmk: Move to the project's source directory,")
    print ("lwarpmk: recompile using pdflatex, xelatex, or lualatex,")
    print ("lwarpmk: then try using lwarpmk again.")
    if ( arg[2] ~= nil ) then
        print (
            "lwarpmk: (\"" .. arg[2] ..
            "\" does not appear to be a project name.)"
        )
    end
    print ("lwarpmk: ===")
    printhelp () ;
    os.exit(1) -- exit the entire lwarpmk script
else -- file exists
-- Read the file:
print ("lwarpmk: Reading " .. conffile ..".")
local cfile = io.open(conffile)
-- Scan each line:
local linenum = 0
for line in cfile:lines() do -- scan lines
linenum = linenum + 1
i,j,cvarname,cvalue = string.find (line,"([%w-_]*)%s*=%s*\"([%w%-_%.]*)\"") ;
-- Error if incorrect enclosing characters:
if ( i == nil ) then
    print ("lwarpmk: ===")
    print ("lwarpmk: " ..  linenum .. " : " .. line ) ;
    print ("lwarpmk: Incorrect entry in " .. conffile ..".\n" ) ;
    print ("lwarpmk: ===")
    printconf () ;
    os.exit(1) ;
end -- nil
if ( cvarname == "opsystem" ) then
    -- Verify choice of opsystem:
    if ( (cvalue == "Unix") or (cvalue == "Windows") ) then
        opsystem = cvalue
    else
        cvalueerror ( line, linenum , cvalue )
    end
elseif ( cvarname == "latexname" ) then
    -- Verify choice of LaTeX compiler:
    if (
        (cvalue == "pdflatex") or
        (cvalue == "xelatex") or
        (cvalue == "lualatex")
    ) then
        latexname = cvalue
    else
        cvalueerror ( line, linenum , cvalue )
    end
elseif ( cvarname == "sourcename" ) then sourcename = cvalue
elseif ( cvarname == "homehtmlfilename" ) then homehtmlfilename = cvalue
elseif ( cvarname == "htmlfilename" ) then htmlfilename = cvalue
elseif ( cvarname == "latexmk" ) then latexmk = cvalue
elseif ( cvarname == "shellescape" ) then shellescape = cvalue
elseif ( cvarname == "xindylanguage" ) then xindylanguage = cvalue
elseif ( cvarname == "xindycodepage" ) then xindycodepage = cvalue
elseif ( cvarname == "xindystyle" ) then xindystyle = cvalue
elseif ( cvarname == "pdftotextenc" ) then pdftotextenc = cvalue
else
    print ("lwarpmk: ===")
    print ("lwarpmk: " .. linenum .. " : " .. line ) ;
    print (
        "lwarpmk: Incorrect variable name \"" .. cvarname .. "\" in " ..
        conffile ..".\n"
    ) ;
    print ("lwarpmk: ===")
    printconf () ;
os.exit(1) ;
end -- cvarname
end -- do scan lines
io.close(cfile)
end -- file exists
-- Error if sourcename is "lwarp".
-- This could happen if a local copy of lwarp has recently been recompiled.
if sourcename=="lwarp" then
    print ("lwarpmk: ===")
    print ("lwarpmk: Lwarp has recently been recompiled in this directory,")
    print ("lwarpmk: and \"lwarpmk.conf\" is no longer set for your own project.")
    print ("lwarpmk: Recompile your own project using pdf/lua/xelatex <projectname>.")
    print ("lwarpmk: After a recompile, \"lwarpmk.conf\" will be set for your project,")
    print ("lwarpmk: and you may again use lwarpmk.")
    print ("lwarpmk: ===")
    os.exit(1)
end -- sourcename of "lwarp"
-- Select some operating-system commands:
if opsystem=="Unix" then  -- For Unix / Linux / Mac OS:
    rmname = "rm"
    mvname = "mv"
    cpname = "cp"
    touchnamepre = "touch"
    touchnamepost = ""
    newtouchname = "touch"
    dirslash = "/"
    opquote= "\'"
    cmdgroupopenname = " ( "
    cmdgroupclosename = " ) "
    seqname = " && "
    bgname = " &"
elseif opsystem=="Windows" then -- For Windows
    rmname = "DEL"
    mvname = "MOVE"
    cpname = "COPY"
    touchnamepre = "COPY /b"
    touchnamepost = "+,,"
    newtouchname = "echo empty >"
    dirslash = "\\"
    opquote= "\""
    cmdgroupopenname = ""
    cmdgroupclosename = ""
    seqname = " & "
    bgname = ""
else print ( "lwarpmk: Select Unix or Windows for opsystem" )
end --- for Windows

-- set xindycmd, glossarycmd according to pdflatex vs xelatex/lualatex:
if ( latexname == "pdflatex" ) then
    xindycmd = "texindy  "
    glossarycmd = "xindy  "
else
    xindycmd = "xindy  -M texindy  "
    glossarycmd = "xindy "
end

end -- loadconf

function executecheckerror ( executecommands , errormessage )
--
-- Execute an operating system call,
-- and maybe exit with an error message.
--
local err
err = os.execute ( executecommands )
if ( err ~= 0 ) then
    print ("lwarpmk: ===")
    print ("lwarpmk: " .. errormessage )
    print ("lwarpmk: ===")
    os.exit(1)
end
end -- executecheckerror

function refreshdate ()
os.execute(touchnamepre .. " " .. sourcename .. ".tex " .. touchnamepost)
end

function reruntoget (filesource)
--
-- Scan the LaTeX log file for the phrase "Rerun to get",
-- indicating that the file should be compiled again.
-- Return true if found.
--
local fsource = io.open(filesource)
for line in fsource:lines() do
if ( string.find(line,"Rerun to get") ~= nil ) then
    io.close(fsource)
    return true
end -- if
end -- do
io.close(fsource)
return false
end

function onetime (fsuffix)
--
-- Compile one time, return true if should compile again.
-- fsuffix is "" for print, "_html" for HTML output.
--
print("lwarpmk: Compiling with " .. latexname .. " " .. sourcename..fsuffix)
local thisshellescape = " "
if ( shellescape == "true" ) then
    thisshellescape = " -shell-escape "
else
    thisshellescape = " "
end
executecheckerror (
    latexname .. thisshellescape .. sourcename..fsuffix ,
    "Compile error."
)
return (reruntoget(sourcename .. fsuffix .. ".log") ) ;
end

function manytimes (fsuffix)
--
-- Compile up to five times.
-- fsuffix is "" for print, "_html" for HTML output
--
if onetime(fsuffix) == true then
if onetime(fsuffix) == true then
if onetime(fsuffix) == true then
if onetime(fsuffix) == true then
if onetime(fsuffix) == true then
end end end end end
end

function verifyfileexists (filename)
--
-- Exit if the given file does not exist.
--
if (lfs.attributes ( filename , "modification" ) == nil ) then
    print ("lwarpmk: ===")
    print ("lwarpmk: " .. filename .. " not found." ) ;
    print ("lwarpmk: ===")
    os.exit (1) ;
end
end

function pdftohtml ()
--
-- Convert <project>_html.pdf into HTML files:
--
-- Convert to text:
print ("lwarpmk: Converting " .. sourcename
    .."_html.pdf to " .. sourcename .. "_html.html")
os.execute("pdftotext  -enc " .. pdftotextenc .. "  -nopgbrk  -layout "
    .. sourcename .. "_html.pdf " .. sourcename .. "_html.html")
-- Split the result into individual HTML files:
splitfile (homehtmlfilename .. ".html" , sourcename .. "_html.html")
end

function removeaux ()
--
-- Remove auxiliary files:
-- All aux files are removed since there may be many bbl*.aux files.
--
os.execute ( rmname .. " *.aux " ..
    sourcename ..".toc " .. sourcename .. "_html.toc " ..
    sourcename ..".lof " .. sourcename .. "_html.lof " ..
    sourcename ..".lot " .. sourcename .. "_html.lot " ..
    sourcename ..".idx " .. sourcename .. "_html.idx " ..
    sourcename ..".ind " .. sourcename .. "_html.ind " ..
    sourcename ..".log " .. sourcename .. "_html.log " ..
    sourcename ..".gl* " .. sourcename .. "_html.gl* " ..
    " *_html_inc.* "
    )
end

function checkhtmlpdfexists ()
--
-- Error if the HTML document does not exist.
-- The lateximages are drawn from the HTML PDF version of the document,
-- so "lwarpmk html" must be done before "lwarpmk limages".
--
local htmlpdffile = io.open(sourcename .. "_html.pdf", "r")
if ( htmlpdffile == nil ) then
    print ("")
    print ("lwarpmk: ===")
    print ("lwarpmk: The HTML version of the document does not exist.")
    print ("lwarpmk: Enter \"lwarpmk html\" to compile the HTML version.")
    print ("lwarpmk: ===")
    os.exit(1)
end
io.close (htmlpdffile)
end -- checkhtmlpdfexists

function warnlimages ()
--
-- Warning of a missing lateximages.txt file:
--
    print ("lwarpmk: ===")
    print ("lwarpmk: \"lateximages.txt\" does not exist.")
    print ("lwarpmk: Your project does not use SVG math or other lateximages,")
    print ("lwarpmk: or the file has been deleted somehow.")
    print ("lwarpmk: Use \"lwarpmk html\" to recompile your project")
    print ("lwarpmk: and recreate \"lateximages.txt\".")
    print ("lwarpmk: If your project does not use SVG math or other lateximages,")
    print ("lwarpmk: then \"lateximages.txt\" will never exist, and")
    print ("lwarpmk: \"lwarpmk limages\" will not be necessary.")
    print ("lwarpmk: ===")
end -- warnlimages

function checklimages ()
--
-- Check lateximages.txt to see if need to recompile first.
-- If any entry has a page number of zero, then there were incorrect images.
--
print ("lwarpmk: Checking for a valid lateximages.txt file.")
local limagesfile = io.open("lateximages.txt", "r")
if ( limagesfile == nil ) then
    warnlimages ()
    os.exit(1)
end
-- Track warning to recompile if find a page 0
local pagezerowarning = false
-- Scan lateximages.txt
for line in limagesfile:lines() do
    -- lwimgpage is the page number in the PDF which has the image
    -- lwimghash is true if this filename is a hash
    -- lwimgname is the lateximage filename root to assign for the image
    i,j,lwimgpage,lwimghash,lwimgname = string.find (line,"|(.*)|(.*)|(.*)|")
    -- For each entry:
    if ( (i~=nil) ) then
        -- If the page number is 0, image references are incorrect
        --  and must recompile the soure document:
        if ( lwimgpage == "0" ) then
            pagezerowarning = true
        end
    end -- if i~=nil
end -- do
if ( pagezerowarning ) then
    print ("")
    print ("lwarpmk: ===")
    print ("lwarpmk: The document must be recompiled before creating the lateximages.")
    print ("lwarpmk: Enter \"lwarpmk html\" again, then try \"lwarpmk limages\" again.")
    print ("lwarpmk: ===")
    os.exit(1) ;
end -- pagezerowarning
end -- checklimages

function createuniximage ( lwimgfullname )
--
-- Create one lateximage for Unix / Linux / Mac OS.
--
executecheckerror (
    cmdgroupopenname ..
    "pdfseparate -f " .. lwimgpage .. " -l " .. lwimgpage .. " " ..
        sourcename .."_html.pdf " ..
        "lateximages" .. dirslash .."lateximagetemp-%d" .. ".pdf" ..
        seqname ..
    -- Crop the image:
    "pdfcrop  --hires  lateximages" .. dirslash .. "lateximagetemp-" ..
        lwimgpage .. ".pdf " ..
        "lateximages" .. dirslash .. lwimgname .. ".pdf" ..
        seqname ..
    -- Convert the image to svg:
    "pdftocairo -svg  -noshrink  lateximages" .. dirslash .. lwimgname .. ".pdf " ..
        "lateximages" .. dirslash .. lwimgname ..".svg" ..
        seqname ..
    -- Remove the temporary files:
    rmname .. " lateximages" .. dirslash .. lwimgname .. ".pdf" .. seqname ..
    rmname .. " lateximages" .. dirslash .. "lateximagetemp-" .. lwimgpage .. ".pdf" ..
    cmdgroupclosename .. " >/dev/null " .. bgname
    ,
    "File error trying to convert " .. lwimgfullname
)
-- Every 32 images, wait for completion at below normal priority,
--  allowing other image tasks to catch up.
numimageprocesses = numimageprocesses + 1
if ( numimageprocesses > 32 ) then
    numimageprocesses = 0
    print ( "lwarpmk: waiting" )
    executecheckerror ( "wait" , "File error trying to wait.")
end
end -- createuniximage

function createwindowsimage ( lwimgfullname )
--
-- Create one lateximage for Windows.
--
-- Every 32 images, wait for completion at below normal priority,
--  allowing other image tasks to catch up.
numimageprocesses = numimageprocesses + 1
if ( numimageprocesses > 32 ) then
    numimageprocesses = 0
    thiswaitcommand = "/WAIT /BELOWNORMAL"
    print ( "lwarpmk: waiting" )
else
    thiswaitcommand = ""
end
-- Execute the image generation command
executecheckerror (
    "start /B " .. thiswaitcommand .. " \"\" lwarp_one_limage " ..
    lwimgpage .. " " ..
    lwimghash .. " " ..
    lwimgname .. " " ..
    sourcename .. " <nul >nul"
    ,
    "File error trying to create image."
)
end -- createwindowsimage

function createonelateximage ( line )
--
-- Given the next line of lateximages.txt, convert a single image.
--
-- lwimgpage is the page number in the PDF which has the image
-- lwimghash is true if this filename is a hash
-- lwimgname is the lateximage filename root to assign for the image
i,j,lwimgpage,lwimghash,lwimgname = string.find (line,"|(.*)|(.*)|(.*)|")
-- For each entry:
if ( (i~=nil) ) then
    -- Skip if the page number is 0:
    if ( lwimgpage == "0" ) then
        pagezerowarning = true
    else
        -- Skip is this image is hashed and already exists:
        local lwimgfullname = "lateximages" .. dirslash .. lwimgname .. ".svg"
        if (
            (lwimghash ~= "true") or
            (lfs.attributes(lwimgfullname,"mode")==nil) -- file not exists
        )
        then -- not hashed or not exists:
            -- Print the name of the file being generated:
            print ( "lwarpmk: " .. lwimgname )
            -- Touch/create the dest so that only once instance tries to build it:
            executecheckerror (
                newtouchname .. " " .. lwimgfullname ,
                "File error trying to touch " .. lwimgfullname
            )
            -- Separate out the image into its own single-page pdf:
            if opsystem=="Unix" then
                createuniximage (lwimgfullname)
            elseif opsystem=="Windows" then
                createwindowsimage (lwimgfullname)
            end
        end -- not hashed or not exists
    end -- not page 0
end -- not nil
end -- createonelateximage

function createlateximages ()
--
-- Create lateximages based on lateximages.txt:
--
-- See if the document must be recompiled first:
checklimages ()
-- See if the print version exists:
checkhtmlpdfexists ()
-- Attempt to create the lateximages:
print ("lwarpmk: Creating lateximages.")
local limagesfile = io.open("lateximages.txt", "r")
if ( limagesfile == nil ) then
    warnlateximages ()
    os.exit(1)
end
-- Create the lateximages directory, ignore error if already exists
err = os.execute("mkdir lateximages")
-- For Windows, create lwarp_one_limage.cmd from lwarp_one_limage.txt:
if opsystem=="Windows" then
    executecheckerror (
        cpname .. " lwarp_one_limage.txt lwarp_one_limage.cmd" ,
        "File error trying to copy lwarp_one_limage.txt to lwarp_one_limage.cmd"
    )
end -- create lwarp_one_limage.cmd
-- Track the number of parallel processes
numimageprocesses = 0
-- Track warning to recompile if find a page 0
pagezerowarning = false
-- Scan lateximages.txt
for line in limagesfile:lines() do
    createonelateximage ( line )
end -- do
io.close(limagesfile)
print ( "lwarpmk limages: ===")
print ( "lwarpmk limages: Wait a moment for the images to complete" )
print ( "lwarpmk limages:   before reloading the page." )
print ( "lwarpmk limages: ===")
print ( "lwarpmk limages: Done." )
if ( pagezerowarning == true ) then
    print ( "lwarpmk limages: WARNING: Images will be incorrect." )
    print ( "lwarpmk limages:   Enter \"lwarpmk cleanlimages\", then" )
    print ( "lwarpmk limages:   recompile the document one more time, then" )
    print ( "lwarpmk limages:   repeat \"lwarpmk images\" again." )
end -- pagezerowarning
end -- function

function compilelatexmk ( fsuffix )
--
-- Use latexmk to compile source and index:
-- fsuffix is "" for print, or "_html" for HTML
--
-- Maybe select the shell-escape option:
local thisshellescape = " "
if ( shellescape == "true" ) then
    thisshellescape = " -shell-escape "
else
    thisshellescape = " "
end
-- The recorder option is required to detect changes in <project>.tex
-- while we are loading <project>_html.tex.
executecheckerror (
    "latexmk -pdf -dvi- -ps- -recorder "
    .. "-e "
    .. opquote
    .. "$makeindex = q/" -- $
    .. xindycmd
    .. "  -M " .. xindystyle
    .. "  -C " .. xindycodepage
    .. "  -L " .. xindylanguage .. " /"
    .. opquote
    .. " -pdflatex=\"" .. latexname .. thisshellescape .." %O %S\" "
    .. sourcename..fsuffix ..".tex"
    ,
    "Compile error."
)
end -- function

function convertpdftosvg ()
--
-- Converts PDF files to SVG files.
-- The filenames are arg[2] and up.
-- arg[1] is the command "pdftosvg".
--
for i = 2 , #arg do
    if (lfs.attributes(arg[i],"mode")==nil) then
        print ("lwarpmk: File \"" .. arg[i] .. "\" does not exist.")
    else
        print ("lwarpmk: Converting \"" .. arg[i] .. "\"")
        os.execute ( "pdftocairo -svg " .. arg[i] )
    end -- if
end -- do
end --function

-- Start of the main code: --

-- lwarpmk --version :

if (arg[1] == "--version") then
print ( "lwarpmk: " .. printversion )

else -- not --version

-- print intro:

print ("lwarpmk: " .. printversion .. "  Automated make for the LaTeX lwarp package.")

-- lwarpmk print:

if arg[1] == "print" then
loadconf ()
if ( latexmk == "true" ) then
    compilelatexmk ("")
    print ("lwarpmk: Done.")
else -- not latexmk
    verifyfileexists (sourcename .. ".tex") ;
    -- See if up to date:
    if (
        ( lfs.attributes ( sourcename .. ".pdf" , "modification" ) == nil ) or
        (
            lfs.attributes ( sourcename .. ".tex" , "modification" ) >
            lfs.attributes ( sourcename .. ".pdf" , "modification" )
        )
    ) then
        -- Recompile if not yet up to date:
        manytimes("")
        print ("lwarpmk: Done.") ;
    else
        print ("lwarpmk: " .. sourcename .. ".pdf is up to date.") ;
    end
end -- not latexmk

-- lwarpmk print1:

elseif arg[1] == "print1" then
    loadconf ()
    verifyfileexists (sourcename .. ".tex") ;
    onetime("")
    print ("lwarpmk: Done.") ;

-- lwarp printindex:
-- Compile the index then touch the source
-- to trigger a recompile of the document:

elseif arg[1] == "printindex" then
loadconf ()
print ("lwarpmk: Processing the index.")
os.execute(
    xindycmd
    .. "  -M " .. xindystyle
    .. "  -C " .. xindycodepage
    .. "  -L " .. xindylanguage
    .. " " .. sourcename .. ".idx")
print ("lwarpmk: Forcing an update of " .. sourcename ..".tex.")
refreshdate ()
print ("lwarpmk: " .. sourcename ..".tex is ready to be recompiled.")
print ("lwarpmk: Done.")

-- lwarp printglossary:
-- Compile the glossary then touch the source
-- to trigger a recompile of the document:

elseif arg[1] == "printglossary" then
loadconf ()
print ("lwarpmk: Processing the glossary.")

os.execute(glossarycmd ..
    "  -L " .. xindylanguage ..
    "  -C " .. xindycodepage ..
    "  -I xindy -M " .. sourcename ..
    " -t " .. sourcename .. ".glg -o " .. sourcename .. ".gls "
    .. sourcename .. ".glo")
print ("lwarpmk: Forcing an update of " .. sourcename ..".tex.")
refreshdate ()
print ("lwarpmk: " .. sourcename ..".tex is ready to be recompiled.")
print ("lwarpmk: Done.")

-- lwarpmk html:

elseif arg[1] == "html" then
loadconf ()
if ( latexmk == "true" ) then
    compilelatexmk ("_html")
    pdftohtml ()
    print ("lwarpmk: Done.")
else -- not latexmk
    verifyfileexists ( sourcename .. ".tex" ) ;
    -- See if exists and is up to date:
    if (
        ( lfs.attributes ( homehtmlfilename .. ".html" , "modification" ) == nil ) or
        (
            lfs.attributes ( sourcename .. ".tex" , "modification" ) >
            lfs.attributes ( homehtmlfilename .. ".html" , "modification" )
        )
    ) then
        -- Recompile if not yet up to date:
        manytimes("_html")
        pdftohtml ()
        print ("lwarpmk: Done.")
    else
        print ("lwarpmk: " .. homehtmlfilename .. ".html is up to date.")
    end
end -- not latexmk

-- lwarpmk html1:

elseif arg[1] == "html1" then
    loadconf ()
    verifyfileexists ( sourcename .. ".tex" ) ;
    onetime("_html")
    pdftohtml ()
    print ("lwarpmk: Done.")

-- lwarpmk pdftohtml:
elseif arg[1] == "pdftohtml" then
    loadconf ()
    pdftohtml ()

-- lwarpmk htmlindex:
-- Compile the index then touch the source
-- to trigger a recompile of the document:

elseif arg[1] == "htmlindex" then
loadconf ()
print ("lwarpmk: Processing the index.")
os.execute(
    xindycmd
    .. "  -M " .. xindystyle
    .. "  -L " .. xindylanguage
    .. "  -C " .. xindycodepage
    .. " " .. sourcename .. "_html.idx"
)
print ("lwarpmk: Forcing an update of " .. sourcename ..".tex.")
refreshdate ()
print ("lwarpmk: " .. sourcename ..".tex is ready to be recompiled.")
print ("lwarpmk: Done.")

-- lwarpmk htmlglossary:
-- Compile the glossary then touch the source
-- to trigger a recompile of the document:

elseif arg[1] == "htmlglossary" then
loadconf ()
print ("lwarpmk: Processing the glossary.")

os.execute(glossarycmd ..
    "  -L " .. xindylanguage ..
    "  -C " .. xindycodepage ..
    "  -I xindy -M " ..sourcename ..
    "_html -t " .. sourcename .. "_html.glg -o " ..sourcename ..
    "_html.gls " ..sourcename .. "_html.glo")

print ("lwarpmk: Forcing an update of " .. sourcename ..".tex.")
refreshdate ()
print ("lwarpmk: " .. sourcename ..".tex is ready to be recompiled.")
print ("lwarpmk: Done.")

-- lwarpmk limages:
-- Scan the lateximages.txt file to create lateximages.

elseif arg[1] == "limages" then
loadconf ()
print ("lwarpmk: Processing images.")
createlateximages ()
print ("lwarpmk: Done.")

-- lwarpmk again:
-- Touch the source to trigger a recompile.

elseif arg[1] == "again" then
loadconf ()
print ("lwarpmk: Forcing an update of " .. sourcename ..".tex.")
refreshdate ()
print ("lwarpmk: " .. sourcename ..".tex is ready to be recompiled.")
print ("lwarpmk: Done.")

-- lwarpmk clean:
-- Remove project.aux, .toc, .lof, .lot, .idx, .ind, .log, *_html_inc.*, .gl*

elseif arg[1] == "clean" then
loadconf ()
removeaux ()
print ("lwarpmk: Done.")

-- lwarpmk cleanall
-- Remove project.aux, .toc, .lof, .lot, .idx, .ind, .log, *_html_inc.*, .gl*
--    and also project.pdf, *.html

elseif arg[1] == "cleanall" then
loadconf ()
removeaux ()
os.execute ( rmname .. " " ..
    sourcename .. ".pdf " .. sourcename .. "_html.pdf " ..
    "*.html"
    )
print ("lwarpmk: Done.")

-- lwarpmk cleanlimages
-- Remove images from the lateximages directory.

elseif arg[1] == "cleanlimages" then
loadconf ()
os.execute ( rmname .. " lateximages/*" )
print ("lwarpmk: Done.")

-- lwarpmk pdftosvg <list of file names>
-- Convert PDf files to SVG using pdftocairo
elseif arg[1] == "pdftosvg" then
convertpdftosvg ()
print ("lwarpmk: Done.")

-- lwarpmk with no argument :

elseif (arg[1] == nil) then
printhelp ()

-- lwarpmk -h or lwarpmk --help :

elseif (arg[1] == "-h" ) or (arg[1] == "--help") then
printusage ()

-- Unknown command:

else
printhelp ()
print ("\nlwarpmk: ****** Unknown command \""..arg[1].."\". ******\n")
end

end -- not --version
