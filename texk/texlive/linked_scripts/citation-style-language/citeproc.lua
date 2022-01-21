--[[
  Copyright (C) 2021 Zeping Lee
--]]


local citeproc = {}

local engine = require("citeproc-engine")
local bib = require("citeproc-bib")
local util = require("citeproc-util")

citeproc.__VERSION__ = "0.1.0"

citeproc.new = engine.CiteProc.new
citeproc.parse_bib = bib.parse
citeproc.util = util

return citeproc
