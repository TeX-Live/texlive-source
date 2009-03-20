--[[------------------------------------------------------------------------
gzip.lua
support code for zlib library version 1.2.1 - gzip extentions
usage lua -lgzip ...

Author: Tiago Dionizio (tngd@mega.ist.utl.pt)
$Id: gzip.lua,v 1.3 2004/07/22 19:11:55 tngd Exp $
--]]------------------------------------------------------------------------


--[[------------------------------------------------------------------------

After loading the module a table with the following interface should be
available:

gzip.open(filename [, mode])

    Opens a file name using "gzopen". Behaviour is identical to the description
    given in the zlib library. If mode is not given a default mode "rb" will be
    used. Mode is the same as interpreted by gzopen function, ie, it can
    include special modes such as characters 1 to 9 that will be treated as the
    compression level when opening a file for writing.

    It returns a new file handle, or, in case of errors, nil plus an error
    message

gzip.lines(filename)

    Same behaviour as io.lines in the io standard library provided by lua
    with the aditional feature of working with gzip files. If a normal text
    file is read it will read it normaly (normal gzopen behaviour).

gzip.close(file)

    Same as file:close, use file:close instead.

file:flush()

    This function takes no parameters and flushes all output to working file.
    The same as calling 'gzflush(file, Z_FINISH)' so writing to the file will
    most likely not work as expected. This is subject to change in the future
    if there is a strong reason for it to happen.

file:read(format1, ...)
    Reads the file file, according to the given formats, which specify what
    to read. For each format, the function returns a string with the characters
    read, or nil if it cannot read data with the specified format. When called
    without formats, it uses a default format that reads the entire next line
    (see below).

    The available formats are

        "*a"   reads the whole file, starting at the current position. On end of
               file, it returns the empty string.
        "*l"   reads the next line (skipping the end of line), returning nil on
               end of file. This is the default format.
        number reads a string with up to that number of characters, returning
               nil on end of file. If number is zero, it reads nothing and
               returns an empty string, or nil on end of file.

    Unlink io.read, the "*n" format will not be available.


file:lines()

    Returns an iterator function that, each time it is called, returns a new
    line from the file. Therefore, the construction
       for line in file:lines() do ... end
   will iterate over all lines of the file. (Unlike gzip.lines, this function
   does not close the file when the loop ends.)

file:seek([whence] [, offset])

    Sets and gets the file position, measured from the beginning of the file,
    to the position given by offset plus a base specified by the string whence,
    as follows:

        "set" base is position 0 (beginning of the file);
        "cur" base is current position;

    In case of success, function seek returns the final file position, measured in bytes from the beginning of the file. If this function fails, it returns nil, plus a string describing the error.
    The default value for whence is "cur", and for offset is 0. Therefore, the call file:seek() returns the current file position, without changing it; the call file:seek("set") sets the position to the beginning of the file (and returns 0); and the call file:seek("end") sets the position to the end of the file, and returns its size.

    This function is subject to limitations imposed by gzseek function from
    zlib library, such as the inability to use "end" as the base for seeking
    and the inability to seek backwards when writing.

file:write(value1, ...)

    Writes the value of each of its arguments to the filehandle file. The
    arguments must be strings or numbers. To write other values, use tostring
    or string.format before write

file:close()

    Closes the file.

--]]------------------------------------------------------------------------

require("requirelib")

requirelib("lzlib", "luaopen_gzip", true)
