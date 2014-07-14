-- The Great Computer Language Shootout
-- http://shootout.alioth.debian.org/
--
-- contributed by Isaac Gouy

-- requires BigNum library
-- http://oss.digirati.com.br/luabignum/


require( "BigNum.lua" ) ;

-- Transformation

local T = {}

function T.bigzero() return BigNum.new(0) end 
function T.bigone() return BigNum.new(1) end 
function T.bigten() return BigNum.new(10) end 

function T.new(q,r,s,t) 
   return {
       q = BigNum.new(q)
      ,r = BigNum.new(r)
      ,s = BigNum.new(s)
      ,t = BigNum.new(t)
      ,k = 0
      } 
end

function T.unity() return T.new(1,0,0,1) end      
function T.zero() return T.new(0,0,0,0) end
  
function T.compose(a,b)    
   local qq = T.bigzero()
   local qr = T.bigzero()
   local rt = T.bigzero()
   local qrrt = T.bigzero()
   local sq = T.bigzero()
   local ts = T.bigzero()
   local sqts = T.bigzero()
   local sr = T.bigzero()
   local tt = T.bigzero()  
   local srtt = T.bigzero()                      
   
   BigNum.mul(a.q,b.q,qq)
   
   BigNum.mul(a.q,b.r,qr)   
   BigNum.mul(a.r,b.t,rt)
   BigNum.add(qr,rt,qrrt)
      
   BigNum.mul(a.s,b.q,sq)   
   BigNum.mul(a.t,b.s,ts)  
   BigNum.add(sq,ts,sqts)   
    
   BigNum.mul(a.s,b.r,sr)   
   BigNum.mul(a.t,b.t,tt)    
   BigNum.add(sr,tt,srtt)    

   return T.new(qq, qrrt, sqts, srtt)  
end


function T.extract(a,j) 
   local bigj = BigNum.new(j)
   local qj = T.bigzero()
   local qjr = T.bigzero()
   local sj = T.bigzero()
   local sjt = T.bigzero()
   local n = T.bigzero()
   local sjt = T.bigzero()                 
   
   BigNum.mul(a.q,bigj,qj)  
   BigNum.add(qj,a.r,qjr)
   
   BigNum.mul(a.s,bigj,sj)  
   BigNum.add(sj,a.t,sjt)       
   
   local d = T.bigzero()
   local r = T.bigzero()   
   BigNum.div(qjr,sjt,d,r)
  
   return math.floor( tonumber( BigNum.mt.tostring(d) ))
end

function T.next(a)
   a.k = a.k + 1
   a.q = BigNum.new(a.k)
   a.r = BigNum.new(4*a.k + 2)
   a.s = T.bigzero()
   a.t = BigNum.new(2*a.k + 1)
   return a
end


-- Pi Digit Stream

local P = {}

function P.new() 
   return {
       z = T.unity()
      ,x = T.zero()
      ,inverse = T.zero()
      } 
end

function P.produce(a,j) 
   local i = a.inverse
   i.q = T.bigten()
   i.r = BigNum.new(-10*j)
   i.s = T.bigzero()
   i.t = T.bigone()
   return T.compose(i,a.z)
end

function P.consume(a,b) 
   return T.compose(a.z,b)
end

function P.digit(a) 
   return T.extract(a.z,3)
end

function P.isSafe(a,j) 
   return j == T.extract(a.z,4)
end

function P.next(a)
   local y
   y = P.digit(a) 
   if P.isSafe(a,y) then  
      a.z = P.produce(a,y)
      return y
   else        
      a.z = P.consume(a,T.next(a.x))      
      return P.next(a)
   end 
end


-- Main

n = tonumber(arg and arg[1]) or 27
i = 0
length = 10
pidigit = P.new()

while n > 0 do
   if n < length then
      for _ = 1,n do io.write( P.next(pidigit) ) end
      for _ = n,length do io.write( ' ' ) end
      i = i + n
   else
      for _ = 1,length do io.write( P.next(pidigit) ) end
      i = i + length
   end
   io.write('\t:', i, '\n')
   n = n - length
end
