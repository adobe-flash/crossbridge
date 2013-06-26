local socket = require "socket"
local posix = require "posix"

local M, reply_size, request_size = 100, 4096, 64
local N = tonumber(arg and arg[1]) or 1

local function client(sk)
  local repsize = reply_size
  local request = string.rep(" ", request_size)
  local replies = 0
  for i=1,N*M do
    sk:send(request)
    sk:receive(repsize)
    replies = replies + 1
  end
  io.write("replies: ", replies, "\tbytes: ", sk:getstats(), "\n")
  io.flush()
  sk:close()
end

local function server(sk)
  local reqsize = request_size
  local reply = string.rep(" ", reply_size)
  for i=1,N*M do
    sk:receive(reqsize)
    sk:send(reply)
  end
  sk:close()
end

local ls = socket.bind("127.0.0.1", 0)
if posix.fork() == 0 then		-- Child is client
  client(socket.connect(ls:getsockname()))
  os.exit()
else					-- Parent is server
  server(ls:accept())
  ls:close()
  posix.wait()
end
