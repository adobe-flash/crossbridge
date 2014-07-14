-- $Id: lists.lua,v 1.1.1.1 2004-05-19 18:10:23 bfulgham Exp $
-- http://www.bagley.org/~doug/shootout/
-- implemented by: Roberto Ierusalimschy

--------------------------------------------------------------
-- List module
-- defines a prototipe for lists
--------------------------------------------------------------

List = {first = 0, last = -1}

function List:new ()
  local n = {}
  for k,v in pairs(self) do n[k] = v end
  return n
end

function List:length ()
  return self.last - self.first + 1
end

function List:pushleft (value)
  local first = self.first - 1
  self.first = first
  self[first] = value
end

function List:pushright (value)
  local last = self.last + 1
  self.last = last
  self[last] = value
end

function List:popleft ()
  local first = self.first
  if first > self.last then error"list is empty" end
  local value = self[first]
  self[first] = nil  -- to allow collection
  self.first = first+1
  return value
end

function List:popright ()
  local last = self.last
  if self.first > last then error"list is empty" end
  local value = self[last]
  self[last] = nil  -- to allow collection
  self.last = last-1
  return value
end

function List:reverse ()
  local i, j = self.first, self.last
  while i<j do
    self[i], self[j] = self[j], self[i]
    i = i+1
    j = j-1
  end
end

function List:equal (otherlist)
  if self:length() ~= otherlist:length() then return nil end
  local diff = otherlist.first - self.first
  for i1=self.first,self.last do
    if self[i1] ~= otherlist[i1+diff] then return nil end
  end
  return 1
end

-----------------------------------------------------------
-----------------------------------------------------------

-- Some tests

function test ()
  local SIZE = 10000
  -- create a list with elements 1..SIZE
  local l1 = List:new()
  for i=1,SIZE do
    l1:pushright(i)
  end
  -- creates a copy of l1
  local l2 = l1:new()
  -- remove each individual item from left side of l2 and
  -- append to right side of l3 (preserving order)
  local l3 = List:new()
  while l2:length() > 0 do
    l3:pushright(l2:popleft())  
  end
  -- remove each individual item from right side of l3 and
  -- append to right side of l2 (reversing list)
  while l3:length() > 0 do
    l2:pushright(l3:popright())
  end
  -- reverse l1 in place
  l1:reverse()
  -- compare Li1 and Li2 for equality
  -- and return length of the list
  if not l1:equal(l2) then return nil
  else return l1:length()
  end
end

N = tonumber((arg and arg[1])) or 1
for i=1, N do
  result = test()
end
print(result)
