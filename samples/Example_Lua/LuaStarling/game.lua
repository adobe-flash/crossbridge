-- This is a simple flash app (based on BunnyMark) written in Lua
-- It's running inside the *actual* Lua interpreter (http://www.lua.org/)
-- The Lua interpreter was compiled to a SWF with flascc 2
-- 
-- Because it's an interpreter you can of course change the code
-- below without having to recompile the SWF!

bunnies = {}
ctx3d = nil
rootsprite = nil
background = nil

numBunnies = 0
incBunnies = 10
maxBunnies = 500

_width = 0
_height = 0

gravity = 0.5
maxX = 0
minX = 0
maxY = 0
minY = 0

cols = 8
rows = 12

imgsloaded = 0
loading = true

function addBunnies(numBunnies)
  for i = 0, numBunnies do
    bunny = { img=flash.new("starling.display.Image", bunnytex), speedX=0.0, speedY=0.0, speedR=0.0 }
    bunny.img.pivotX = flash.getnumber(bunny.img, "width") / 2
    bunny.img.pivotY = flash.getnumber(bunny.img, "height") / 2
    bunny.speedX = math.random() * 5
    bunny.speedY = (math.random() * 5) - 2.5
    bunny.img.rotation = 15 - math.random() * 30
    
    table.insert(bunnies, bunny)
    rootsprite:addChild(bunny.img)
  end
end

-- The host will call this function once when the starling context has been setup
-- and 'rootsprite' has been added to the starling stage
function setupGame(_rootsprite, _ctx3d, w, h)
  rootsprite = _rootsprite
  ctx3d = _ctx3d

  _width = 640
  _height = 960
  maxX = _width
  maxY = _height

  downloadImage("assets/grass.png", function(event)
    local ldrinfo = event.target
    grassimg = ldrinfo.content
    imgsloaded = imgsloaded + 1
  end
  )

  downloadImage("assets/wabbit_alpha.png", function(event)
    local ldrinfo = event.target
    bunnyimg = ldrinfo.content
    imgsloaded = imgsloaded + 1
  end
  )
end

function setupWorld()
  grasstex = ctx3d:createTexture(grassimg.width, grassimg.height, "bgra", false)
  flash.call(grasstex, "uploadFromBitmapData", flash.getprop(grassimg, "bitmapData"), 0)
  bunnytex = flash.callstatic("starling.textures.Texture", "fromBitmap", bunnyimg)

  buildMesh()

  background = flash.new("LuaDisplayObject", rootsprite, "renderBackground")
  rootsprite:addChild(background)

  --build shaders
  local miniasm_vertex = flash.new("com.adobe.utils.AGALMiniAssembler")
  miniasm_vertex:assemble("vertex",
    "m44 op, va0, vc0  \n" ..        -- 4x4 matrix transform to output clipspace
    "mov v0, va1       \n")          -- pass texture coordinates to fragment program

  local miniasm_fragment = flash.new("com.adobe.utils.AGALMiniAssembler")
  miniasm_fragment:assemble("fragment",  
    "tex oc, v0, fs0 <2d, linear,wrap>" ) -- sample texture 0
  shader_program = ctx3d:createProgram()

  shader_program:upload(miniasm_vertex.agalcode, miniasm_fragment.agalcode)
  
  --create projection matrix
  _modelViewMatrix = flash.new("flash.geom.Matrix3D")
  _modelViewMatrix:appendTranslation(-(_width)/2, -(_height)/2, 0)
  _modelViewMatrix:appendScale(2.0/(_width-50), -2.0/(_height-50), 1)
  
  --set everything
  ctx3d:setTextureAt(0, grasstex)
  ctx3d:setProgram(shader_program )
  ctx3d:setVertexBufferAt(0, vb, 0, "float2" )  
  ctx3d:setVertexBufferAt(1, uvb, 0, "float2" )
  ctx3d:setProgramConstantsFromMatrix("vertex", 0, _modelViewMatrix, true)
end

