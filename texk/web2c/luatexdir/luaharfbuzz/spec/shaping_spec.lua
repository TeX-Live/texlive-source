local harfbuzz = require("harfbuzz")

local compare_glyphs_against_fixture = function(glyphs, fixture)
  local json = require('dkjson')
  local f = io.open("fixtures/"..fixture)
  local s = f:read("*all")
  f:close()
  local hb_shape_glyphs = json.decode(s)
  assert.are_equal(#hb_shape_glyphs, #glyphs)
  for c = 1, #glyphs do
    local g = glyphs[c]
    local h = hb_shape_glyphs[c]
    assert.are_equal(h.g, g.codepoint)
    assert.are_equal(h.cl, g.cluster)
    assert.are_equal(h.ax, g.x_advance)
    assert.are_equal(h.ay, g.y_advance)
    assert.are_equal(h.dx, g.x_offset)
    assert.are_equal(h.dy, g.y_offset)
    assert.are_equal(h.fl, g.flags)
  end
end


describe("harfbuzz module shaping functions", function()
  local face = harfbuzz.Face.new('fonts/notonastaliq.ttf')
  local font = harfbuzz.Font.new(face)
  local urdu_text = "یہ" -- U+06CC U+06C1

  it("can take a buffer and font and shape it, with output matching hb-shape", function()
    local buf = harfbuzz.Buffer.new()
    buf:add_utf8(urdu_text)

    harfbuzz.shape(font, buf)
    local glyphs = buf:get_glyphs()
    assert.True(#glyphs > 0)

    -- Compare against output of hb-shape
    compare_glyphs_against_fixture(glyphs, 'notonastaliq_U06CC_U06C1.json')
  end)

  it("can take a buffer, font and an options table with script, language and direction settings.", function()
    local buf = harfbuzz.Buffer.new()
    buf:add_utf8(urdu_text)

    harfbuzz.shape(font, buf, { language = harfbuzz.Language.new("urd"), script = harfbuzz.Script.new("Arab"), direction = harfbuzz.Direction.HB_DIRECTION_RTL })
    local glyphs = buf:get_glyphs()
    assert.True(#glyphs > 0)

    -- Compare against output of hb-shape
    compare_glyphs_against_fixture(glyphs, 'notonastaliq_U06CC_U06C1.json')
  end)

  it("can take codepoints, font and an options table with script, language and direction settings. #mac", function()
    local buf = harfbuzz.Buffer.new()
    local korean_text = { 0xAC00, 0xB098, 0xB2E4 }
    buf:add_codepoints(korean_text)

    local face_korean = harfbuzz.Face.new('/Library/Fonts/AppleGothic.ttf')
    local font_korean = harfbuzz.Font.new(face_korean)

    harfbuzz.shape(font_korean, buf, { language = harfbuzz.Language.new("KOR"), script = harfbuzz.Script.new("hang"), direction = harfbuzz.Direction.HB_DIRECTION_LTR })
    local glyphs = buf:get_glyphs()
    assert.True(#glyphs > 0)

    -- Compare against output of hb-shape
    compare_glyphs_against_fixture(glyphs, 'AppleGothic_korean_issue_22.json')
  end)

  it("can take a string containing a comma-delimited list of valid features", function()
    local buf = harfbuzz.Buffer.new()
    buf:add_utf8(urdu_text)

    harfbuzz.shape(font, buf, { language = harfbuzz.Language.new("urd"), script = harfbuzz.Script.new("Arab"), direction = harfbuzz.Direction.HB_DIRECTION_RTL, features = "+kern,smcp" })
    local glyphs = buf:get_glyphs()
    assert.True(#glyphs > 0)
  end)

  describe("features option", function()
    local buf
    local options

    before_each(function()
      buf= harfbuzz.Buffer.new()
      buf:add_utf8(urdu_text)
      options = { language = harfbuzz.Language.new("urd"), script = harfbuzz.Script.new("Arab"), direction = harfbuzz.Direction.HB_DIRECTION_RTL }
    end)

    it("can take a table containing a valid features", function()
      options.features = {
        harfbuzz.Feature.new('+kern'),
        harfbuzz.Feature.new('smcp')
      }

      harfbuzz.shape(font, buf, options)
      local glyphs = buf:get_glyphs()
      assert.True(#glyphs > 0)
    end)

    it("throws an error if feature string is invalid", function()
      options.features = "#kern"
      assert.has_error(function()
        harfbuzz.shape(font, buf, options)
      end, "Invalid feature string: '#kern'")
    end)

    it("throws an error if feature option is not a table or string", function()
      options.features = 25
      assert.has_error(function()
        harfbuzz.shape(font, buf, options)
      end, "Invalid features option")
    end)

    it("throws an error if features table does not contain a feature", function()
      options.features = {
        harfbuzz.Feature.new('+kern'),
        25,
        harfbuzz.Feature.new('smcp')
      }
      assert.has_error(function()
        harfbuzz.shape(font, buf, options)
      end, "bad argument #-1 to 'shape_full' (harfbuzz.Feature expected, got number)")
    end)

    it("shapes a string appropriately with the features turned on",function()
      buf= harfbuzz.Buffer.new()
      buf:add_utf8("123")
      local opts = { language = harfbuzz.Language.new("eng"), script = harfbuzz.Script.new("Latn"), direction = harfbuzz.Direction.HB_DIRECTION_LTR }
      local amiri_face = harfbuzz.Face.new('fonts/amiri-regular.ttf')
      local amiri_font = harfbuzz.Font.new(amiri_face)

      -- Check normal shaping w/o features
      buf= harfbuzz.Buffer.new()
      buf:add_utf8("123")

      harfbuzz.shape(amiri_font, buf, opts)
      local glyphs = buf:get_glyphs()
      compare_glyphs_against_fixture(glyphs, "amiri-regular_123.json")

      -- Check shaping with '+numr'
      buf= harfbuzz.Buffer.new()
      buf:add_utf8("123")
      opts.features = "+numr"
      harfbuzz.shape(amiri_font, buf, opts)
      glyphs = buf:get_glyphs()
      compare_glyphs_against_fixture(glyphs, "amiri-regular_123_numr.json")
    end)

    it("can set specefic shaper",function()
      options.shapers = { "fallback"}
      harfbuzz.shape(font, buf, options)
      local glyphs = buf:get_glyphs()
      assert.are_equal(2, #glyphs)
      assert.are_equal(906, glyphs[1].codepoint)
      assert.are_equal(909, glyphs[2].codepoint)
    end)
  end)
end)
