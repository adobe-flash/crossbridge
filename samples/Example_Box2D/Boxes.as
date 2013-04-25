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
  import flash.display.Bitmap;
  import flash.display.BitmapData;
  import flash.display.DisplayObjectContainer;
  import flash.display.Sprite;
  import flash.display.Stage;
  import flash.display.StageScaleMode;
  import flash.events.Event;
  import flash.net.LocalConnection;
  import flash.net.URLRequest;
  import flash.text.TextField;
  import flash.utils.ByteArray;

  import sample.Box2D.CModule;
  import sample.Box2D.vfs.ISpecialFile;
  import Box2DSprite;

  /**
  * A basic implementation of a console for flascc apps.
  * The PlayerKernel class delegates to this for things like read/write
  * so that console output can be displayed in a TextField on the Stage.
  */
  public class Boxes extends Sprite implements ISpecialFile
  {
    private var enableConsole:Boolean = false
    private var _tf:TextField
    private var inputContainer:DisplayObjectContainer

    private var gravity:b2Vec2
    private var world:b2World
    private var groundBodyDef:b2BodyDef
    private var groundBodyDefPos:b2Vec2
    private var groundBody:b2Body
    private var groundBox:b2PolygonShape
    private var boxes:Vector.<Box2DSprite> = new Vector.<Box2DSprite>()

    include "../TestingCode.as"

    public function Boxes(container:DisplayObjectContainer = null)
    {
      CModule.rootSprite = container ? container.root : this
      if(container) {
        container.addChild(this)
        init(null)
      } else {
        addEventListener(Event.ADDED_TO_STAGE, init)
      }
    }

    /**
    * All of the real flascc init happens in this method
    * which is either run on startup or once the SWF has
    * been added to the stage.
    */
    private function init(e:Event):void
    {
      inputContainer = new Sprite()
      addChild(inputContainer)

      addEventListener(Event.ENTER_FRAME, enterFrame)

      stage.frameRate = 60
      stage.scaleMode = StageScaleMode.NO_SCALE

      if(enableConsole) {
        _tf = new TextField
        _tf.multiline = true
        _tf.width = stage.stageWidth
        _tf.height = stage.stageHeight 
        inputContainer.addChild(_tf)
      }

      CModule.startAsync(this)

      // Define the gravity vector.
      gravity = b2Vec2.create()
      gravity.Set(0.0, -10.0);

      // Construct a world object, which will hold and simulate the rigid bodies.
      world = b2World.create(gravity.swigCPtr)

      // Define the ground body.
      groundBodyDef = b2BodyDef.create();
      groundBodyDefPos = b2Vec2.create()
      groundBodyDefPos.Set(0.0, -5.0);
      groundBodyDef.position = groundBodyDefPos.swigCPtr;

      // Call the body factory which allocates memory for the ground body
      // from a pool and creates the ground box shape (also from a pool).
      // The body is also added to the world.
      groundBody = new b2Body();
      groundBody.swigCPtr = world.CreateBody(groundBodyDef.swigCPtr);

      // Define the ground box shape.
      groundBox = b2PolygonShape.create();

      // The extents are the half-widths of the box.
      groundBox.SetAsBox(2000.0, 5.0);

      // Add the ground fixture to the ground body.
      groundBody.CreateFixture2(groundBox.swigCPtr, 0.0);

      for(var i:int=0; i<500; i++) {
        var bs:Box2DSprite = new Box2DSprite(100 + random() * 200, 10 + random() * 3000, 10 + random()*50, 2 + random()*5, world);
        boxes.push(bs);
        addChild(bs);
      }

      initTesting();
    }

    /**
    * The PlayerKernel implementation will use this function to handle
    * C IO write requests to the file "/dev/tty" (e.g. output from
    * printf will pass through this function).
    */
    public function write(fd:int, buf:int, nbyte:int, errnoPtr:int):int
    {
      var str:String = CModule.readString(buf, nbyte)
      consoleWrite(str)
      return nbyte
    }

    /**
    * The PlayerKernel implementation will use this function to handle
    * C IO read requests to the file "/dev/tty" (e.g. reads from stdin
    * will expect this function to provide the data).
    */
    public function read(fd:int, buf:int, nbyte:int, errnoPtr:int):int
    {
      return 0
    }

    /**
    * The PlayerKernel implementation will use this function to handle
    * C fcntl requests to the file "/dev/tty"
    * See the ISpecialFile documentation for more information about the
    * arguments and return value.
    */
    public function fcntl(fd:int, com:int, data:int, errnoPtr:int):int
    {
      return 0
    }

    /**
    * The PlayerKernel implementation will use this function to handle
    * C ioctl requests to the file "/dev/tty"
    * See the ISpecialFile documentation for more information about the
    * arguments and return value.
    */
    public function ioctl(fd:int, com:int, data:int, errnoPtr:int):int
    {
      return 0
    }

    /**
    * Helper function that traces to the flashlog text file and also
    * displays output in the on-screen textfield console.
    */
    private function consoleWrite(s:String):void
    {
      trace(s)
      if(enableConsole) {
        _tf.appendText(s)
        _tf.scrollV = _tf.maxScrollV
      }
    }

    /**
    * Calling serviceUIRequests from the main worker allows us to service any requests
    * from background workers that want to use flash APIs that need main
    * worker privileges. 
    */
    public function enterFrame(e:Event):void
    {
      CModule.serviceUIRequests()
      var timeStep:Number = 1.0 / 60.0;
      var velocityIterations:int = 12;
      var positionIterations:int = 4;
      world.Step(timeStep, velocityIterations, positionIterations);

      for(var i:int=0; i<boxes.length; i++) {
        boxes[i].update()
      }
    }

    // ======================================================
    // The following code is from Grant Skinner's Rndm.as
    // ======================================================

    /**
    * Rndm by Grant Skinner. Jan 15, 2008
    * Visit www.gskinner.com/blog for documentation, updates and more free code.
    *
    * Incorporates implementation of the Park Miller (1988) "minimal standard" linear 
    * congruential pseudo-random number generator by Michael Baczynski, www.polygonal.de.
    * (seed * 16807) % 2147483647
    *
    *
    *
    * Copyright (c) 2008 Grant Skinner
    * 
    * Permission is hereby granted, free of charge, to any person
    * obtaining a copy of this software and associated documentation
    * files (the "Software"), to deal in the Software without
    * restriction, including without limitation the rights to use,
    * copy, modify, merge, publish, distribute, sublicense, and/or sell
    * copies of the Software, and to permit persons to whom the
    * Software is furnished to do so, subject to the following
    * conditions:
    * 
    * The above copyright notice and this permission notice shall be
    * included in all copies or substantial portions of the Software.
    * 
    * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    * OTHER DEALINGS IN THE SOFTWARE.
    */

    protected var _currentSeed:uint=1234;

    /**
    * returns a number between 0-1 exclusive.
    */
    public function random():Number {
      return (_currentSeed = (_currentSeed * 16807) % 2147483647)/0x7FFFFFFF+0.000000000233;
    }
  }
}
