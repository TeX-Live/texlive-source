package.path = "./src/?.lua;./src/?/init.lua;" .. package.path

local harfbuzzsubset = require("harfbuzzsubset")
local harfbuzz = require("harfbuzz")

describe("harfbuzz module", function()
    it("can be required when subset module is also required", function()
        assert.is_not_nil(harfbuzz)
        assert.is_not_nil(harfbuzzsubset)
    end)
end)


describe("harfbuzzsubset basic usage", function()
    local hb, hb_subset
    local font_path = "fonts/AdventPro-VariableFont_wdth,wght.ttf"

    setup(function()
        hb = require "harfbuzz"
        hb_subset = require "harfbuzzsubset"
    end)

    it("returns a valid version string", function()
      assert.are_equal("string", type(hb_subset.version()))
    end)

    it("creates a subset input object", function()
        local input = hb_subset.SubsetInput.new()
        assert.truthy(input)
        -- minimal test of methods existing
        assert.is_function(input.keep_everything)
        assert.is_function(input.pin_axis_location)
    end)

    it("can subset a variable font without errors", function()
        local face = hb.Face.new(font_path)
        local wght = hb.Tag.new("wght")
        local wdth = hb.Tag.new("wdth")

        local input = hb_subset.SubsetInput.new()
        input:pin_axis_location(face, wght, 100)
        input:pin_axis_location(face, wdth, 100)
        input:keep_everything()

        local ok, new_face = pcall(function()
            return hb_subset.subset(face, input)
        end)

        assert.is_true(ok)
        assert.truthy(new_face)
        assert.are_not.equal(face, new_face)
    end)

    it("produces a non-empty blob after subsetting", function()
        local face = hb.Face.new(font_path)
        local wght = hb.Tag.new("wght")
        local wdth = hb.Tag.new("wdth")

        local input = hb_subset.SubsetInput.new()
        input:pin_axis_location(face, wght, 100)
        input:pin_axis_location(face, wdth, 100)
        input:keep_everything()

        local new_face = hb_subset.subset(face, input)

        local blob = new_face:blob()
        assert.truthy(blob)
        assert.is_function(blob.get_data)

        local data = blob:get_data()
        assert.is_string(data)
        assert.is_true(#data > 0)
    end)

    it("preserves glyph count when keep_everything is used", function()
        local face = hb.Face.new(font_path)
        local orig_glyphs = face:get_glyph_count()

        local wght = hb.Tag.new("wght")
        local wdth = hb.Tag.new("wdth")
        local input = hb_subset.SubsetInput.new()
        input:pin_axis_location(face, wght, 100)
        input:pin_axis_location(face, wdth, 100)
        input:keep_everything()

        local new_face = hb_subset.subset(face, input)
        local subset_glyphs = new_face:get_glyph_count()

        assert.equals(orig_glyphs, subset_glyphs)
    end)

    it("produces different blobs for different axis locations", function()
        local face = hb.Face.new(font_path)
        local wght = hb.Tag.new("wght")
        local wdth = hb.Tag.new("wdth")

        local input1 = hb_subset.SubsetInput.new()
        input1:pin_axis_location(face, wght, 100)
        input1:pin_axis_location(face, wdth, 100)
        input1:keep_everything()
        local face_light = hb_subset.subset(face, input1)
        local blob_light = face_light:blob():get_data()

        local input2 = hb_subset.SubsetInput.new()
        input2:pin_axis_location(face, wght, 900)
        input2:pin_axis_location(face, wdth, 100)
        input2:keep_everything()
        local face_bold = hb_subset.subset(face, input2)
        local blob_bold = face_bold:blob():get_data()

        assert.is_true(
            blob_light ~= blob_bold,
            ("expected different blobs for axis locations, but they are identical (len=%d bytes)")
            :format(#blob_light)
        )
        assert.is_true(#blob_light > 0, "blob_light is empty")
        assert.is_true(#blob_bold > 0, "blob_bold is empty")
    end)

    it("reduces glyph count when subsetting to a single Unicode", function()
        local face = hb.Face.new(font_path)
        local orig_glyphs = face:get_glyph_count()

        local input = hb_subset.SubsetInput.new()

        local uset = input:unicode_set()
        uset:add(0x41) -- oder string.byte("A")

        local new_face = hb_subset.subset(face, input)
        local subset_glyphs = new_face:get_glyph_count()

        -- Sanity-Checks mit vernünftiger Fehlermeldung, ohne Binärmüll:
        assert.is_true(
            subset_glyphs > 0,
            ("expected subset glyph count > 0, got %d"):format(subset_glyphs)
        )
        assert.is_true(
            subset_glyphs < orig_glyphs,
            ("expected fewer glyphs after subsetting, got %d (orig %d)")
            :format(subset_glyphs, orig_glyphs)
        )
    end)
end)
