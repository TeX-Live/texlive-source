--
-- Copyright (c) 2021-2022 Zeping Lee
-- Released under the MIT license.
-- Repository: https://github.com/zepinglee/citeproc-lua
--

local citeproc = {}

local engine = require("citeproc-engine")
local bib = require("citeproc-bib")
local util = require("citeproc-util")

citeproc.__VERSION__ = "0.1.1"

citeproc.new = engine.CiteProc.new
citeproc.parse_bib = bib.parse
citeproc.util = util

return citeproc
