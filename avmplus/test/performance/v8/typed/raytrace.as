// The ray tracer code in this file is written by Adam Burmister. It
// is available in its original form from:
//
//   http://labs.flog.nz.co/raytracer/
//
// It has been modified slightly by Google to work as a standalone
// benchmark, but the all the computational code remains
// untouched. This file also contains a copy of the Prototype
// JavaScript framework which is used by the ray tracer.
package FlogRayTracer {

/*
var RayTrace = new BenchmarkSuite('RayTrace', 932666, [
  new Benchmark('RayTrace', renderScene)
]);
*/

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------

// The rest of this file is the actual ray tracer written by Adam
// Burmister. It's a concatenation of the following files:
//
//   flog/color.js
//   flog/light.js
//   flog/vector.js
//   flog/ray.js
//   flog/scene.js
//   flog/material/basematerial.js
//   flog/material/solid.js
//   flog/material/chessboard.js
//   flog/shape/baseshape.js
//   flog/shape/sphere.js
//   flog/shape/plane.js
//   flog/intersectioninfo.js
//   flog/camera.js
//   flog/background.js
//   flog/engine.js


/* Fake a Flog.* namespace */
// if(typeof(Flog) == 'undefined') var Flog = {};
// if(typeof(Flog.RayTracer) == 'undefined') Flog.RayTracer = {};

//Color = Class.create();




  //Color = {
  public class Color {
    public var red:Number = 0.0;
    public var green:Number = 0.0
    public var blue:Number = 0.0;
    public function Color(r:Number, g:Number, b:Number):void {
      if(!r) r = 0.0;
      if(!g) g = 0.0;
      if(!b) b = 0.0;
  
      this.red = r;
      this.green = g;
      this.blue = b;
    }

    public static function add (c1:Color, c2:Color):Color {
        var result:Color = new Color(0,0,0);

        result.red = c1.red + c2.red;
        result.green = c1.green + c2.green;
        result.blue = c1.blue + c2.blue;

        return result;
    }

    public static function addScalar(c1:Color, s:Number):Color{
        var result:Color = new Color(0,0,0);

        result.red = c1.red + s;
        result.green = c1.green + s;
        result.blue = c1.blue + s;

        result.limit();

        return result;
    }

    public static function subtract(c1:Color, c2:Color){
        var result:Color = new Color(0,0,0);

        result.red = c1.red - c2.red;
        result.green = c1.green - c2.green;
        result.blue = c1.blue - c2.blue;

        return result;
    }

    public static function multiply(c1:Color, c2:Color):Color {
        var result:Color = new Color(0,0,0);

        result.red = c1.red * c2.red;
        result.green = c1.green * c2.green;
        result.blue = c1.blue * c2.blue;

        return result;
    }

    public  static function multiplyScalar(c1:Color, f:Number):Color {
        var result = new Color(0,0,0);

        result.red = c1.red * f;
        result.green = c1.green * f;
        result.blue = c1.blue * f;

        return result;
    }

    public  static function divideFactor(c1:Color, f:Number):Color {
        var result:Color = new Color(0,0,0);

        result.red = c1.red / f;
        result.green = c1.green / f;
        result.blue = c1.blue / f;

        return result;
    }

    public function limit():void {
        this.red = (this.red > 0.0) ? ( (this.red > 1.0) ? 1.0 : this.red ) : 0.0;
        this.green = (this.green > 0.0) ? ( (this.green > 1.0) ? 1.0 : this.green ) : 0.0;
        this.blue = (this.blue > 0.0) ? ( (this.blue > 1.0) ? 1.0 : this.blue ) : 0.0;
    }

    public function distance(color:Color):Number {
        var d:Number = Math.abs(this.red - color.red) + Math.abs(this.green - color.green) + Math.abs(this.blue - color.blue);
        return d;
    }

    public  static function blend(c1:Color, c2:Color, w:Number):Color{
        var result:Color = new Color(0,0,0);
        result = Color.add(
                    Color.multiplyScalar(c1, 1 - w),
                    Color.multiplyScalar(c2, w)
                  );
        return result;
    }

    public function toString ():String {
        var r:Number = Math.floor(this.red*255);
        var g:Number = Math.floor(this.green*255);
        var b:Number = Math.floor(this.blue*255);

        return "rgb("+ r +","+ g +","+ b +")";
    }
  }

/* Fake a Flog.* namespace */
//if(typeof(Flog) == 'undefined') var Flog = {};
//if(typeof(Flog.RayTracer) == 'undefined') Flog.RayTracer = {};

public class Light {
  public var position
  public var color:Color
  public var intensity:Number = 10.0;

  public function Light(pos, color:Color, intensity:Number=undefined) {
        this.position = pos;
        this.color = color;
        this.intensity = (intensity ? intensity : 10.0);
    }

    public function getIntensity(distance:Number):Number{
        if(distance >= intensity) return 0;

        return Math.pow((intensity - distance) / strength, 0.2);
    }

    public function toString():String {
        return 'Light [' + this.position.x + ',' + this.position.y + ',' + this.position.z + ']';
    }
}
/* Fake a Flog.* namespace */
//if(typeof(Flog) == 'undefined') var Flog = {};
//if(typeof(Flog.RayTracer) == 'undefined') Flog.RayTracer = {};

//RVector = Class.create();

public class RVector {
    public var x:Number = 0.0;
    public var y:Number = 0.0;
    public var z:Number = 0.0;

    public function RVector(x:Number, y:Number, z:Number):void {
        this.x = (x ? x : 0);
        this.y = (y ? y : 0);
        this.z = (z ? z : 0);
    }

    public function copy(vector:RVector):void{
        this.x = vector.x;
        this.y = vector.y;
        this.z = vector.z;
    }

    public function normalize():RVector {
        var m:Number = this.magnitude();
        return new RVector(this.x / m, this.y / m, this.z / m);
    }

    public function magnitude():Number {
        return Math.sqrt((this.x * this.x) + (this.y * this.y) + (this.z * this.z));
    }

    public function cross(w:RVector):RVector {
        return new RVector(
          -this.z * w.y + this.y * w.z,
          this.z * w.x - this.x * w.z,
          -this.y * w.x + this.x * w.y);
    }

    public function dot(w:RVector):Number {
        return this.x * w.x + this.y * w.y + this.z * w.z;
    }

    public static function add(v:RVector, w:RVector):RVector {
        return new RVector(w.x + v.x, w.y + v.y, w.z + v.z);
    }

    public static function subtract(v:RVector, w:RVector):RVector {
        if(!w || !v) throw 'RVectors must be defined [' + v + ',' + w + ']';
        return new RVector(v.x - w.x, v.y - w.y, v.z - w.z);
    }

    public static function multiplyRVector(v:RVector, w:RVector):RVector {
        return new RVector(v.x * w.x, v.y * w.y, v.z * w.z);
    }

    public static function multiplyScalar(v:RVector, w:Number):RVector {
        return new RVector(v.x * w, v.y * w, v.z * w);
    }

    public function toString():String {
        return 'RVector [' + this.x + ',' + this.y + ',' + this.z + ']';
    }
}
/* Fake a Flog.* namespace */
//if(typeof(Flog) == 'undefined') var Flog = {};
//if(typeof(Flog.RayTracer) == 'undefined') Flog.RayTracer = {};

//Ray = Class.create();

public class Ray {
    var position:RVector
    var direction:RVector
    
    public function Ray(pos:RVector, dir:RVector):void {
        this.position = pos;
        this.direction = dir;
    }

    public function toString ():String {
        return 'Ray [' + this.position + ',' + this.direction + ']';
    }
}
/* Fake a Flog.* namespace */
//if(typeof(Flog) == 'undefined') var Flog = {};
//if(typeof(Flog.RayTracer) == 'undefined') Flog.RayTracer = {};

//Scene = Class.create();

public class Scene {
    public var camera:Camera
    public var shapes:Vector.<Shape>;
    public var lights:Vector.<Light>;
    public var background:Background;

    public function Scene ():void {
        this.camera = new Camera(
            new RVector(0,0,-5),
            new RVector(0,0,1),
            new RVector(0,1,0)
        );
        this.shapes = new Vector.<Shape>();
        this.lights = new Vector.<Light>();
        this.background = new Background(new Color(0,0,0.5), 0.2);
    }
}

//Material.BaseMaterial = {
public class Material {

    public var gloss:Number = 2.0;             // [0...infinity] 0 = matt
    public var transparency:Number = 0.0;      // 0=opaque
    public var reflection:Number = 0.0;       // [0...infinity] 0 = no reflection
    public var refraction:Number = 0.50;
    public var hasTexture:Boolean = false;

    public function Material ():void {
    }

    public function wrapUp (t:Number):Number{
        t = t % 2.0;
        if(t < -1) t += 2.0;
        if(t >= 1) t -= 2.0;
        return t;
    }

    public function toString ():String {
        return 'Material [gloss=' + this.gloss + ', transparency=' + this.transparency + ', hasTexture=' + this.hasTexture +']';
    }
}

public class Solid extends Material {
    public var color:Color;
    public function Solid (color:Color, reflection:Number, refraction:Number, transparency:Number, gloss:Number=undefined):void {
        super();
        this.color = color;
        this.reflection = reflection;
        this.transparency = transparency;
        if (gloss) this.gloss = gloss;
        this.hasTexture = false;
    }

    public function getColor(u:Number=undefined, v:Number=undefined):Color{
        return this.color;
    }

    override public function toString ():String {
        return 'SolidMaterial [gloss=' + this.gloss + ', transparency=' + this.transparency + ', hasTexture=' + this.hasTexture +']';
    }
}


public class Chessboard extends Material {

    public var colorEven:Color;
    public var colorOdd:Color;
    public var density:Number = 0.5;
    
    public function Chessboard (colorEven:Color, colorOdd:Color, reflection:Number, transparency:Number, gloss:Number, density:Number=undefined):void {
        this.colorEven = colorEven;
        this.colorOdd = colorOdd;
        this.reflection = reflection;
        this.transparency = transparency;
        this.gloss = gloss;
        if (density) this.density = density;
        this.hasTexture = true;
    }
    
    public function getColor (u:Number=undefined, v:Number=undefined):Color{
        var t:Number = this.wrapUp(u * this.density) * this.wrapUp(v * this.density);
    
        if(t < 0.0)
            return this.colorEven;
        else
            return this.colorOdd;
    }
    
    override public function toString ():String {
        return 'ChessMaterial [gloss=' + this.gloss + ', transparency=' + this.transparency + ', hasTexture=' + this.hasTexture +']';
    }
}

public class Shape {
    public var position:RVector;
    public var material:Material;

    public function Shape () {
        this.position = new RVector(0,0,0);
        this.material = new Solid(new Color(1,0,1),0,0,0);
    }

    public function toString ():String {
        return 'Material [gloss=' + this.gloss + ', transparency=' + this.transparency + ', hasTexture=' + this.hasTexture +']';
    }
}

public class Sphere extends Shape {
    public var radius:Number;
    
    public function Sphere (pos:RVector, radius:Number, material:Material) {
        this.radius = radius;
        this.position = pos;
        this.material = material;
    }

    public function intersect (ray:Ray):IntersectionInfo {
        var info:IntersectionInfo = new IntersectionInfo();
        info.shape = this;

        var dst:RVector = RVector.subtract(ray.position, this.position);

        var B:Number = dst.dot(ray.direction);
        var C:Number = dst.dot(dst) - (this.radius * this.radius);
        var D:Number= (B * B) - C;

        if(D > 0){ // intersection!
            info.isHit = true;
            info.distance = (-B) - Math.sqrt(D);
            info.position = RVector.add(
                                                ray.position,
                                                RVector.multiplyScalar(
                                                    ray.direction,
                                                    info.distance
                                                )
                                            );
            info.normal = RVector.subtract(
                                            info.position,
                                            this.position
                                        ).normalize();

            info.color = this.material.getColor(0,0);
        } else {
            info.isHit = false;
        }
        return info;
    }

    override public function toString ():String {
        return 'Sphere [position=' + this.position + ', radius=' + this.radius + ']';
    }
}

public class Plane extends Shape {
    public var d:Number = 0.0;

    public function Plane (pos:RVector, d:Number, material:Material) {
        super();
        this.position = pos;
        this.d = d;
        this.material = material;
    }

    public function intersect (ray:Ray):IntersectionInfo{
        var info:IntersectionInfo = new IntersectionInfo();

        var Vd:Number = this.position.dot(ray.direction);
        if(Vd == 0) return info; // no intersection

        var t:Number = -(this.position.dot(ray.position) + this.d) / Vd;
        if(t <= 0) return info;

        info.shape = this;
        info.isHit = true;
        info.position = RVector.add(
                                            ray.position,
                                            RVector.multiplyScalar(
                                                ray.direction,
                                                t
                                            )
                                        );
        info.normal = this.position;
        info.distance = t;

        if(this.material.hasTexture){
            var vU:RVector = new RVector(this.position.y, this.position.z, -this.position.x);
            var vV:RVector = vU.cross(this.position);
            var u:Number = info.position.dot(vU);
            var v:Number = info.position.dot(vV);
            info.color = this.material.getColor(u,v);
        } else {
            info.color = this.material.getColor(0,0);
        }

        return info;
    }

    override public function toString ():String {
        return 'Plane [' + this.position + ', d=' + this.d + ']';
    }
}

public class IntersectionInfo {
    public var isHit:Boolean = false;
    public var hitCount:int = 0;
    public var shape:Shape;
    public var position:RVector;
    public var normal:RVector;
    public var color:Color;
    public var distance:Number;

    public function IntersectionInfo ():void {
        this.color = new Color(0,0,0);
    }

    public function toString ():String {
        return 'Intersection [' + this.position + ']';
    }
}

public class Camera {
    public var position:RVector;
    public var lookAt:RVector;
    public var equator:RVector;
    public var up:RVector;
    public var screen:RVector;

    public function Camera (pos:RVector, lookAt:RVector, up:RVector):void {
        this.position = pos;
        this.lookAt = lookAt;
        this.up = up;
        this.equator = lookAt.normalize().cross(this.up);
        this.screen = RVector.add(this.position, this.lookAt);
    }

    public function getRay (vx:Number, vy:Number):Ray {
        var pos:RVector = RVector.subtract(
            this.screen,
            RVector.subtract(
                RVector.multiplyScalar(this.equator, vx),
                RVector.multiplyScalar(this.up, vy)
            )
        );
        pos.y = pos.y * -1;
        var dir:RVector = RVector.subtract(
            pos,
            this.position
        );

        var ray:Ray = new Ray(pos, dir.normalize());

        return ray;
    }

    public function toString ():String {
        return 'Ray []';
    }
}

public class Background {
    public var color:Color;
    public var ambience:Number = 0.0;

    public function Background (color:Color, ambience:Number):void {
        this.color = color;
        this.ambience = ambience;
    }
}

public class Engine {
    public const expectedResult:String = "0,0,5,5,5,0,5,5,10,0,5,5,15,0,5,5,20,0,5,5,25,0,5,5,30,0,5,5,35,0,5,5,40,0,5,5,45,0,5,5,50,0,5,5,55,0,5,5,60,0,5,5,65,0,5,5,70,0,5,5,75,0,5,5,80,0,5,5,85,0,5,5,90,0,5,5,95,0,5,5,0,5,5,5,5,5,5,5,10,5,5,5,15,5,5,5,20,5,5,5,25,5,5,5,30,5,5,5,35,5,5,5,40,5,5,5,45,5,5,5,50,5,5,5,55,5,5,5,60,5,5,5,65,5,5,5,70,5,5,5,75,5,5,5,80,5,5,5,85,5,5,5,90,5,5,5,95,5,5,5,0,10,5,5,5,10,5,5,10,10,5,5,15,10,5,5,20,10,5,5,25,10,5,5,30,10,5,5,35,10,5,5,40,10,5,5,45,10,5,5,50,10,5,5,55,10,5,5,60,10,5,5,65,10,5,5,70,10,5,5,75,10,5,5,80,10,5,5,85,10,5,5,90,10,5,5,95,10,5,5,0,15,5,5,5,15,5,5,10,15,5,5,15,15,5,5,20,15,5,5,25,15,5,5,30,15,5,5,35,15,5,5,40,15,5,5,45,15,5,5,50,15,5,5,55,15,5,5,60,15,5,5,65,15,5,5,70,15,5,5,75,15,5,5,80,15,5,5,85,15,5,5,90,15,5,5,95,15,5,5,0,20,5,5,5,20,5,5,10,20,5,5,15,20,5,5,20,20,5,5,25,20,5,5,30,20,5,5,35,20,5,5,40,20,5,5,45,20,5,5,50,20,5,5,55,20,5,5,60,20,5,5,65,20,5,5,70,20,5,5,75,20,5,5,80,20,5,5,85,20,5,5,90,20,5,5,95,20,5,5,0,25,5,5,5,25,5,5,10,25,5,5,15,25,5,5,20,25,5,5,25,25,5,5,30,25,5,5,35,25,5,5,40,25,5,5,45,25,5,5,50,25,5,5,55,25,5,5,60,25,5,5,65,25,5,5,70,25,5,5,75,25,5,5,80,25,5,5,85,25,5,5,90,25,5,5,95,25,5,5,0,30,5,5,5,30,5,5,10,30,5,5,15,30,5,5,20,30,5,5,25,30,5,5,30,30,5,5,35,30,5,5,40,30,5,5,45,30,5,5,50,30,5,5,55,30,5,5,60,30,5,5,65,30,5,5,70,30,5,5,75,30,5,5,80,30,5,5,85,30,5,5,90,30,5,5,95,30,5,5,0,35,5,5,5,35,5,5,10,35,5,5,15,35,5,5,20,35,5,5,25,35,5,5,30,35,5,5,35,35,5,5,40,35,5,5,45,35,5,5,50,35,5,5,55,35,5,5,60,35,5,5,65,35,5,5,70,35,5,5,75,35,5,5,80,35,5,5,85,35,5,5,90,35,5,5,95,35,5,5,0,40,5,5,5,40,5,5,10,40,5,5,15,40,5,5,20,40,5,5,25,40,5,5,30,40,5,5,35,40,5,5,40,40,5,5,45,40,5,5,50,40,5,5,55,40,5,5,60,40,5,5,65,40,5,5,70,40,5,5,75,40,5,5,80,40,5,5,85,40,5,5,90,40,5,5,95,40,5,5,0,45,5,5,5,45,5,5,10,45,5,5,15,45,5,5,20,45,5,5,25,45,5,5,30,45,5,5,35,45,5,5,40,45,5,5,45,45,5,5,50,45,5,5,55,45,5,5,60,45,5,5,65,45,5,5,70,45,5,5,75,45,5,5,80,45,5,5,85,45,5,5,90,45,5,5,95,45,5,5,0,50,5,5,5,50,5,5,10,50,5,5,15,50,5,5,20,50,5,5,25,50,5,5,30,50,5,5,35,50,5,5,40,50,5,5,45,50,5,5,50,50,5,5,55,50,5,5,60,50,5,5,65,50,5,5,70,50,5,5,75,50,5,5,80,50,5,5,85,50,5,5,90,50,5,5,95,50,5,5,0,55,5,5,5,55,5,5,10,55,5,5,15,55,5,5,20,55,5,5,25,55,5,5,30,55,5,5,35,55,5,5,40,55,5,5,45,55,5,5,50,55,5,5,55,55,5,5,60,55,5,5,65,55,5,5,70,55,5,5,75,55,5,5,80,55,5,5,85,55,5,5,90,55,5,5,95,55,5,5,0,60,5,5,5,60,5,5,10,60,5,5,15,60,5,5,20,60,5,5,25,60,5,5,30,60,5,5,35,60,5,5,40,60,5,5,45,60,5,5,50,60,5,5,55,60,5,5,60,60,5,5,65,60,5,5,70,60,5,5,75,60,5,5,80,60,5,5,85,60,5,5,90,60,5,5,95,60,5,5,0,65,5,5,5,65,5,5,10,65,5,5,15,65,5,5,20,65,5,5,25,65,5,5,30,65,5,5,35,65,5,5,40,65,5,5,45,65,5,5,50,65,5,5,55,65,5,5,60,65,5,5,65,65,5,5,70,65,5,5,75,65,5,5,80,65,5,5,85,65,5,5,90,65,5,5,95,65,5,5,0,70,5,5,5,70,5,5,10,70,5,5,15,70,5,5,20,70,5,5,25,70,5,5,30,70,5,5,35,70,5,5,40,70,5,5,45,70,5,5,50,70,5,5,55,70,5,5,60,70,5,5,65,70,5,5,70,70,5,5,75,70,5,5,80,70,5,5,85,70,5,5,90,70,5,5,95,70,5,5,0,75,5,5,5,75,5,5,10,75,5,5,15,75,5,5,20,75,5,5,25,75,5,5,30,75,5,5,35,75,5,5,40,75,5,5,45,75,5,5,50,75,5,5,55,75,5,5,60,75,5,5,65,75,5,5,70,75,5,5,75,75,5,5,80,75,5,5,85,75,5,5,90,75,5,5,95,75,5,5,0,80,5,5,5,80,5,5,10,80,5,5,15,80,5,5,20,80,5,5,25,80,5,5,30,80,5,5,35,80,5,5,40,80,5,5,45,80,5,5,50,80,5,5,55,80,5,5,60,80,5,5,65,80,5,5,70,80,5,5,75,80,5,5,80,80,5,5,85,80,5,5,90,80,5,5,95,80,5,5,0,85,5,5,5,85,5,5,10,85,5,5,15,85,5,5,20,85,5,5,25,85,5,5,30,85,5,5,35,85,5,5,40,85,5,5,45,85,5,5,50,85,5,5,55,85,5,5,60,85,5,5,65,85,5,5,70,85,5,5,75,85,5,5,80,85,5,5,85,85,5,5,90,85,5,5,95,85,5,5,0,90,5,5,5,90,5,5,10,90,5,5,15,90,5,5,20,90,5,5,25,90,5,5,30,90,5,5,35,90,5,5,40,90,5,5,45,90,5,5,50,90,5,5,55,90,5,5,60,90,5,5,65,90,5,5,70,90,5,5,75,90,5,5,80,90,5,5,85,90,5,5,90,90,5,5,95,90,5,5,0,95,5,5,5,95,5,5,10,95,5,5,15,95,5,5,20,95,5,5,25,95,5,5,30,95,5,5,35,95,5,5,40,95,5,5,45,95,5,5,50,95,5,5,55,95,5,5,60,95,5,5,65,95,5,5,70,95,5,5,75,95,5,5,80,95,5,5,85,95,5,5,90,95,5,5,95,95,5,5";

    
    public var canvas:Object; /* 2d context we can render to */
    public var options:Object;
    public var result:Array = [];
    
    
    public function Engine(options:Object = null){
        if (options) {
          this.options = options;
        } else {
          this.options = {
                canvasHeight: 100,
                canvasWidth: 100,
                pixelWidth: 2,
                pixelHeight: 2,
                renderDiffuse: false,
                renderShadows: false,
                renderHighlights: false,
                renderReflections: false,
                rayDepth: 2
            };
        }

        this.options.canvasHeight /= this.options.pixelHeight;
        this.options.canvasWidth /= this.options.pixelWidth;

        /* TODO: dynamically _include other scripts */
    }

    public function setPixel (x, y, color):void{
        var pxW:int, pxH:int;
        pxW = this.options.pixelWidth;
        pxH = this.options.pixelHeight;

        if (this.canvas) {
          this.canvas.fillStyle = color.toString();
          this.canvas.fillRect (x * pxW, y * pxH, pxW, pxH);
        } else {
           //print(x * pxW, y * pxH, pxW, pxH);
           result.push(x * pxW, y * pxH, pxW, pxH);
        }
    }

    public function renderScene (scene:Scene, canvas:Object = null):Boolean {
        /* Get canvas */
        if (canvas) {
          this.canvas = canvas.getContext("2d");
        } else {
          this.canvas
        }

        var canvasHeight:int = this.options.canvasHeight;
        var canvasWidth:int = this.options.canvasWidth;

        for(var y:int=0; y < canvasHeight; y++){
            for(var x:int=0; x < canvasWidth; x++){
                var yp:Number = y * 1.0 / canvasHeight * 2 - 1;
                        var xp:Number = x * 1.0 / canvasWidth * 2 - 1;

                        var ray:Ray = scene.camera.getRay(xp, yp);

                        var color:Color = this.getPixelColor(ray, scene);

                this.setPixel(x, y, color);
            }
        }
        
        return (result.toString() == expectedResult);
    }

    public function getPixelColor (ray:Ray, scene:Scene):Color{
        var info:IntersectionInfo = this.testIntersection(ray, scene, null);
        if(info.isHit){
            var color = this.rayTrace(info, ray, scene, 0);
            return color;
        }
        return scene.background.color;
    }

    public function testIntersection (ray:Ray, scene:Scene, exclude:Shape):IntersectionInfo {
        var hits:int = 0;
        var best:IntersectionInfo = new IntersectionInfo();
        best.distance = 2000;

        for(var i:int=0; i<scene.shapes.length; i++){
            var shape:Shape = scene.shapes[i];

            if(shape != exclude){
                var info:IntersectionInfo = shape.intersect(ray);
                if(info.isHit && info.distance >= 0 && info.distance < best.distance){
                    best = info;
                    hits++;
                }
            }
        }
        best.hitCount = hits;
        return best;
    }

    public function getReflectionRay (P:RVector,N:RVector,V:RVector):Ray{
        var c1:Number = -N.dot(V);
        var R1:RVector = RVector.add(
            RVector.multiplyScalar(N, 2*c1),
            V
        );
        return new Ray(P, R1);
    }

    public function rayTrace (info:IntersectionInfo, ray:Ray, scene:Scene, depth:Number):Color {
        // Calc ambient
        var color:Color = Color.multiplyScalar(info.color, scene.background.ambience);
        var oldColor:Color = color;
        var shininess:Number = Math.pow(10, info.shape.material.gloss + 1);

        for(var i:int=0; i<scene.lights.length; i++){
            var light:Light = scene.lights[i];

            // Calc diffuse lighting
            var v:RVector = RVector.subtract(
                                light.position,
                                info.position
                            ).normalize();

            if(this.options.renderDiffuse){
                var L:Number = v.dot(info.normal);
                if(L > 0.0){
                    color = Color.add(
                                        color,
                                        Color.multiply(
                                            info.color,
                                            Color.multiplyScalar(
                                                light.color,
                                                L
                                            )
                                        )
                                    );
                }
            }

            // The greater the depth the more accurate the colours, but
            // this is exponentially (!) expensive
            if(depth <= this.options.rayDepth){
                // calculate reflection ray
                if(this.options.renderReflections && info.shape.material.reflection > 0)
                {
                    var reflectionRay:Ray = this.getReflectionRay(info.position, info.normal, ray.direction);
                    var refl:IntersectionInfo = this.testIntersection(reflectionRay, scene, info.shape);
      
                    if (refl.isHit && refl.distance > 0){
                        refl.color = this.rayTrace(refl, reflectionRay, scene, depth + 1);
                    } else {
                        refl.color = scene.background.color;
                              }
      
                        color = Color.blend(
                          color,
                          refl.color,
                          info.shape.material.reflection
                        );
                }

                // Refraction
                /* TODO */
            }

            /* Render shadows and highlights */

            var shadowInfo:IntersectionInfo = new IntersectionInfo();

            if(this.options.renderShadows){
                var shadowRay:Ray = new Ray(info.position, v);

                shadowInfo = this.testIntersection(shadowRay, scene, info.shape);
                if(shadowInfo.isHit && shadowInfo.shape != info.shape /*&& shadowInfo.shape.type != 'PLANE'*/){
                    var vA:Color = Color.multiplyScalar(color, 0.5);
                    var dB:Number = (0.5 * Math.pow(shadowInfo.shape.material.transparency, 0.5));
                    color = Color.addScalar(vA,dB);
                }
            }

            // Phong specular highlights
            if(this.options.renderHighlights && !shadowInfo.isHit && info.shape.material.gloss > 0){
              var Lv:RVector = RVector.subtract(
                                  info.shape.position,
                                  light.position
                              ).normalize();
      
              var E:RVector = RVector.subtract(
                                  scene.camera.position,
                                  info.shape.position
                              ).normalize();
      
              var H:RVector = RVector.subtract(
                                  E,
                                  Lv
                              ).normalize();
      
              var glossWeight:Number = Math.pow(Math.max(info.normal.dot(H), 0), shininess);
              color = Color.add(
                                  Color.multiplyScalar(light.color, glossWeight),
                                  color
                              );
            }
        }
        color.limit();
        return color;
    }
} // class Engine

function renderScene():void{
    var scene:Scene = new Scene();

    scene.camera = new Camera(
                        new RVector(0, 0, -15),
                        new RVector(-0.2, 0, 5),
                        new RVector(0, 1, 0)
                    );

    scene.background = new Background(
                                new Color(0.5, 0.5, 0.5),
                                0.4
                            );

    var sphere:Sphere = new Sphere(
        new RVector(-1.5, 1.5, 2),
        1.5,
        new Solid(
            new Color(0,0.5,0.5),
            0.3,
            0.0,
            0.0,
            2.0
        )
    );

    var sphere1:Sphere = new Sphere(
        new RVector(1, 0.25, 1),
        0.5,
        new Solid(
            new Color(0.9,0.9,0.9),
            0.1,
            0.0,
            0.0,
            1.5
        )
    );

    var plane:Plane = new Plane(
                                new RVector(0.1, 0.9, -0.5).normalize(),
                                1.2,
                                new Chessboard(
                                    new Color(1,1,1),
                                    new Color(0,0,0),
                                    0.2,
                                    0.0,
                                    1.0,
                                    0.7
                                )
                            );

    scene.shapes.push(plane);
    scene.shapes.push(sphere);
    scene.shapes.push(sphere1);

    var light:Light = new Light(
        new RVector(5, 10, -1),
        new Color(0.8, 0.8, 0.8)
    );

    var light1:Light = new Light(
        new RVector(-3, 5, -15),
        new Color(0.8, 0.8, 0.8),
        100
    );

    scene.lights.push(light);
    scene.lights.push(light1);

    var imageWidth:int = 100; // $F('imageWidth');
    var imageHeight:int = 100; // $F('imageHeight');
    var pixelSize:Array = "5,5".split(','); //  $F('pixelSize').split(',');
    var renderDiffuse:Boolean = true; // $F('renderDiffuse');
    var renderShadows:Boolean = true; // $F('renderShadows');
    var renderHighlights:Boolean = true; // $F('renderHighlights');
    var renderReflections:Boolean = true; // $F('renderReflections');
    var rayDepth:int = 2;//$F('rayDepth');

    var raytracer:Engine = new Engine(
        {
            canvasWidth: imageWidth,
            canvasHeight: imageHeight,
            pixelWidth: pixelSize[0],
            pixelHeight: pixelSize[1],
            "renderDiffuse": renderDiffuse,
            "renderHighlights": renderHighlights,
            "renderShadows": renderShadows,
            "renderReflections": renderReflections,
            "rayDepth": rayDepth
        }
    );

    raytracer.renderScene(scene);
}

var RayTrace = new BenchmarkSuite('RayTrace', 932666, [
  new Benchmark('RayTrace', renderScene)
]);

// if running individually, start the Benchmark suite.
try {
  runSuite
} catch (e:ReferenceError) {
  BenchmarkSuite.RunSuites({ NotifyResult: PrintResult,
                           NotifyScore: PrintScore });
}



} // package
