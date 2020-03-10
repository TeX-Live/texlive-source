local harfbuzz = require("harfbuzz")

describe("harfbuzz.Buffer", function()
  it("can be created", function()
    harfbuzz.Buffer.new()
  end)

  it("can add a single codepoints with explicit cluster value", function()
    local b = harfbuzz.Buffer.new()
    b:add(0x06CC, 42)
    local glyphs = b:get_glyphs()
    assert.are_equal(#glyphs, 1)
    assert.are_equal(glyphs[1].cluster, 42)
    assert.are_equal(glyphs[1].codepoint, 0x06CC)
  end)

  it("can add a UTF8 string", function()
    local b = harfbuzz.Buffer.new()
    local s = "Some String"
    b:add_utf8(s)
    assert.are_equal(string.len(s), b:get_length())
  end)

  it("can add a UTF 8 string with item_offset", function()
    local b = harfbuzz.Buffer.new()
    local s = "Some String"
    local o = 5
    b:add_utf8(s,o)
    assert.are_equal(string.len(s) - o, b:get_length())
  end)

  it("can add a UTF 8 string with item_length", function()
    local b = harfbuzz.Buffer.new()
    local s = "Some String"
    local o = 5
    local l = 2
    b:add_utf8(s,o,l)
    assert.are_equal(l, b:get_length())
  end)

  it("can add codepoints", function()
    local b = harfbuzz.Buffer.new()
    local s = { 0x06CC, 0x06C1 }
    b:add_codepoints(s)
    assert.are_equal(#s, b:get_length())
  end)

  it("can add codepoints with item_offset", function()
    local b = harfbuzz.Buffer.new()
    local s = { 0x06CC, 0x06C1 }
    local o = 1
    b:add_codepoints(s,o)
    assert.are_equal(#s - o, b:get_length())
  end)

  it("can add codepoints with item_length", function()
    local b = harfbuzz.Buffer.new()
    local s = { 0x06CC, 0x06C1 }
    local o = 1
    local l = 1
    b:add_codepoints(s,o,l)
    assert.are_equal(l, b:get_length())
  end)

  it("can call guess_segment_properties", function()
    local b = harfbuzz.Buffer.new()
    b:add_utf8("Some String")
    b:guess_segment_properties()
  end)

  it("can get and set the direction of a buffer", function()
    local b = harfbuzz.Buffer.new()
    b:add_utf8("abc")
    local dir = harfbuzz.Direction.RTL
    b:set_direction(dir)
    assert.are_equal(dir, b:get_direction())
  end)

  it("sets direction to INVALID if direction is invalid", function()
    local b = harfbuzz.Buffer.new()
    b:set_direction(harfbuzz.Direction.new("invalid"))
    assert.are_equal(harfbuzz.Direction.INVALID, b:get_direction())
  end)

  it("can get the direction correctly", function()
    local b = harfbuzz.Buffer.new()
    b:add_utf8("یہ")
    b:guess_segment_properties()
    assert.are_equal(harfbuzz.Direction.RTL, b:get_direction())
  end)

  it("can get and set the language of a buffer", function()
    local b = harfbuzz.Buffer.new()
    b:add_utf8("یہ")
    local urd = harfbuzz.Language.new("urd")
    b:set_language(urd)
    assert.are_equal(urd, b:get_language())
  end)

  it("Sets language to INVALID if language is invalid", function()
    local b = harfbuzz.Buffer.new()
    b:set_language(harfbuzz.Language.INVALID)
    assert.are_equal(harfbuzz.Language.INVALID, b:get_language())
  end)

  it("can get the language correctly", function()
    local b = harfbuzz.Buffer.new()
    b:add_utf8("یہ")
    b:guess_segment_properties()
    assert.are_not_equal(harfbuzz.Language.INVALID, b:get_language())
  end)

  it("can get and set the script of a buffer", function()
    local b = harfbuzz.Buffer.new()
    b:add_utf8("abc")
    b:set_script(harfbuzz.Script.new("latn"))
    assert.are_equal("Latn", tostring(b:get_script()))
  end)

  it("returns script as UNKNOWN if script is invalid", function()
    local b = harfbuzz.Buffer.new()
    b:set_script(harfbuzz.Script.new("xxx"))
    assert.are_equal(harfbuzz.Script.UNKNOWN, b:get_script())
  end)

  it("can get the script correctly", function()
    local b = harfbuzz.Buffer.new()
    b:add_utf8("یہ")
    assert.are_equal(harfbuzz.Script.new(""), b:get_script())
    b:guess_segment_properties()
    assert.are_equal(harfbuzz.Script.new("Arab"), b:get_script())
  end)

  it("can reverse the buffer", function()
    local face = harfbuzz.Face.new('fonts/notonastaliq.ttf')
    local font = harfbuzz.Font.new(face)
    local urdu_text = "یہ" -- U+06CC U+06C1
    local options = { language = harfbuzz.Language.new("urd"), script = harfbuzz.Script.new("Arab"), direction = harfbuzz.Direction.RTL }

    local buf= harfbuzz.Buffer.new()
    buf:add_utf8(urdu_text)
    harfbuzz.shape(font, buf, options)
    local orig_glyphs = buf:get_glyphs()
    buf:reverse()
    local reversed_glyphs = buf:get_glyphs()

    assert.are_equal(#orig_glyphs, #reversed_glyphs)

    for c = 1, #orig_glyphs do
      local g = orig_glyphs[#orig_glyphs - (c - 1)]
      local r = reversed_glyphs[c]
      assert.are_equal(g.codepoint, r.codepoint)
      assert.are_equal(g.cluster, r.cluster)
      assert.are_equal(g.x_advance, r.x_advance)
      assert.are_equal(g.y_advance, r.y_advance)
      assert.are_equal(g.x_offset, r.x_offset)
      assert.are_equal(g.y_offset, r.y_offset)
    end

  end)

  it("can get the length of the buffer", function()
    local b = harfbuzz.Buffer.new()
    local s = "some string"
    b:add_utf8(s)
    assert.are_equal(string.len(s), b:get_length())
  end)

  it("can get the cluster level of the buffer", function()
    local b = harfbuzz.Buffer.new()
    assert.are_equal(harfbuzz.Buffer.CLUSTER_LEVEL_DEFAULT, b:get_cluster_level())
  end)

  it("can set the cluster level of the buffer", function()
    local b = harfbuzz.Buffer.new()
    b:set_cluster_level(harfbuzz.Buffer.CLUSTER_LEVEL_CHARACTERS)
    assert.are_equal(harfbuzz.Buffer.CLUSTER_LEVEL_CHARACTERS, b:get_cluster_level())
  end)

  it("can get the replacement glyph for invisible characters of the buffer", function()
    local b = harfbuzz.Buffer.new()
    assert.are_equal(0, b:get_invisible_glyph())
  end)

  it("can set the replacement glyph for invisible characters of the buffer", function()
    local b = harfbuzz.Buffer.new()
    b:set_invisible_glyph(3)
    assert.are_equal(3, b:get_invisible_glyph())
  end)

  it("can get the replacement codepoint of the buffer", function()
    local b = harfbuzz.Buffer.new()
    assert.are_equal(0xFFFD, b:get_replacement_codepoint())
  end)

  it("can set the replacement codepoint of the buffer", function()
    local b = harfbuzz.Buffer.new()
    b:set_replacement_codepoint(0xFFFF)
    assert.are_equal(0xFFFF, b:get_replacement_codepoint())
    b:add_utf8("\xFF")
    local glyphs = b:get_glyphs()
    assert.are_equal(1, #glyphs)
    assert.are_equal(0xFFFF, glyphs[1].codepoint)
  end)

  it("can get the flags of the buffer", function()
    local b = harfbuzz.Buffer.new()
    assert.are_equal(harfbuzz.Buffer.FLAG_DEFAULT, b:get_flags())
  end)

  it("can set the flags of the buffer", function()
    local b = harfbuzz.Buffer.new()
    b:set_flags(harfbuzz.Buffer.FLAG_DEFAULT | harfbuzz.Buffer.FLAG_BOT)
    assert.are_equal(harfbuzz.Buffer.FLAG_DEFAULT | harfbuzz.Buffer.FLAG_BOT, b:get_flags())
  end)

  it("can clear the buffer", function()
    local b = harfbuzz.Buffer.new()
    b:add_utf8("abc")
    b:set_flags(harfbuzz.Buffer.FLAG_DEFAULT | harfbuzz.Buffer.FLAG_BOT)
    b:clear_contents()
    assert.are_equal(0, b:get_length())
    assert.are_equal(harfbuzz.Buffer.FLAG_DEFAULT | harfbuzz.Buffer.FLAG_BOT, b:get_flags())
  end)

  it("can reset the buffer", function()
    local b = harfbuzz.Buffer.new()
    b:add_utf8("abc")
    b:set_flags(harfbuzz.Buffer.FLAG_DEFAULT | harfbuzz.Buffer.FLAG_BOT)
    b:reset()
    assert.are_equal(0, b:get_length())
    assert.are_equal(harfbuzz.Buffer.FLAG_DEFAULT, b:get_flags())
  end)
end)


