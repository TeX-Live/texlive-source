#!/usr/bin/env texlua

--
-- Copyright (c) 2021-2024 Zeping Lee
-- Released under the MIT license.
-- Repository: https://github.com/zepinglee/citeproc-lua
--

kpse.set_program_name("texlua", "citeproc-lua")

local cli = require("citeproc-cli")
os.exit(cli.main())
