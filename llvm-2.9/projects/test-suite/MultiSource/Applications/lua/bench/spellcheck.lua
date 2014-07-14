-- $Id: spellcheck.lua,v 1.2 2004-05-25 02:26:33 bfulgham Exp $
-- http://www.bagley.org/~doug/shootout/
-- based on code from Roberto Ierusalimschy

local dict_file = "input/spellcheck-dict.txt"

assert(io.open(dict_file))
local dict = {}
for line in io.lines(dict_file) do
  dict[line] = true
end

for word in io.lines() do
  if not dict[word] then print(word) end
end

