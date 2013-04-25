  //print("sanity check");
  var size;
  //Scene scene;
  var scene;
  /**
   * Lights for the rendering scene
   * Light lights[];
   */
  var lights:Vector.<Light>;

  /**
   * Objects (spheres) for the rendering scene
   * Primitive prim[];
   */
  var prim:Vector.<Primitive>;

  /**
   * The view for the rendering scene
   * View view;
   */
  var view:View;
  /**
   * Temporary ray
   * Ray tRay = new Ray();
   */
   //print("point 1");
  var tRay:Ray = new Ray();
  //print("point2");
  /**
   * Alpha channel
   */
  var alpha:int = 255 << 24;

  /**
   * Null vector (for speedup, instead of <code>new Vec(0,0,0)</code>
   */
  var voidVec:Vec = new Vec(0.0,0.0,0.0);

  /**
   * Temporary vect
   */
  var L:Vec = new Vec(0.0,0.0,0.0);

  /**
   * Current intersection instance (only one is needed!)
   */
  var inter:Isect = new Isect();

  /**
   * Height of the <code>Image</code> to be rendered
   */
  var height:int;

  /**
   * Width of the <code>Image</code> to be rendered
   */
  var width:int;

  var datasizes:Vector.<int> = new Vector.<int>(4);
  datasizes[0] = 50;
  datasizes[1] = 150;
  datasizes[2] = 500;
  datasizes[3] = 7;

  var checksum:Number = 0;
  var lastValue:Number = 0;
  var size:int;

  var numobjects:int;
  if (CONFIG::desktop) {
      var start = new Date();
      JGFrun(0);
      var elapsed = new Date() - start;
  }
  else { // mobile
      var start = getTimer();
      JGFrun(3);
      var elapsed = getTimer() - start;
  }
  print("metric time "+elapsed);
  function JGFsetsize(sizel):void {
    size = sizel;
  }

  function JGFinitialise():void {
    //print("Entering JGFinitialise");
    // set image size
    width = height = datasizes[size];

    // create the objects to be rendered
    scene = createScene();

    // get lights, objects etc. from scene.
    setScene(scene);

    numobjects = scene.getObjects();

  }

  function JGFapplication():void {
    // Set interval to be rendered to the whole picture
    // (overkill, but will be useful to retain this for parallel versions)
    var interval:Interval = new Interval(0, width, height, 0, height, 1);

    // Do the business!
    render(interval);

  }

  function JGFvalidate():void {
    //print("entering JFGvalidate");
    var refval:Vector.<int> = new Vector.<int>(4);
    refval[0] = 2676692;
    refval[1] = 29827635;
    refval[2] = 29827635;
    refval[3] = 5158;
    var dev:Number = checksum - refval[size];
    if (dev != 0) {
      print("Validation failed");
      print("Pixel checksum = " + checksum);
      print("Reference value = " + refval[size]);
    }else
    {
      print("Validation successfull");
    }
  }

  function JGFtidyup():void {
    scene = null;
    lights = null;
    prim = null;
    tRay = null;
    inter = null;
  }

  function JGFrun(size:int):void {
    //print("entering JFGrun");
    JGFsetsize(size);
    JGFinitialise();
    JGFapplication();
    JGFvalidate();
    //JGFtidyup();
  }
  
  /****************Start Class RayTracer*************/
    /**
   * Create and initialize the scene for the rendering picture.
   *
   * @return The scene just created
   */

  function createScene():Scene {
    //print("entering createScene");
    var x:int = 0;
    var y:int = 0;

    var scene:Scene = new Scene();

    /* create spheres */

    var p:Primitive;
    var nx:int = 4;
    var ny:int = 4;
    var nz:int = 4;
    for (var i:int = 0; i < nx; i++) {
      for (var j:int = 0; j < ny; j++) {
        for (var k:int = 0; k < nz; k++) {
          var xx:Number = 20.0 / (nx - 1) * i - 10.0;
          var yy:Number = 20.0 / (ny - 1) * j - 10.0;
          var zz:Number = 20.0 / (nz - 1) * k - 10.0;
          p = new Sphere(new Vec(xx, yy, zz), 3);
          
          // p.setColor(i/(double) (nx-1), j/(double)(ny-1),
          // k/(double) (nz-1));
          p.setColor(0, 0, (i + j) / (nx + ny - 2));
          p.surf.shine = 15.0;
          p.surf.ks = 1.5 - 1.0;
          p.surf.kt = 1.5 - 1.0;
          scene.addObject(p);
        }
      }
    }

    /* Creates five lights for the scene */
    scene.addLight(new Light(100, 100, -50, 1.0));
    scene.addLight(new Light(-100, 100, -50, 1.0));
    scene.addLight(new Light(100, -100, -50, 1.0));
    scene.addLight(new Light(-100, -100, -50, 1.0));
    scene.addLight(new Light(200, 200, 0, 1.0));

    /* Creates a View (viewing point) for the rendering scene */
    var v:View = new View(new Vec(x, 20, -30), new Vec(x, y, 0), new Vec(0, 1,0), 1.0, 35.0 * 3.14159265 / 180.0, 1.0);
    /*
     * v.from = new Vec(x, y, -30); v.at = new Vec(x, y, -15); v.up = new
     * Vec(0, 1, 0); v.angle = 35.0 * 3.14159265 / 180.0; v.aspect = 1.0;
     * v.dist = 1.0;
     *
     */
    scene.setView(v);

    return scene;
  }

  function setScene(scene:Scene):void {
    //print("entering setScene");
    // Get the objects count
    var nLights:int = scene.getLights();
    var nObjects:int = scene.getObjects();

    lights = new Vector.<Light>(nLights);
    prim = new Vector.<Primitive>(nObjects);

    // Get the lights
    for (var l:int = 0; l < nLights; l++) {
      lights[l] = scene.getLight(l);
    }

    // Get the primitives
    for (var o:int = 0; o < nObjects; o++) {
      prim[o] = scene.getObject(o);
    }

    // Set the view
    view = scene.getView();
  }

  function render(interval:Interval):void {
    //print("entering render");
    // Screen variables
    var row:Vector.<int> = new Vector.<int>(interval.width * (interval.yto - interval.yfrom));
    var pixCounter:int = 0; // iterator

    // Rendering variables
    var x:int, y:int, red:int, green:int, blue:int;
    var xlen:Number, ylen:Number;
    var viewVec:Vec;
    
    viewVec = Vec.sub(view.at, view.from);

    viewVec.normalize();

    var tmpVec:Vec = new Vec(0.0,0.0,0.0);
    tmpVec.setVec(viewVec);
    tmpVec.scale(Vec.dot(view.up, viewVec));

    var upVec:Vec = Vec.sub(view.up, tmpVec);
    upVec.normalize();

    var leftVec:Vec = Vec.cross(view.up, viewVec);
    leftVec.normalize();

    var frustrumwidth:Number = view.dist * Math.tan(view.angle);

    upVec.scale(-frustrumwidth);
    leftVec.scale(view.aspect * frustrumwidth);

    var r:Ray = new Ray();
    r.setRay(view.from, voidVec);
    var col:Vec = new Vec(0.0,0.0,0.0);

    // Header for .ppm file
    // System.out.println("P3");
    // System.out.println(width + " " + height);
    // System.out.println("255");

    // All loops are reversed for 'speedup' (cf. thinking in java p331)

    // For each line
    for (y = interval.yfrom; y < interval.yto; y++) {
      //print("outer loop in render :"+y);
      ylen = (2.0 * y) / interval.width - 1.0;
      // System.out.println("Doing line " + y);
      // For each pixel of the line
      for (x = 0; x < interval.width; x++) {
        //print("inner loop in render: "+x);
        xlen = (2.0 * x) / interval.width - 1.0;
        r.D = Vec.comb(xlen, leftVec, ylen, upVec);
        r.D.add(viewVec);
        r.D.normalize();
        //print("executing trace");
        col = trace2(0, 1.0, r);
        if(col == undefined)
        {
          print("col is set: "+col);
          print("r is: "+r);
        }
        // computes the color of the ray
        red = (col.x * 255.0);
        if (red > 255)
          red = 255;
        green = (col.y * 255.0);
        //print("green is set");
        if (green > 255)
          green = 255;
        blue = (col.z * 255.0);
        //print("blue is set");
        if (blue > 255)
          blue = 255;
        //print("adding checksum");
        red = Math.floor(red);
        green = Math.floor(green);
        blue = Math.floor(blue);
        checksum += red;
        checksum += green;
        checksum += blue;
        
        // RGB values for .ppm file
        // System.out.println(red + " " + green + " " + blue);
        // Sets the pixels
        row[pixCounter++] = alpha | (red << 16) | (green << 8) | (blue);
      } // end for (x)
    } // end for (y)

  }

  function intersect( r:Ray,  maxt:Number):Boolean {
    var tp:Isect;
    var i:int, nhits:int;
    //print("entering intersect");
    nhits = 0;
    inter.t = 1e9;
    for (i = 0; i < prim.length; i++) {
      // uses global temporary Prim (tp) as temp.object for speedup
      tp = prim[i].intersect(r);
      if (tp != null && tp.t < inter.t) {
        inter.t = tp.t;
        inter.prim = tp.prim;
        inter.surf = tp.surf;
        inter.enter = tp.enter;
        nhits++;
      }
    }
    return nhits > 0 ? true : false;
  }

  /**
   * Checks if there is a shadow
   *
   * @param r
   *            The ray
   * @return Returns 1 if there is a shadow, 0 if there isn't
   */
  function Shadow( r:Ray,  tmax:Number):int {
    if (intersect(r, tmax))
      return 0;
    return 1;
  }

  /**
   * Return the Vector's reflection direction
   *
   * @return The specular direction
   */
  function SpecularDirection( I:Vec,  N:Vec):Vec {
    var r:Vec;
    r = Vec.comb(1.0 / Math.abs(Vec.dot(I, N)), I, 2.0, N);
    r.normalize();
    return r;
  }

  /**
   * Return the Vector's transmission direction
   */
  function TransDir( m1:Surface,  m2:Surface,  I:Vec,  N:Vec):Vec {
    var n1:Number, n2:Number, eta:Number, c1:Number, cs2:Number;
    var r:Vec;
    n1 = m1 == null ? 1.0 : m1.ior;
    n2 = m2 == null ? 1.0 : m2.ior;
    eta = n1 / n2;
    c1 = -Vec.dot(I, N);
    cs2 = 1.0 - eta * eta * (1.0 - c1 * c1);
    if (cs2 < 0.0)
      return null;
    r = Vec.comb(eta, I, eta * c1 - Math.sqrt(cs2), N);
    r.normalize();
    return r;
  }

  /**
   * Returns the shaded color
   *
   * @return The color in Vec form (rgb)
   */
  function shade( level:int,  weight:Number,  P:Vec,  N:Vec,  I:Vec,  hit:Isect):Vec{
    var n1:Number, n2:Number, eta:Number, c1:Number, cs2:Number;
    var r:Vec;
    var tcol:Vec;
    var R:Vec;
    var t:Number, diff:Number, spec:Number;
    var surf:Surface;
    var col:Vec;
    var l:int;

    col = new Vec(0.0,0.0,0.0);
    surf = hit.surf;
    R = new Vec(0.0,0.0,0.0);
    if (surf.shine > 1e-6) {
      R = SpecularDirection(I, N);
    }

    // Computes the effectof each light
    for (l = 0; l < lights.length; l++) {
      L.sub2(lights[l].pos, P);
      if (Vec.dot(N, L) >= 0.0) {
        t = L.normalize();

        tRay.P = P;
        tRay.D = L;

        // Checks if there is a shadow
        if (Shadow(tRay, t) > 0) {
          diff = Vec.dot(N, L) * surf.kd * lights[l].brightness;

          col.adds_two(diff, surf.color);
          if (surf.shine > 1e-6) {
            spec = Vec.dot(R, L);
            if (spec > 1e-6) {
              spec = Math.pow(spec, surf.shine);
              col.x += spec;
              col.y += spec;
              col.z += spec;
            }
          }
        }
      } // if
    } // for

    tRay.P = P;
    if (surf.ks * weight > 1e-3) {
      tRay.D = SpecularDirection(I, N);
      tcol = trace2(level + 1, surf.ks * weight, tRay);
      col.adds_two(surf.ks, tcol);
    }
    if (surf.kt * weight > 1e-3) {
      if (hit.enter > 0)
        tRay.D = TransDir(null, surf, I, N);
      else
        tRay.D = TransDir(surf, null, I, N);
      tcol = trace2(level + 1, surf.kt * weight, tRay);
      col.adds_two(surf.kt, tcol);
    }

    // garbaging...
    tcol = null;
    surf = null;

    return col;
  }

  /**
   * Launches a ray
   */
  function trace2( level:int,  weight:Number,  r:Ray):Vec {
    //print("entering trace "+ level);
    var P:Vec, N:Vec;
    var hit:Boolean;
    //print("checking recursion in trace");
    // Checks the recursion level
    if (level > 6) {
      return new Vec(0.0,0.0,0.0);
    }

    hit = intersect(r, 1e6);
    //print("hit is: "+hit);
    if (hit) {
      P = r.point(inter.t);
      N = inter.prim.normal(P);
      if (Vec.dot(r.D, N) >= 0.0) {
        N.negate();
      }
      return shade(level, weight, P, N, r.D, inter);
    }
    // no intersection --> col = 0,0,0
    return voidVec;
  }
  /****************End Class RayTracer***************/
  
