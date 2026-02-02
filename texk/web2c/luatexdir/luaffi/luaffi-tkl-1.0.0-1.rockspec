package = "luaffi-tkl"
version = "1.0.0-1"

source = {
   url = "git+https://github.com/Tekenlight/luaffifb.git",
   tag = "v1.0.2"
}

description = {
   summary = "FFI library for calling C functions from lua",
   detailed = [[
   ]],
   homepage = "https://github.com/Tekenlight/luaffifb",
   license = "BSD"
}

dependencies = {
   "lua >= 5.1",
}

build = {
   type = "builtin",
   modules = {
      ['ffi'] = {
         incdirs = {
            "dynasm"
         },
         sources = {
            "call.c", "ctype.c", "ffi.c", "parser.c",
         }
      },
      ['ffi_test.libtest'] = 'test.c',
      ['ffi_test.test'] = 'test.lua',
   }
}
