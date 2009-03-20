--[[------------------------------------------------------------------------
zlib.lua
support code for zlib library version 1.2.1
usage lua -lzlib ...

Author: Tiago Dionizio (tngd@mega.ist.utl.pt)
$Id: zlib.lua,v 1.3 2004/07/22 19:11:55 tngd Exp $
--]]------------------------------------------------------------------------

--[[ Exports ---------------------------------------------------------------

ds  = deflate stream
is  = inflate stream
int = number (integer)
*   = (any type)

[...] represent optional parameters that may be omited or nil, in wich case
will be replaced by their default values

-- zlib constants --

int         zlib.XXXX

-- zlib functions --

string      zlib.version()
int         zlib.compile_flags()
int         zlib.adler32(int adler, string buf)
int         zlib.crc32(int crc, string buf)

string      zlib.compress(string buf [, int level] [, int method] [, int windowBits] [, int memLevel] [, int strategy])
string      zlib.uncompress(string buf [, int windowBits])

ds          zlib.deflate_init([int level] [, int method] [, int windowBits] [, int memLevel] [, int strategy])
is          zlib.inflate_init([int windowBits])

-- deflate stream methods --

int         ds:adler()
int         ds:data_type()
int         ds:total_in()
int         ds:total_out()
int, int    df:process(string [, int flush])
void        ds:done()
void        ds:callback(function callback, * userdata)
int         ds:set_dictionary(string dictionary)
ds [,int]   ds:clone()
int         ds:reset()
int         ds:params(int level, int strategy)
int         ds:prime(int bits, int value)

-- inflate stream methods --

int         is:adler()
int         is:data_type()
int         is:total_in()
int         is:total_out()
int, int    if:process(string [, int flush])
void        if:done()
void        is:callback([function callback] [, * userdata])
void        is:dictionary([function callback] [, * userdata])
int         is:set_dictionary(string dictionary)
is [,int]   is:clone()
int         is:reset()
int, int    is:sync(string buf)

-- callbacks --
void        callback(string buf, * userdata)
void        dictionary(* userdata)

-- general description --
most functions/methods correspond to the original function in the zlib
library, the main differences are:

zlib.XXXX constants correspond to the Z_XXXX constants defined in the
zlib library

when (de)compressing blocks, the generated output is sent to the callback
function, this is done to prevent passing too much meaningful result values
from the process method

deflate/inflate zlib functions are interfaced through the process method

ds:params may invoke the output callback

process method returns the error value and the number of input bytes
processed

clone method returns a copy of the stream (also copies callbacks) and
returns a new stream object or nil plus an error code

dictionary callback is not strictly necessary, but may avoid extra process
calls if used, only needed when compressed stream also used a custom
dictionary. when using it you must call is:set_dictionary as needed, if not
you will have to watch the return code for zlib.

is:sync returns an error code and the number of input bytes processed


** for more information please refer to zlib.h **
--]]------------------------------------------------------------------------

require('requirelib')

requirelib('lzlib', 'luaopen_zlib', true)