class Interval {
  /*
   * public int number; public int width; public int height; public int yfrom;
   * public int yto; public int total;
   */
  var number:int;

  var width:int;

  var height:int;

  var yfrom:int;

  var yto:int;

  var total:int;

  function Interval( number:int,  width:int,  height:int,  yfrom:int,  yto:int,  total:int):void {
    this.number = number;
    this.width = width;
    this.height = height;
    this.yfrom = yfrom;
    this.yto = yto;
    this.total = total;
  }
}
class Isect {
  //public double t;
  var t:Number;
  //public int enter;
  var enter:int;
  //public Primitive prim;
  var prim:Primitive;
  //public Surface surf;
  var surf:Surface;
}

class Light {
  //public Vec pos;
  var pos:Vec;
  //public double brightness;
  var brightness:Number;
  function Light( x:Number,  y:Number,  z:Number,  brightnessl:Number) {
    this.pos = new Vec(x, y, z);
    this.brightness = brightnessl;
  }
}



class Ray {
  var P:Vec, D:Vec;
  
  function setRay(pnt, dir):void {
    //print("set ray start");
    P = new Vec(pnt.x, pnt.y, pnt.z);
    D = new Vec(dir.x, dir.y, dir.z);
    //print("set ray after init");
    D.normalize();
    //print("set ray after normalize");
  }
  function Ray():void
  {
    //print("start Ray()");
    P = new Vec(0.0,0.0,0.0);
    D = new Vec(0.0,0.0,0.0);
    //print("end Ray()");
  }

