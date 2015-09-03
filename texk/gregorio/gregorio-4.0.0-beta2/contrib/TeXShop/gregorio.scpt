--Applescript
-- Tracy Llenos <tllenos@gmail.com>
-- a workaround macro for compiling Gregorio gabc files inside TeXShop

set fileName to #FILEPATH#
if fileName is equal to ""
activate
display dialog "Please save the file first" buttons {"OK"} default button "OK"
return
end if

-- finds proper filenames
-- from the Claus Gerhardt TeXShop macros
set fileName to #NAMEPATH#
set n to (number of characters of contents of fileName)
set fileNamequoted to quoted form of fileName
set baseName to do shell script "basename " & fileNamequoted
set m to (number of characters of contents of baseName)
set dirName to quoted form of (characters 1 thru (n - m - 1) of fileName as string)
set gabcName to baseName & ".gabc"

do shell script "cd " & dirName & ";" & "/usr/local/bin/gregorio " & gabcName

tell application "TeXShop"
activate
end tell
-- we return to TeXShop  TODO: the option of opening the tex file?
