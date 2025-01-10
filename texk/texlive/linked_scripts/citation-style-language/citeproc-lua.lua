#!/usr/bin/env texlua

--
-- Copyright (c) 2021-2025 Zeping Lee
-- Released under the MIT license.
-- Repository: https://github.com/zepinglee/citeproc-lua
--

local using_luatex, kpse = pcall(require, "kpse")
if not using_luatex then
  error("citeproc-lua must be run with LuaTeX")
end

kpse.set_program_name("texlua", "citeproc-lua")

local cli = require("citeproc-cli")
os.exit(cli.main())