  function point(t):Vec {
    return new Vec(P.x + D.x * t, P.y + D.y * t, P.z + D.z * t);
  }

  //@Override
  function toString():String {
    return "{" + P.toString() + " -> " + D.toString() + "}";
  }
}
class Scene {
  var lights:Vector.<Light>;

  var objects:Vector.<Primitive>;

  var view:View;

  function Scene():void {
    this.lights = new Vector.<Light>();
    this.objects = new Vector.<Primitive>();
  }

  function addLight(l:Light):void {
    this.lights.push(l);
  }

  function addObject(object:Primitive):void {
    this.objects.push(object);
  }

  function setView(view:View):void {
    this.view = view;
  }

  function getView():View {
    return this.view;
  }

  function getLight(number:int):Light {
    return  this.lights[number];
  }

  function getObject( number:int):Primitive {
    return objects[number];
  }

  function getLights():int {
    //print("start getLights");
    return this.lights.length;
  }

  function getObjects():int {
    return this.objects.length;
  }

  function setObject(object:Primitive, pos:int):void {
    this.objects[pos] = object;
  }
}

class Primitive {
  var surf:Surface = new Surface();

  function setColor( r:Number,  g:Number,  b:Number):void {
    surf.color = new Vec(r, g, b);
  }
  //abstract
  function normal(pnt:Vec):Vec{}
  //abstract
  function intersect(ry:Ray):Isect{}

