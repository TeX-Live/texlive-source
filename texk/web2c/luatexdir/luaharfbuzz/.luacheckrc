cache = true
max_line_length = false
files["spec/"].std = "+busted"
files["examples/luatex/"].globals = {
  "texio",
  "fontloader",
  "callback",
  "tex",
  "node",
  "read_font",
  "unicode",
  "font"
}

