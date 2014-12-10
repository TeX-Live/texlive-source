-- $Id: pdflua.lua 3949 2010-11-07 00:09:19Z hhenkel $
-- $URL: https://foundry.supelec.fr/svn/luatex/trunk/source/texk/web2c/luatexdir/luascripts/pdflua.lua $

-- this is early work in progress...

------------------------------------------------------------------------

beginpage = function(a)
end

endpage = function(a)
end

outputpagestree = function()
end

------------------------------------------------------------------------

local pdflua = {
  beginpage = beginpage,
  endpage = endpage,
  outputpagestree = outputpagestree,
}

return pdflua