  //abstract, override
  function toString():String{}
  //abstract
  function getCenter():Vec{}
  //abstract
  
  function setCenter(c:Vec):void{}
}

class Sphere  extends Primitive {
  var c:Vec;

  var r:Number, r2:Number;

  var v:Vec, b:Vec; // temporary vecs used to minimize the memory load

  function Sphere( center, radius):void {
    c = center;
    r = radius;
    r2 = r * r;
    v = new Vec(0.0,0.0,0.0);
    b = new Vec(0.0,0.0,0.0);
  }

  //@Override
  override function intersect(ry:Ray):Isect {
    var b:Number, disc:Number, t:Number;
    var ip:Isect;
    v.sub2(c, ry.P);
    b = Vec.dot(v, ry.D);
    disc = b * b - Vec.dot(v, v) + r2;
    if (disc < 0.0) {
      return null;
    }
    disc = Math.sqrt(disc);
    t = (b - disc < 1e-6) ? b + disc : b - disc;
    if (t < 1e-6) {
      return null;
    }
    ip = new Isect();
    ip.t = t;
    ip.enter = Vec.dot(v, v) > r2 + 1e-6 ? 1 : 0;
    ip.prim = this;
    ip.surf = surf;
    return ip;
  }

  //@Override
  override function normal(p:Vec):Vec {
    var r:Vec;
    r = Vec.sub(p, c);
    r.normalize();
    return r;
  }

