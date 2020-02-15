local harfbuzz = require("harfbuzz")

describe("harfbuzz module", function()

  it("returns a valid version string", function()
    assert.are_equal("string", type(harfbuzz.version()))
  end)

  it("returns a valid list of shapers", function()
    local shapers = { harfbuzz.shapers }
    assert.is_not.True(#shapers == 0)
  end)

  describe("harfbuzz.Blob", function()
    it("can be initialized with a string", function()
      local s = "test string"
      local blob = harfbuzz.Blob.new(s)
      assert.are_equal(string.len(s), blob:get_length())
    end)
  end)

  describe("harfbuzz.Face", function()
    local face = harfbuzz.Face.new('fonts/notonastaliq.ttf')
    it("can be initialized with a blob", function()
      local fontfile = io.open('fonts/notonastaliq.ttf', "r")
      local fontdata = fontfile:read("*all")
      fontfile:close()

      local blob = harfbuzz.Blob.new(fontdata)
      harfbuzz.Face.new_from_blob(blob,0)
    end)

    it("can be initialized with a file and a font index", function()
      harfbuzz.Face.new('fonts/notonastaliq.ttf',0)
    end)

    it("can be initialized with a file only", function()
      harfbuzz.Face.new('fonts/notonastaliq.ttf')
    end)

    it("returns a valid upem value", function()
      assert.are_equal(2048,face:get_upem())
    end)

    it("can return SFNT table", function()
      local b = face:get_table(harfbuzz.Tag.new("OS/2"))
      local d = b:get_data()
      assert.are_equal(96,b:get_length())
      assert.are_equal(96,#d)
      assert.are_equal(4,string.unpack(">H", d, 1))
      assert.are_equal(1155,string.unpack(">h", d, 3))
      assert.are_equal(5,string.unpack(">H", d, -2))
    end)

    it("can return table tags", function()
      local t = face:get_table_tags()
      assert.are_equal(14,#t)
      assert.are_equal("GDEF",tostring(t[1]))
      assert.are_equal("post",tostring(t[#t]))
    end)

    it("can return glyph count", function()
      assert.are_equal(1133,face:get_glyph_count())
    end)

    it("can return unicode characters supported by face", function()
      local u = face:collect_unicodes()
      assert.are_equal(267,#u)
      assert.are_equal(0x0000,u[1])
      assert.are_equal(0xFEFF,u[#u])
    end)

    it("can return face names", function()
      assert.are_equal("Copyright 2014 Google Inc. All Rights Reserved.",face:get_name(harfbuzz.ot.NAME_ID_COPYRIGHT))
      assert.are_equal("Noto Nastaliq Urdu",face:get_name(harfbuzz.ot.NAME_ID_FONT_FAMILY))
      assert.are_equal("Regular",face:get_name(harfbuzz.ot.NAME_ID_FONT_SUBFAMILY))
      assert.are_equal("Noto Nastaliq Urdu",face:get_name(harfbuzz.ot.NAME_ID_FULL_NAME))
      assert.are_equal("NotoNastaliqUrdu",face:get_name(harfbuzz.ot.NAME_ID_POSTSCRIPT_NAME))
      assert.are_equal("Noto is a trademark of Google Inc.",face:get_name(harfbuzz.ot.NAME_ID_TRADEMARK))
      assert.are_equal(331,#face:get_name(harfbuzz.ot.NAME_ID_LICENSE))
      assert.are_equal(nil,face:get_name(harfbuzz.ot.NAME_ID_INVALID))
    end)

    it("can return face names with language", function()
      local f = harfbuzz.Face.new('fonts/amiri-regular.ttf')
      local ar = harfbuzz.Language.new("ar")
      local en = harfbuzz.Language.new("en")
      assert.are_equal("حقوق النشر 2010-2015، خالد حسني <khaledhosny@eglug.org>.",f:get_name(harfbuzz.ot.NAME_ID_COPYRIGHT, ar))
      assert.are_equal("Copyright (c) 2010-2015, Khaled Hosny <khaledhosny@eglug.org>.\nPortions copyright (c) 2010, Sebastian Kosch <sebastian@aldusleaf.org>.",f:get_name(harfbuzz.ot.NAME_ID_COPYRIGHT, en))
      assert.are_equal("عادي",f:get_name(harfbuzz.ot.NAME_ID_FONT_SUBFAMILY, ar))
      assert.are_equal("Regular",f:get_name(harfbuzz.ot.NAME_ID_FONT_SUBFAMILY, en))
      assert.are_equal("إصدارة 000٫108",f:get_name(harfbuzz.ot.NAME_ID_VERSION_STRING, ar))
      assert.are_equal("Version 000.108 ",f:get_name(harfbuzz.ot.NAME_ID_VERSION_STRING, en))
      assert.are_equal("خالد حسني",f:get_name(harfbuzz.ot.NAME_ID_DESIGNER, ar))
      assert.are_equal("Khaled Hosny",f:get_name(harfbuzz.ot.NAME_ID_DESIGNER, en))
      assert.are_equal(512,#f:get_name(harfbuzz.ot.NAME_ID_DESCRIPTION, ar))
      assert.are_equal(263,#f:get_name(harfbuzz.ot.NAME_ID_DESCRIPTION, en))
      assert.are_equal("صِفْ خَلْقَ خَوْدٍ كَمِثْلِ ٱلشَّمْسِ إِذْ بَزَغَتْ يَحْظَىٰ ٱلضَّجِيعُ بِهَا نَجْلَاءَ مِعْطَارِ.",f:get_name(harfbuzz.ot.NAME_ID_SAMPLE_TEXT, ar))
      assert.are_equal("صِفْ خَلْقَ خَوْدٍ كَمِثْلِ ٱلشَّمْسِ إِذْ بَزَغَتْ يَحْظَىٰ ٱلضَّجِيعُ بِهَا نَجْلَاءَ مِعْطَارِ.",f:get_name(harfbuzz.ot.NAME_ID_SAMPLE_TEXT, en))
    end)

    it("can check color palettes", function()
      local f = harfbuzz.Face.new('fonts/amiriquran-colored.ttf')
      assert.are_equal(false,face:ot_color_has_palettes())
      assert.are_equal(true,f:ot_color_has_palettes())
    end)

    it("can return number of color palettes", function()
      local f = harfbuzz.Face.new('fonts/amiriquran-colored.ttf')
      assert.are_equal(0,face:ot_color_palette_get_count())
      assert.are_equal(1,f:ot_color_palette_get_count())
    end)

    it("can return palette colors", function()
      local f = harfbuzz.Face.new('fonts/amiriquran-colored.ttf')
      assert.are_equal(nil,face:ot_color_palette_get_colors())
      local colors = {
        { alpha = 255, blue = 51,  green = 51,  red = 204, },
        { alpha = 255, blue = 80,  green = 165, red = 0,   },
        { alpha = 255, blue = 51,  green = 153, red = 238, },
        { alpha = 255, blue = 153, green = 102, red = 51,  },
      }
      assert.are_same(colors,f:ot_color_palette_get_colors())
    end)

    it("can check color layers", function()
      local f = harfbuzz.Face.new('fonts/amiriquran-colored.ttf')
      assert.are_equal(false,face:ot_color_has_layers())
      assert.are_equal(true,f:ot_color_has_layers())
    end)

    it("can return glyph color layers", function()
      local f = harfbuzz.Face.new('fonts/amiriquran-colored.ttf')
      assert.are_equal(nil,face:ot_color_glyph_get_layers(100))
      assert.are_equal(nil,f:ot_color_glyph_get_layers(2))
      local layers = {
        { color_index = 65535, glyph = 1341 },
        { color_index = 1,     glyph = 1370 },
      }
      assert.are_same(layers,f:ot_color_glyph_get_layers(100))
    end)

    it("can check PNG glyph support", function()
      local f = harfbuzz.Face.new('fonts/notocoloremoji-subset.ttf')
      assert.are_equal(false,face:ot_color_has_png())
      assert.are_equal(true,f:ot_color_has_png())
    end)

    it("can check SVG glyph support", function()
      local f = harfbuzz.Face.new('fonts/TwitterColorEmoji-SVGinOT.ttf')
      assert.are_equal(false,face:ot_color_has_svg())
      assert.are_equal(true,f:ot_color_has_svg())
    end)

    it("can return glyph color png", function()
      local f = harfbuzz.Face.new('fonts/TwitterColorEmoji-SVGinOT.ttf')

      assert.are_equal(nil,face:ot_color_glyph_get_svg(100))
      assert.are_equal(nil,f:ot_color_glyph_get_svg(0))
      assert.are_same(751,f:ot_color_glyph_get_svg(5):get_length())
      assert.are_same(804,f:ot_color_glyph_get_svg(6):get_length())
      assert.are_same("<?xml version='1.0' encoding='UTF-8'?>",f:ot_color_glyph_get_svg(5):get_data():sub(1, 38))
    end)

    it("can return script tags", function()
      local t
      local tags = {
        harfbuzz.Tag.new("arab"),
        harfbuzz.Tag.new("dflt"),
        harfbuzz.Tag.new("latn"),
      }
      t = face:ot_layout_get_script_tags(harfbuzz.Tag.new("GSUB"))
      assert.are_same(tags, t)
      t = face:ot_layout_get_script_tags(harfbuzz.Tag.new("GPOS"))
      assert.are_same({ tags[1] }, t)
    end)

    it("can return language tags", function()
      local t
      local tags = {
        harfbuzz.Tag.new("ARA "),
        harfbuzz.Tag.new("FAR "),
        harfbuzz.Tag.new("KSH "),
        harfbuzz.Tag.new("SND "),
        harfbuzz.Tag.new("URD "),
      }
      t = face:ot_layout_get_language_tags(harfbuzz.Tag.new("GSUB"), 0)
      assert.are_same(tags, t)
      t = face:ot_layout_get_language_tags(harfbuzz.Tag.new("GPOS"), 0)
      assert.are_equal(nil, t)
    end)

    it("can return feature tags", function()
      local t, tags
      tags = {
        harfbuzz.Tag.new("ccmp"),
        harfbuzz.Tag.new("isol"),
        harfbuzz.Tag.new("init"),
        harfbuzz.Tag.new("medi"),
        harfbuzz.Tag.new("fina"),
        harfbuzz.Tag.new("rlig"),
      }
      t = face:ot_layout_get_feature_tags(harfbuzz.Tag.new("GSUB"), 0, 0)
      assert.are_same(tags, t)
      tags = {
        harfbuzz.Tag.new("curs"),
        harfbuzz.Tag.new("mark"),
        harfbuzz.Tag.new("mkmk"),
      }
      t = face:ot_layout_get_feature_tags(harfbuzz.Tag.new("GPOS"), 0, harfbuzz.ot.LAYOUT_DEFAULT_LANGUAGE_INDEX)
      assert.are_same(tags, t)
    end)

    it("can find scripts, languages and features", function()
      local r, i
      r, i = face:ot_layout_find_script(harfbuzz.Tag.new("GSUB"), harfbuzz.Tag.new("latn"))
      assert.True(r)
      assert.are_same(2, i)
      r, i = face:ot_layout_find_language(harfbuzz.Tag.new("GSUB"), i, harfbuzz.Tag.new("ENG "))
      assert.False(r)
      assert.are_same(harfbuzz.ot.LAYOUT_DEFAULT_LANGUAGE_INDEX, i)
      r, i = face:ot_layout_find_language(harfbuzz.Tag.new("GSUB"), 0, harfbuzz.Tag.new("ARA "))
      assert.True(r)
      assert.are_same(0, i)
      r, i = face:ot_layout_find_feature(harfbuzz.Tag.new("GSUB"), 0, i, harfbuzz.Tag.new("rlig"))
      assert.True(r)
      assert.are_same(13, i)

      r, i = face:ot_layout_find_feature(harfbuzz.Tag.new("GSUB"), 1, harfbuzz.ot.LAYOUT_DEFAULT_LANGUAGE_INDEX, harfbuzz.Tag.new("rlig"))
      assert.True(r)
      assert.are_same(13, i)
    end)
  end)

  describe("harfbuzz.Font", function()
    local face = harfbuzz.Face.new('fonts/notonastaliq.ttf')
    it("can be initialized with a face", function()
      harfbuzz.Font.new(face)
    end)

    it("has a default scale set to the fonts upem", function()
      local font = harfbuzz.Font.new(face)
      local upem = face:get_upem()
      local xs, ys = font:get_scale()
      assert.are_equal(upem, xs)
      assert.are_equal(upem, ys)
    end)

    it("can set the scale of the font using set_scale", function()
      local font = harfbuzz.Font.new(face)
      font:set_scale(1024,2048)
      local xs, ys = font:get_scale()
      assert.are_equal(1024, xs)
      assert.are_equal(2048, ys)
    end)

    it("can get glyph extents using get_glyph_extents", function()
      local font = harfbuzz.Font.new(face)
      local extents = font:get_glyph_extents(0)
      assert.are_equal(145, extents.x_bearing)
      assert.are_equal(2452, extents.y_bearing)
      assert.are_equal(1553, extents.width)
      assert.are_equal(-2452, extents.height)
      extents = font:get_glyph_extents(1)
      assert.are_equal(0, extents.x_bearing)
      assert.are_equal(0, extents.y_bearing)
      assert.are_equal(0, extents.width)
      assert.are_equal(0, extents.height)
    end)

    it("can get font extents", function()
      local font = harfbuzz.Font.new(face)
      local extents = font:get_h_extents(0)
      assert.are_equal(3900, extents.ascender)
      assert.are_equal(-1220, extents.descender)
      assert.are_equal(0, extents.line_gap)
      extents = font:get_v_extents(1)
      assert.are_equal(nil, extents)
    end)

    it("can get glyph name using get_glyph_name", function()
      local font = harfbuzz.Font.new(face)
      assert.are_equal(".notdef", font:get_glyph_name(0))
      assert.are_equal("null", font:get_glyph_name(1))
    end)

    it("can get glyph using get_glyph_from_name", function()
      local font = harfbuzz.Font.new(face)
      assert.are_equal(0, font:get_glyph_from_name(".notdef"))
      assert.are_equal(1, font:get_glyph_from_name("null"))
    end)

    it("can get glyph advance using get_glyph_h_advance", function()
      local font = harfbuzz.Font.new(face)
      assert.are_equal(1843, font:get_glyph_h_advance(0))
      assert.are_equal(0, font:get_glyph_h_advance(1))
    end)

    it("can get glyph advance using get_glyph_v_advance", function()
      local font = harfbuzz.Font.new(face)
      assert.are_equal(-2048, font:get_glyph_v_advance(0))
      assert.are_equal(-2048, font:get_glyph_v_advance(1))
    end)

    it("can get nominal glyph for codepoint", function()
      local font = harfbuzz.Font.new(face)
      assert.are_equal(nil, font:get_nominal_glyph(0x0041))
      assert.are_equal(858, font:get_nominal_glyph(0x0627))
    end)

    it("can return glyph color png", function()
      local font = harfbuzz.Font.new(face)
      local f = harfbuzz.Font.new(harfbuzz.Face.new('fonts/notocoloremoji-subset.ttf'))

      assert.are_equal(nil,font:ot_color_glyph_get_png(100))
      assert.are_equal(nil,f:ot_color_glyph_get_png(0))
      assert.are_same(2233,f:ot_color_glyph_get_png(1):get_length())
      assert.are_same(2857,f:ot_color_glyph_get_png(2):get_length())
      assert.are_same("\137PNG",f:ot_color_glyph_get_png(2):get_data():sub(1, 4))
    end)
  end)

  describe("harfbuzz.Feature", function()
    it("can be initialised with a valid feature string", function()
      harfbuzz.Feature.new('kern')
      harfbuzz.Feature.new('+kern')
    end)

    it("throws an error when trying to initialise a new Feature with an invalid string", function()
       assert.are_equal(nil, harfbuzz.Feature.new(''))
       assert.are_equal(nil, harfbuzz.Feature.new('#kern'))
    end)

    it("has a valid tostring value", function()
      local fs = 'kern'
      local f = harfbuzz.Feature.new(fs)
      assert.are_equal(fs, tostring(f))
    end)

    it("has visible fields", function()
      local f = harfbuzz.Feature.new('-kern')
      print(getmetatable(f).__index)
      assert.are_equal(tostring(f.tag), 'kern')
      assert.are_equal(f.value, 0)
      assert.are_equal(f.start, nil)
      assert.are_equal(f._end, nil)

      f = harfbuzz.Feature.new('aalt[3:5]=4')
      assert.are_equal(tostring(f.tag), 'aalt')
      assert.are_equal(f.value, 4)
      assert.are_equal(f.start, 3)
      assert.are_equal(f._end, 5)
    end)

    it("has editable fields", function()
      local f = harfbuzz.Feature.new('-kern')
      f.tag, f.value, f.start, f._end = harfbuzz.Tag.new"aalt", 4, 3, 5
      assert.are_equal(tostring(f), "aalt[3:5]=4")

      f.tag, f.value, f.start, f._end = harfbuzz.Tag.new"harf", 0, nil, nil
      assert.are_equal(tostring(f), "-harf")
    end)
  end)

  describe("harfbuzz.Tag", function()
    it("can be initialised with a valid tag string", function()
      harfbuzz.Tag.new('Zyyy')
    end)

    it("can be initialised to NONE with nil or empty argument", function()
      local t = harfbuzz.Tag.new()
      assert.are_equal(harfbuzz.Tag.NONE, t)
      t = harfbuzz.Tag.new(nil)
      assert.are_equal(harfbuzz.Tag.NONE, t)
    end)

    it("has a valid tostring value", function()
      local ts = 'Arab'
      local t = harfbuzz.Tag.new(ts)
      assert.are_equal(ts, tostring(t))
    end)

    it("has a valid equality check functions", function()
      local ts = 'Arab'
      local t1 = harfbuzz.Tag.new(ts)
      local t2 = harfbuzz.Tag.new(ts)
      local t3 = harfbuzz.Tag.new("Latn")
      assert.are_equal(t1, t2)
      assert.are_not_equal(t1, t3)
    end)

    it("has a preset value for NONE", function()
      local n = harfbuzz.Tag.NONE
      assert.is_not_nil(n)
      assert.are_equal("", tostring(n))
      assert.are_equal(harfbuzz.Tag.NONE, harfbuzz.Tag.new(""))
    end)
  end)

  describe("harfbuzz.Script", function()
    it("can be initialised with a string", function()
      harfbuzz.Script.new('Arab')
    end)

    it("can be initialised to INVALID with nil or empty argument", function()
      local t = harfbuzz.Script.new()
      assert.are_equal(harfbuzz.Script.INVALID, t)
      t = harfbuzz.Script.new(nil)
      assert.are_equal(harfbuzz.Script.INVALID, t)
    end)

    it("can be initialised with a tag", function()
      local ts = "Arab"
      local s = harfbuzz.Script.from_iso15924_tag(harfbuzz.Tag.new(ts))
      assert.are_equal(ts, tostring(s))
    end)

    it("can be converted to a tag", function()
      local s = 'Arab'
      local sc = harfbuzz.Script.new(s)
      assert.are_equal(s, tostring(sc:to_iso15924_tag()))
    end)

    it("has a valid tostring value", function()
      local ts = 'Arab'
      local t = harfbuzz.Script.new(ts)
      assert.are_equal(ts, tostring(t))
    end)

    it("has a valid equality check functions", function()
      local ts = 'Arab'
      local t1 = harfbuzz.Script.new(ts)
      local t2 = harfbuzz.Script.new(ts)
      local t3 = harfbuzz.Script.new("Latn")
      assert.are_equal(t1, t2)
      assert.are_not_equal(t1, t3)
    end)
  end)

  describe("harfbuzz.Direction", function()
    it("can be initialised with a valid tag string", function()
      harfbuzz.Direction.new('LTR')
    end)

    it("can be initialised with invalid strings", function()
      local d1 = harfbuzz.Direction.new("i")
      local d2 = harfbuzz.Direction.new("inv")

      assert.are_equal(d1, d2)
      assert.are_equal("invalid", tostring(d1))
    end)

    it("has a valid tostring value", function()
      local ts = 'ltr'
      local t = harfbuzz.Direction.new(ts)
      assert.are_equal(ts, tostring(t))

      t = harfbuzz.Direction.new("LTR")
      assert.are_equal(ts, tostring(t))
    end)

    it("has a valid equality check functions", function()
      local ts = 'ltr'
      local t1 = harfbuzz.Direction.new(ts)
      local t2 = harfbuzz.Direction.new(ts)
      local t3 = harfbuzz.Direction.new("rtl")
      assert.are_equal(t1, t2)
      assert.are_not_equal(t1, t3)
    end)

    it("has a is_valid function", function()
      assert.True(harfbuzz.Direction.LTR:is_valid())
      assert.False(harfbuzz.Direction.INVALID:is_valid())
    end)

    it("has a is_horizontal function", function()
      assert.True(harfbuzz.Direction.LTR:is_horizontal())
      assert.False(harfbuzz.Direction.TTB:is_horizontal())
    end)

    it("has a is_vertical function", function()
      assert.True(harfbuzz.Direction.TTB:is_vertical())
      assert.False(harfbuzz.Direction.LTR:is_vertical())
    end)

    it("has a is_forward function", function()
      assert.True(harfbuzz.Direction.LTR:is_forward())
      assert.False(harfbuzz.Direction.RTL:is_forward())
    end)

    it("has a is_backward function", function()
      assert.True(harfbuzz.Direction.RTL:is_backward())
      assert.False(harfbuzz.Direction.LTR:is_backward())
    end)
  end)

  describe("harfbuzz.Language", function()
    it("can be initialised with a valid language string", function()
      harfbuzz.Language.new('urd')
    end)

    it("can be initialised to INVALID with nil or empty argument", function()
      local t = harfbuzz.Language.new()
      assert.are_equal(harfbuzz.Language.INVALID, t)
      t = harfbuzz.Language.new(nil)
      assert.are_equal(harfbuzz.Language.INVALID, t)
    end)

    it("has a valid tostring value", function()
      local ts = 'urd'
      local t = harfbuzz.Language.new(ts)
      assert.are_equal(ts, tostring(t))
    end)

    it("has a valid equality check functions", function()
      local ts = 'urd'
      local t1 = harfbuzz.Language.new(ts)
      local t2 = harfbuzz.Language.new(ts)
      local t3 = harfbuzz.Language.new("hin")
      assert.are_equal(t1, t2)
      assert.are_not_equal(t1, t3)
    end)

    it("has a preset value for INVALID", function()
      local n = harfbuzz.Language.INVALID
      assert.is_not_nil(n)
      assert.are_equal(harfbuzz.Language.INVALID, harfbuzz.Language.new())
      assert.are_equal("", tostring(n))
    end)
  end)

  describe("harfbuzz.unicode", function()
    describe("script function returns a valid script for a codepoint",function()
      local s = harfbuzz.unicode.script(0x0020)
      assert.are_equal(harfbuzz.Script.COMMON, s)
      s = harfbuzz.unicode.script(0x0041)
      assert.are_equal(harfbuzz.Script.new("Latn"), s)
    end)
  end)
end)

