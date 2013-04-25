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
  import flash.display.Stage;
  import flash.geom.Matrix;

  public class Box2DSprite extends Sprite
  {
    private var bodyDef:b2BodyDef
    private var bodyDefPos:b2Vec2
    private var body:b2Body
    private var dynamicBox:b2PolygonShape
    private var fixtureDef:b2FixtureDef
    private var w:Number, h:Number;

    public function Box2DSprite(_x:Number, _y:Number, _w:Number, _h:Number, world:b2World)
    {
      w = _w;
      h = _h;

      graphics.lineStyle(0.25,0x000000);
      graphics.beginFill(0, 0.2);
      graphics.drawRect(0, 0, _w, _h);
      graphics.endFill();

      // Define the dynamic body. We set its position and call the body factory.
      bodyDef = b2BodyDef.create();
      bodyDef.type = Box2D.b2_dynamicBody;
      bodyDefPos = b2Vec2.create()
      bodyDefPos.Set(_x/10, _y/10);
      bodyDef.position = bodyDefPos.swigCPtr;
      body = new b2Body();
      body.swigCPtr = world.CreateBody(bodyDef.swigCPtr);

      // Define another box shape for our dynamic body.
      dynamicBox = b2PolygonShape.create();
      dynamicBox.SetAsBox(_w/20, _h/20);

      // Define the dynamic body fixture.
      fixtureDef = b2FixtureDef.create();
      fixtureDef.shape = dynamicBox.swigCPtr;

      // Set the box density to be non-zero, so it will be dynamic.
      fixtureDef.density = 1.0;

      // Override the default friction.
      fixtureDef.friction = 0.3;

      // Add the shape to the body.
      body.CreateFixture(fixtureDef.swigCPtr);
    }

    public function update():void
    {
      bodyDefPos.swigCPtr = body.GetPosition();
      var matrix:Matrix = new Matrix() 
      matrix.translate(- w/2, - (h/2));
      matrix.rotate(-body.GetAngle());
      matrix.translate((bodyDefPos.x*10) + w/2, (stage.stageHeight + (h/2)) - (bodyDefPos.y*10));
      transform.matrix = matrix;
    }
  }
}