  //@Override
  override function toString():String {
    return "Sphere {" + c.toString() + "," + r + "}";
  }

  //@Override
  override function getCenter():Vec {
    return c;
  }

  //@Override
  override function setCenter(c:Vec):void {
    this.c = c;
  }
}

class Surface {
  var color:Vec;

  var kd:Number;

  var ks:Number;

  var shine:Number;

  var kt:Number;

  var ior:Number;

  function Surface():void {
    color = new Vec(1, 0, 0);
    kd = 1.0;
    ks = 0.0;
    shine = 0.0;
    kt = 0.0;
    ior = 1.0;
  }

  //@Override
  function toString():String {
    return "Surface { color=" + color + "}";
  }
}
/**
 * This class reflects the 3d vectors used in 3d computations
 */
class Vec {

  /**
   * The x coordinate
   */
  var x:Number;

  /**
   * The y coordinate
   */
  var y:Number;

  /**
   * The z coordinate
   */
  var z:Number;

  x = 0.0;
  y = 0.0;
  z = 0.0;
  
  /**
   * Constructor
   *
   * @param a
   *            the x coordinate
   * @param b
   *            the y coordinate
   * @param c
   *            the z coordinate
   */
  function Vec( a:Number,  b:Number,  c:Number):void {
    x = a;
    y = b;
    z = c;
  }
  /**
   * Copy constructor
   */
  function setVec(a:Vec):void {
    x = a.x;
    y = a.y;
    z = a.z;
  }
  /**
   * Add a vector to the current vector
   *
   * @param: a The vector to be added
   */
  function add( a:Vec):void {
    x += a.x;
    y += a.y;
    z += a.z;
  }

  /**
   * adds: Returns a new vector such as new = sA + B
   */
  function adds( s:Number,  a:Vec,  b:Vec):Vec {
    return new Vec(s * a.x + b.x, s * a.y + b.y, s * a.z + b.z);
  }

  /**
   * Adds vector such as: this+=sB
   *
   * @param: s The multiplier
   * @param: b The vector to be added
   */
  function adds_two( s:Number,  b:Vec):void {
    x += s * b.x;
    y += s * b.y;
    z += s * b.z;
  }

