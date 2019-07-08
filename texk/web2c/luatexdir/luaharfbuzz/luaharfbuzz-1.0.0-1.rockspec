package = "luaharfbuzz"
version = "1.0.0-1"
source = {
   url = "git://github.com/deepakjois/luaharfbuzz",
   tag = "v1.0.0"
}
description = {
   summary = "Lua bindings for the Harfbuzz text shaping library",
   homepage = "https://github.com/deepakjois/luaharfbuzz",
   license = "MIT",
   maintainer = "Deepak Jois <deepak.jois@gmail.com>"
}
dependencies = {
   "lua >= 5.2"
}
external_dependencies = {
   HARFBUZZ = {
      header = "harfbuzz/hb.h"
   }
}
build = {
   type = "builtin",
   modules = {
      harfbuzz = "src/harfbuzz.lua",
      luaharfbuzz = {
         incdirs = {
            "$(HARFBUZZ_INCDIR)/harfbuzz"
         },
         libdirs = {
            "$(HARFBUZZ_LIBDIR)"
         },
         libraries = {
            "harfbuzz"
         },
         sources = {
            "src/luaharfbuzz/luaharfbuzz.c",
            "src/luaharfbuzz/blob.c",
            "src/luaharfbuzz/face.c",
            "src/luaharfbuzz/font.c",
            "src/luaharfbuzz/buffer.c",
            "src/luaharfbuzz/feature.c",
            "src/luaharfbuzz/tag.c",
            "src/luaharfbuzz/unicode.c",
            "src/luaharfbuzz/script.c",
            "src/luaharfbuzz/direction.c",
            "src/luaharfbuzz/language.c",
            "src/luaharfbuzz/class_utils.c"
         }
      }
   }
}
