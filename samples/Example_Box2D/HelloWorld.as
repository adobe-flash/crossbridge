// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

package
{
  import flash.display.Sprite;
  import flash.text.TextField;
  import flash.display.Stage;
  import flash.display.StageScaleMode;
  import flash.display.DisplayObjectContainer;
  import flash.display.Bitmap;
  import flash.display.BitmapData;
  import flash.utils.ByteArray
  import flash.net.LocalConnection;
  import flash.net.URLRequest;
  import flash.events.Event;
  import sample.Box2D.CModule;
  import b2Vec2;
  import b2World;

  public class HelloWorld extends Sprite
  {
    public static var current:HelloWorld;
    private var enableConsole:Boolean = true
    private var _tf:TextField;
    private var inputContainer:DisplayObjectContainer;

    public function HelloWorld(container:DisplayObjectContainer = null)
    {
      HelloWorld.current = this
      if(container) {
        container.addChild(this)
        initG(null);
      } else {
        addEventListener(Event.ADDED_TO_STAGE, initG);
      }
    }

    private function initG(e:Event):void
    {
      inputContainer = new Sprite()
      addChild(inputContainer)
  
      addEventListener(Event.ENTER_FRAME, framebufferBlit);
      stage.frameRate = 30;
      stage.quality = "best";
      
      if(enableConsole) {
      _tf = new TextField;
      _tf.multiline = true;
      _tf.width = stage.stageWidth;
      _tf.height = stage.stageHeight;
      inputContainer.addChild(_tf);
      }

      addEventListener(Event.ENTER_FRAME, framebufferBlit);

      CModule.startAsync(this);

      // Define the gravity vector.
      var gravity:b2Vec2 = b2Vec2.create()
      gravity.Set(0.0, -10.0);

      // Construct a world object, which will hold and simulate the rigid bodies.
      var world:b2World = b2World.create(gravity.swigCPtr)

      // Define the ground body.
      var groundBodyDef:b2BodyDef = b2BodyDef.create();
      var groundBodyDefPos:b2Vec2 = b2Vec2.create()
      groundBodyDefPos.Set(0.0, -10.0);
      groundBodyDef.position = groundBodyDefPos.swigCPtr;

      // Call the body factory which allocates memory for the ground body
      // from a pool and creates the ground box shape (also from a pool).
      // The body is also added to the world.
      var groundBody:b2Body = new b2Body();
      groundBody.swigCPtr = world.CreateBody(groundBodyDef.swigCPtr);

      // Define the ground box shape.
      var groundBox:b2PolygonShape = b2PolygonShape.create();

      // The extents are the half-widths of the box.
      groundBox.SetAsBox(50.0, 10.0);

      // Add the ground fixture to the ground body.
      groundBody.CreateFixture2(groundBox.swigCPtr, 0.0);

      // Define the dynamic body. We set its position and call the body factory.
      var bodyDef:b2BodyDef = b2BodyDef.create();
      bodyDef.type = Box2D.b2_dynamicBody;
      var bodyDefPos:b2Vec2 = b2Vec2.create()
      bodyDefPos.Set(0.0, 4.0);
      bodyDef.position = bodyDefPos.swigCPtr;
      var body:b2Body = new b2Body();
      body.swigCPtr = world.CreateBody(bodyDef.swigCPtr);

      // Define another box shape for our dynamic body.
      var dynamicBox:b2PolygonShape = b2PolygonShape.create();
      dynamicBox.SetAsBox(1.0, 1.0);

      // Define the dynamic body fixture.
      var fixtureDef:b2FixtureDef = b2FixtureDef.create();
      fixtureDef.shape = dynamicBox.swigCPtr;

      // Set the box density to be non-zero, so it will be dynamic.
      fixtureDef.density = 1.0;

      // Override the default friction.
      fixtureDef.friction = 0.3;

      // Add the shape to the body.
      body.CreateFixture(fixtureDef.swigCPtr);

      // Prepare for simulation. Typically we use a time step of 1/60 of a
      // second (60Hz) and 10 iterations. This provides a high quality simulation
      // in most game scenarios.
      var timeStep:Number = 1.0 / 60.0;
      var velocityIterations:int = 6;
      var positionIterations:int = 2;

      // This is our little game loop.
      for (var i:int = 0; i < 60; ++i)
      {
        // Instruct the world to perform a single step of simulation.
        // It is generally best to keep the time step and iterations fixed.
        world.Step(timeStep, velocityIterations, positionIterations);

        // Now print the position and angle of the body.
        var pos:b2Vec2 = new b2Vec2();
        pos.swigCPtr = body.GetPosition();
        var angle:Number = body.GetAngle();

        consoleWrite("body: " + pos.x + ", " + pos.y + ", " + angle + "\n");
      }
    }

    public function write(fd:int, buf:int, nbyte:int, errno_ptr:int):int
    {
      var str:String = CModule.readString(buf, nbyte);
      i_write(str);
      return nbyte;
    }

    public function read(fd:int, buf:int, nbyte:int, errno_ptr:int):int
    {
      return 0
    }

    public function consoleWrite(s:String):void
    {
      if(enableConsole) {
        _tf.appendText(s);
        _tf.scrollV = _tf.maxScrollV
      }
    }

    public function i_exit(code:int):void
    {
      consoleWrite("\nexit code: " + code + "\n");
    }

    public function i_error(e:String):void
    {
       consoleWrite("\nexception: " + e + "\n");
    }

    public function i_write(str:String):void
    {
      consoleWrite(str);
    }

    public function framebufferBlit(e:Event):void
    {
	  //var args:Vector.<int> = new Vector.<int>;
	  //CModule.callI(CModule.getPublicSymbol("updateUniverse"), args);
    }
  }
}
