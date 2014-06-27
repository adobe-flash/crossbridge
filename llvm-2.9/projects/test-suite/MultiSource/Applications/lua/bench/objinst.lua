-- $Id: objinst.lua,v 1.4 2004-07-04 22:26:59 bfulgham Exp $
-- http://shootout.alioth.debian.org/
-- contributed by Roberto Ierusalimschy

--------------------------------------------------------------
-- Toggle class
--------------------------------------------------------------

Toggle = {}

function Toggle:value ()
  return self.state
end

function Toggle:activate ()
  self.state = not self.state
  return self
end

function Toggle:new (start_state)
  local o = {state = start_state}
  self.__index = self
  setmetatable(o, self)
  return o
end

--------------------------------------------------------------
-- NthToggle class
--------------------------------------------------------------

NthToggle = Toggle:new()

function NthToggle:activate ()
  self.counter = self.counter + 1
  if self.counter >= self.count_max then
    self.state = not self.state
    self.counter = 0
  end
  return self
end

function NthToggle:new (start_state, max_counter)
  local o = Toggle.new(self, start_state)
  o.count_max = max_counter
  o.counter = 0
  return o
end

-----------------------------------------------------------
-- main
-----------------------------------------------------------
function main ()
    local N = tonumber((arg and arg[1])) or 1
    local toggle = Toggle:new(1)
    for i=1,5 do
      toggle:activate()
      print(toggle:value() and "true" or "false")
    end
    for i=1,N do
      toggle = Toggle:new(1)
    end

    print("")

    local ntoggle = NthToggle:new(1, 3)
    for i=1,8 do
      ntoggle:activate()
      print(ntoggle:value() and "true" or "false")
    end
    for i=1,N do
      ntoggle = NthToggle:new(1, 3)
    end
end

main()


