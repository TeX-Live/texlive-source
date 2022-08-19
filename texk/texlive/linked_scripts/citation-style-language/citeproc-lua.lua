#!/usr/bin/env texlua

--
-- Copyright (c) 2021-2022 Zeping Lee
-- Released under the MIT license.
-- Repository: https://github.com/zepinglee/citeproc-lua
--

kpse.set_program_name("luatex")

local cli = require("citeproc-cli")
cli.main()
