local harfbuzz = require('harfbuzz')
local serpent  = require('serpent') -- luarocks install serpent

-- Harfbuzz API Version
print("Harfbuzz API version", harfbuzz.version())

-- Shapers available
print("Shapers:", serpent.line({ harfbuzz.shapers() }, {comment = false}))

-- harfbuzz.Face
local face = harfbuzz.Face.new('../fonts/notonastaliq.ttf')
print('\nFace upem = '..face:get_upem())

-- harfbuzz.Font
local font = harfbuzz.Font.new(face)
local xs, xy = font:get_scale()
print("\nDefault font scale = X: "..xs..", Y: "..xy)

-- harfbuzz.Buffer
local text = "یہ" -- U+06CC U+06C1
local buf = harfbuzz.Buffer.new()
buf:add_utf8(text)

-- harfbuzz.shape (Shapes text)
print("\nShaping '"..text.."' set with Noto Nastaliq Urdu")
harfbuzz.shape(font, buf, { language = harfbuzz.Language.new("urd"), script = harfbuzz.Script.new("Arab"), direction = harfbuzz.Direction.RTL})

local glyphs = buf:get_glyphs()
print("No. of glyphs", #glyphs)
print(serpent.line(glyphs, {comment = false}))

local opts = { language = harfbuzz.Language.new("eng"), script = harfbuzz.Script.new("Latn"), direction = harfbuzz.Direction.LTR }
local amiri_face = harfbuzz.Face.new('../fonts/amiri-regular.ttf')
local amiri_font = harfbuzz.Font.new(amiri_face)

-- shaping '123' w/o features
print("\nShaping '123' set with Amiri Regular and no features")
buf= harfbuzz.Buffer.new()
buf:add_utf8("123")
harfbuzz.shape(amiri_font, buf, opts)
glyphs = buf:get_glyphs()
print(serpent.line(glyphs, {comment = false}))

-- shaping '123' with '+numr' (numerators)
print("\nShaping '123' set with Amiri Regular with 'numr' feature turned on")
buf= harfbuzz.Buffer.new()
buf:add_utf8("123")
opts.features = "+numr"
harfbuzz.shape(amiri_font, buf, opts)
glyphs = buf:get_glyphs()
print(serpent.line(glyphs, {comment = false}))

