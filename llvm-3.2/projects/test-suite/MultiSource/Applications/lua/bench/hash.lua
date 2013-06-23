-- $Id: hash.lua,v 1.2 2004-05-22 04:31:50 bfulgham Exp $
-- http://www.bagley.org/~doug/shootout/
-- Author: Roberto Ierusalimschy

local n = tonumber((arg and arg[1]) or 1)

local X={}
for i=1,n do
  X[string.format("%x", i)] = i
end

local c = 0

for i=n,1,-1 do
  if X[i..''] then c = c+1 end
end

print(c)
