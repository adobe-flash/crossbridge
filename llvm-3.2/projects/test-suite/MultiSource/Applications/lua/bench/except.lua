-- $Id: except.lua,v 1.2 2004-05-23 04:36:29 bfulgham Exp $
-- http://www.bagley.org/~doug/shootout/
-- from Roberto Ierusalimschy

-- uses `call' to catch errors; return the error message
-- (or nil if there are no errors)

function try (f, arg)
  local status, err = pcall(f, arg)
  if not status then return err end
end

local HI = 0
local LO = 0

function some_function (n)
  local res = try(hi_function, n)
  if res then print("We shouldn't get here: " .. res) end
end


function hi_function (n)
  local res = try(lo_function, n)
  if res == "Hi_Exception" then HI = HI+1 
  elseif res then error(res, 0)  -- rethrow
  end
end


function lo_function (n)
  local res = try(blowup, n)
  if res == "Lo_Exception" then LO = LO+1 
  elseif res then error(res, 0)  -- rethrow
  end
end


function blowup (n)
  if math.fmod(n,2) ~= 0 then error("Lo_Exception", 0)
  else error("Hi_Exception",0)
  end
end


N = (arg and arg[1]) or 1
for i=1,N do
  some_function(i)
end

print(string.format("Exceptions: HI=%d / LO=%d", HI, LO))