  /**
   * Substracs two vectors
   */
  static function sub( a:Vec,  b:Vec):Vec {
    return new Vec(a.x - b.x, a.y - b.y, a.z - b.z);
  }

  /**
   * Substracts two vects and places the results in the current vector Used
   * for speedup with local variables -there were too much Vec to be gc'ed
   * Consumes about 10 units, whether sub consumes nearly 999 units!! cf
   * thinking in java p. 831,832
   */
  function sub2( a:Vec,  b:Vec):void {
    this.x = a.x - b.x;
    this.y = a.y - b.y;
    this.z = a.z - b.z;
  }

  static function mult( a:Vec,  b:Vec):Vec {
    return new Vec(a.x * b.x, a.y * b.y, a.z * b.z);
  }

  static function cross( a:Vec,  b:Vec):Vec {
    return new Vec(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y
        - a.y * b.x);
  }

  static function dot( a:Vec,  b:Vec):Number {
    return a.x * b.x + a.y * b.y + a.z * b.z;
  }

  static function comb( a:Number,  A:Vec,  b:Number,  B:Vec):Vec {
    return new Vec(a * A.x + b * B.x, a * A.y + b * B.y, a * A.z + b * B.z);
  }

  function comb2( a:Number,  A:Vec,  b:Number,  B:Vec):void {
    x = a * A.x + b * B.x;
    y = a * A.y + b * B.y;
    z = a * A.z + b * B.z;
  }

  function scale( t:Number):void {
    x *= t;
    y *= t;
    z *= t;
  }

  function negate():void {
    x = -x;
    y = -y;
    z = -z;
  }

  function normalize():Number {
    var len:Number;
    len = Math.sqrt(x * x + y * y + z * z);
    if (len > 0.0) {
      x /= len;
      y /= len;
      z /= len;
    }
    return len;
  }

  //@Override
  function toString():String {
    return "<" + x + "," + y + "," + z + ">";
  }
}

class View {
  /*
   * public Vec from; public Vec at; public Vec up; public double dist; public
   * double angle; public double aspect;
   */
  var from:Vec;

  var at:Vec;

  var up:Vec;

  var dist:Number;

  var angle:Number;

  var aspect:Number;

  function View( froml:Vec,  atl:Vec,  upl:Vec,  distl:Number,  anglel:Number, aspectl:Number):void {
    this.from = froml;
    this.at = atl;
    this.up = upl;
    this.dist = distl;
    this.angle = anglel;
    this.aspect = aspectl;
  }
}


//PEYER - Vector class now builtin