function renderBackground()
  local t = flash.gettimer() / 1000.0
  local sw = _width
  local sh = _height
  local kx, ky
  local ci, ri;
  ctx3d:setBlendFactors("one", "oneMinusSourceAlpha")
  ctx3d:setTextureAt(0, grasstex)
  ctx3d:setProgram(shader_program )
  ctx3d:setVertexBufferAt(0, vb, 0, "float2" )
  ctx3d:setVertexBufferAt(1, uvb, 0, "float2" )
  ctx3d:setProgramConstantsFromMatrix("vertex", 0, _modelViewMatrix, true)

  local i = 0
  for j = 0, rows do
    ri = j * (cols + 1) * 2
    for i = 0,cols do
      ci = ri + i * 2
      kx = i / cols + math.cos(t + i) * 0.02
      ky = j / rows + math.sin(t + j + i) * 0.02
      flash.setidxnumber(vertices, ci, sw * kx)
      flash.setidxnumber(vertices, ci + 1, sh * ky)
    end
  end

  vb:uploadFromVector(vertices, 0, numVertices)
  ctx3d:drawTriangles(ib, 0, numTriangles)
end

function starlingUpdate()
  if loading and imgsloaded < 2 then
    return
  end

  if loading and imgsloaded == 2 then
    setupWorld()
    loading = false
  end

  if numBunnies < maxBunnies then
    addBunnies(incBunnies)
    numBunnies = numBunnies+incBunnies
  end

  for i,bunny in ipairs(bunnies) do
    local nx = flash.getx(bunny.img) + bunny.speedX
    local ny = flash.gety(bunny.img) + bunny.speedY
    flash.setx(bunny.img, nx)
    flash.sety(bunny.img, ny)
    bunny.speedY = bunny.speedY + gravity

    if nx > maxX then
      bunny.speedX = bunny.speedX * -1
      flash.setx(bunny.img, maxX)
    elseif nx < minX then
      bunny.speedX = bunny.speedX * -1
      flash.setx(bunny.img, minX)
    end
 
    if ny > maxY then
      bunny.speedY = bunny.speedY * -0.95
      flash.sety(bunny.img, maxY)
      if math.random() > 0.5 then
        bunny.speedY = bunny.speedY - 3 + math.random() * 8
      end
    elseif ny < minY then
      bunny.speedY = 0
      flash.sety(bunny.img, minY)
    end
  end
end

function buildMesh()
  local uw = _width / flash.getnumber(grassimg, "width")
  local uh = _height / flash.getnumber(grassimg, "height")
  local kx, ky, ci, ci2, ri

  -- create vertices, use magic number for now for 8 cols 12 rows
  -- mVertexData = new VertexData(117, false)
  -- mRawData = mVertexData.rawData

  vertices = flash.newnumbervec(0)
  uvt = flash.newnumbervec(0)
  indices = flash.newuintvec(0)

  local i, j

  for j = 0, rows do
    ri = j * (cols + 1) * 2
    ky = j / rows
    for i = 0,cols do
      ci = ri + i * 2
      kx = i / cols
      flash.setidxnumber(vertices, ci, _width * kx)
      flash.setidxnumber(vertices, ci + 1, _height * ky)
      flash.setidxuint(uvt, ci, uw * kx)
      flash.setidxuint(uvt, ci + 1, uh * ky)
    end
  end

  for j=0,rows-1 do
    ri = j * (cols + 1)
    for i=0,cols-1 do
      ci = i + ri
      ci2 = ci + cols + 1
      indices:push(ci)
      indices:push(ci + 1)
      indices:push(ci2)
      indices:push(ci + 1)
      indices:push(ci2 + 1)
      indices:push(ci2)
    end
  end

  --now create the buffers
  numIndices = flash.getuint(indices, "length")
  numTriangles = numIndices / 3
  numVertices = flash.getuint(vertices, "length") / 2
  vb = ctx3d:createVertexBuffer(numVertices, 2)
  uvb = ctx3d:createVertexBuffer(numVertices, 2)
  ib = ctx3d:createIndexBuffer(numIndices)
  vb:uploadFromVector(vertices, 0, numVertices)
  ib:uploadFromVector(indices, 0, numIndices)
  uvb:uploadFromVector(uvt, 0, numVertices)
end

function downloadImage(url, completehandler)
  local loader = flash.new("flash.display.Loader")
  loader:load(flash.new("flash.net.URLRequest", url))
  flash.call(loader.contentLoaderInfo, "addEventListener", "complete", completehandler)
end
