rockspec_format = "3.0"
package = "luaharfbuzz"
version = "scm-1"
source = {
  url = "git://github.com/ufytex/luaharfbuzz"
}
description = {
  summary = "Lua bindings for the Harfbuzz text shaping library",
  homepage = "https://github.com/ufytex/luaharfbuzz",
  license = "MIT",
  maintainer = "Deepak Jois <deepak.jois@gmail.com>",
  issues_url = "https://github.com/ufytex/luaharfbuzz/issues",
  labels = {"unicode", "fonts"}
}
dependencies = {
  "lua >= 5.2"
}
build = {
  type = "builtin",
  modules = {
    harfbuzz ="src/harfbuzz.lua",
    luaharfbuzz= {
      sources = {
      "src/luaharfbuzz/luaharfbuzz.c",
      "src/luaharfbuzz/blob.c",
      "src/luaharfbuzz/face.c",
      "src/luaharfbuzz/font.c",
      "src/luaharfbuzz/buffer.c",
      "src/luaharfbuzz/feature.c",
      "src/luaharfbuzz/tag.c",
      "src/luaharfbuzz/ot.c",
      "src/luaharfbuzz/unicode.c",
      "src/luaharfbuzz/script.c",
      "src/luaharfbuzz/direction.c",
      "src/luaharfbuzz/language.c",
      "src/luaharfbuzz/class_utils.c"
      },
      libraries = {"harfbuzz"},
      incdirs = {"$(HARFBUZZ_INCDIR)/harfbuzz"},
      libdirs = {"$(HARFBUZZ_LIBDIR)"}
    }
  }
}
external_dependencies = {
   HARFBUZZ = {
      header = "harfbuzz/hb.h"
   }
}