/*****************Start Vector class*****************/
//http://sourceforge.net/projects/jsvector/
// Vector Constructor -- constructs the object
// Vector Constructor -- constructs the object
/*
function Vector(inc) {
  if (inc == 0) {
    inc = 100;
  }
  
  // Properties
  this.data = new Array(inc);
  this.increment = inc;
  this.size = 0;
  
  // Methods
  this.getCapacity = getCapacity;
  this.getSize = getSize;
  this.isEmpty = isEmpty;
  this.getLastElement = getLastElement;
  this.getFirstElement = getFirstElement;
  this.getElementAt = getElementAt;
  this.addElement = addElement;
  this.insertElementAt = insertElementAt;
  this.removeElementAt = removeElementAt;
  this.removeAllElements = removeAllElements;
  this.indexOf = indexOf;
  this.contains = contains
  this.resize = resize;
  this.toString = toString;
  this.sort = sort;
  this.trimToSize = trimToSize;
  this.clone = clone;
  this.overwriteElementAt;
}

// getCapacity() -- returns the number of elements the vector can hold
function getCapacity() {
  return this.data.length;
}

// getSize() -- returns the current size of the vector
function getSize() {
  return this.size;
}

// isEmpty() -- checks to see if the Vector has any elements
function isEmpty() {
  return this.getSize() == 0;
}

// getLastElement() -- returns the last element
function getLastElement() {
  if (this.data[this.getSize() - 1] != null) {
    return this.data[this.getSize() - 1];
  }
}

// getFirstElement() -- returns the first element
function getFirstElement() {
  if (this.data[0] != null) {
    return this.data[0];
  }
}

// getElementAt() -- returns an element at a specified index
function getElementAt(i) {
  try {
    return this.data[i];
  }
  catch (e) {
    return "Exception " + e + " occured when accessing " + i;
  }
}

// addElement() -- adds a element at the end of the Vector
function addElement(obj) {
  if(this.getSize() == this.data.length) {
    this.resize();
  }
  this.data[this.size++] = obj;
}

// insertElementAt() -- inserts an element at a given position
function insertElementAt(obj, index) {
  try {
    if (this.size == this.capacity) {
      this.resize();
    }
    
    for (var i=this.getSize(); i > index; i--) {
      this.data[i] = this.data[i-1];
    }
    this.data[index] = obj;
    this.size++;
  }
  catch (e) {
    return "Invalid index " + i;
  }
}

// removeElementAt() -- removes an element at a specific index
function removeElementAt(index) {
  try {
    var element = this.data[index];
    
    for(var i=index; i<(this.getSize()-1); i++) {
      this.data[i] = this.data[i+1];
    }
    
    this.data[getSize()-1] = null;
    this.size--;
    return element;
  }
  catch(e) {
    return "Invalid index " + index;
  }
}

// removeAllElements() -- removes all elements in the Vector
function removeAllElements() {
  this.size = 0;
  
  for (var i=0; i<this.data.length; i++) {
    this.data[i] = null;
  }
}

// indexOf() -- returns the index of a searched element
function indexOf(obj) {
  for (var i=0; i<this.getSize(); i++) {
    if (this.data[i] == obj) {
      return i;
    }
  }
  return -1;
}

// contains() -- returns true if the element is in the Vector, otherwise false
function contains(obj) {
  for (var i=0; i<this.getSize(); i++) {
    if (this.data[i] == obj) {
      return true;
    }
  }
  return false;
}

// resize() -- increases the size of the Vector
function resize() {
  newData = new Array(this.data.length + this.increment);
  
  for (var i=0; i< this.data.length; i++) {
    newData[i] = this.data[i];
  }
  
  this.data = newData;
}


// trimToSize() -- trims the vector down to it's size
function trimToSize() {
  var temp = new Array(this.getSize());
  
  for (var i = 0; i < this.getSize(); i++) {
    temp[i] = this.data[i];
  }
  this.size = temp.length - 1;
  this.data = temp;
}

// sort() - sorts the collection based on a field name - f
function sort(f) {
  var i, j;
  var currentValue;
  var currentObj;
  var compareObj;
  var compareValue;
  
  for(i=1; i<this.getSize();i++) {
    currentObj = this.data[i];
    currentValue = currentObj[f];
    
    j= i-1;
    compareObj = this.data[j];
    compareValue = compareObj[f];
    
    while(j >=0 && compareValue > currentValue) {
      this.data[j+1] = this.data[j];
      j--;
      if (j >=0) {
        compareObj = this.data[j];
        compareValue = compareObj[f];
      }
    }
    this.data[j+1] = currentObj;
  }
}

// clone() -- copies the contents of a Vector to another Vector returning the new Vector.
function clone() {
  var newVector = new Vector(this.size);
  
  for (var i=0; i<this.size; i++) {
    newVector.addElement(this.data[i]);
  }
  
  return newVector;
}

// toString() -- returns a string rep. of the Vector
function toString() {
  var str = "Vector Object properties:\n" +
            "Increment: " + this.increment + "\n" +
            "Size: " + this.size + "\n" +
            "Elements:\n";
  
  for (var i=0; i<getSize(); i++) {
    for (var prop in this.data[i]) {
      var obj = this.data[i];
      str += "\tObject." + prop + " = " + obj[prop] + "\n";
    }
  }
  return str;
}

// overwriteElementAt() - overwrites the element with an object at the specific index.
function overwriteElementAt(obj, index) {
  this.data[index] = obj;
}
*/
/*****************End Vector class*******************/