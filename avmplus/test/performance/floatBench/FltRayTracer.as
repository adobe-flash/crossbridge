/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package Raytracer {

import Math;

public final class vector3
{
public var x:float;
public var y:float;
public var z:float;

public function vector3( a_X:float ,  a_Y:float , a_Z:float ) {  x= a_X; y=a_Y; z=a_Z;};
public function scalarMultiply(val: float):vector3 { x*=val; y*=val; z*=val; return this;}
public function Multiply(val: vector3):vector3 { x*=val.x; y*=val.y; z*=val.z; return this;}
public function Add(val: vector3):vector3 { x+=val.x; y+=val.y; z+=val.z; return this;}
public function Subtract(val: vector3):vector3 { x-=val.x; y-=val.y; z-=val.z; return this;}
public function Set( a_X:float ,  a_Y:float , a_Z:float ):void {  x= a_X; y=a_Y; z=a_Z;};
};


public function sqrtf(v:float):float { return float(Math.sqrt(v));}

public function DOT(A:vector3,B:vector3):float { return (A.x*B.x+A.y*B.y+A.z*B.z);}
public function NORMALIZE(A:vector3):void {
	var l:float=1.0f/sqrtf(A.x*A.x+A.y*A.y+A.z*A.z);
	A.x*=l;A.y*=l;A.z*=l;
}
public function SUBTRACT(A:vector3,B:vector3):vector3 { return new vector3(A.x-B.x,A.y-B.y,A.z-B.z);}
public function ADD_SCALED(A:vector3,B:vector3, s:float):vector3 { return new vector3(A.x+s*B.x,A.y+s*B.y,A.z+s*B.z);}
public function SCALAR_MULTIPLY(s:float,A:vector3):vector3 { return new vector3(A.x*s,A.y*s,A.z*s);}


var c1:uint =0;
var c2:uint =0;
var c3:uint =0;
var c4:uint =0;
var c5:uint =0;

const TRACEDEPTH:int = 6;

public function fletcher32( data:Vector.<uint>, len:int ):uint
{
	var sum1:uint = 0xffff, sum2:uint = 0xffff;
    var i:int=0;

	while (len) {
		var tlen:uint = len > 180 ? 180 : len;
		len -= tlen;
		do {
		    var d1:uint = data[i]; i++;
			var d2:uint = d1 >> 16;
			d1 = d1 & 0xffff;
			
			sum1 += d1;
			sum2 += sum1;
			sum1 += d2;
			sum2 += sum1;

		} while (--tlen);
		sum1 = (sum1 & 0xffff) + (sum1 >> 16);
		sum2 = (sum2 & 0xffff) + (sum2 >> 16);
	}
	/* Second reduction step to reduce sums to 16 bits */
	sum1 = (sum1 & 0xffff) + (sum1 >> 16);
	sum2 = (sum2 & 0xffff) + (sum2 >> 16);
	return sum2 << 16 | sum1;
}

public final class plane
{
	public var N:vector3;
	public var D:float;
	public function	plane( a_Normal:vector3, a_D:float ) { N=a_Normal; D = a_D;};
};

public final class Surface
{
	public static const OWNER:int = 1;
	private var m_Buffer:Vector.<uint>;
	private var m_Width:int;
	private var m_Height:int;
	
	// constructor / destructors
	public function Surface( a_Width:int,  a_Height:int ){
		m_Width  = a_Width;
		m_Height = a_Height;
		m_Buffer = new Vector.<uint>(a_Width*a_Height,true);
	}
	
	public final function GetBuffer():Vector.<uint> { return m_Buffer;}

	public function  Clear(  a_Color:uint ):void{
		var s:int = m_Width * m_Height;
		for (var i:Number = 0; i < s; i++ ) m_Buffer[i] = a_Color;
	}
};

public final class FloatRef
{
	public var value:float;

	public function FloatRef(value:float)
	{
		this.value = value;
	}
}

// Intersection method return values
public const HIT:int=1;		// Ray hit primitive
public const MISS:int=0		// Ray missed primitive
public const INPRIM:int=-1		// Ray started inside primitive

// -----------------------------------------------------------
// Material class definition
// -----------------------------------------------------------

public final class Material
{
	public function Material() { m_Color = new vector3(0.2f,0.2f,0.2f); m_Refl=0f; m_Diff = 0.2f; }
	public final function SetColor(  a_Color:vector3 ):void { m_Color = a_Color; }
	public final function GetColor():vector3 { return m_Color; }
	public final function SetDiffuse(  a_Diff:float ):void  { m_Diff = a_Diff; }
	public final function SetReflection( a_Refl:float  ):void  { m_Refl = a_Refl; }
	public final function GetSpecular():float { return 1.0f - m_Diff; }
	public final function GetDiffuse():float { return m_Diff; }
	public final function GetReflection():float { return m_Refl; }

	private var m_Color:vector3;
	private var m_Refl:float;
	private var m_Diff:float;
};

// -----------------------------------------------------------
// Primitive class definition
// -----------------------------------------------------------

public class Primitive
{
	public static const SPHERE:int = 1;
	public static const PLANE:int = 2;

	public function Primitive() { m_Light = false; m_Material = new Material(); };
	public final function GetMaterial():Material { return m_Material; }
	public final function SetMaterial( a_Mat:Material):void { m_Material = a_Mat; }
	public virtual function GetType():int { throw Error("abstract method");return 0;}
	public virtual function Intersect( a_Ray:Ray, a_Dist:FloatRef ):int { throw Error("abstract method");return 0;}
	public virtual function GetNormal( a_Pos:vector3 ):vector3 { throw Error("abstract method");return a_Pos;};
	public virtual function GetColor( a_Pos:vector3 ):vector3  { return m_Material.GetColor(); }
	public virtual function Light( a_Light:Boolean ):void { m_Light = a_Light; }
	public virtual function IsLight():Boolean { return m_Light; }

	protected var m_Material:Material;
	protected var m_Light:Boolean;
};

// -----------------------------------------------------------
// Sphere primitive class definition
// -----------------------------------------------------------

public final class Sphere extends Primitive
{

	override public virtual function GetType():int { return Primitive.SPHERE;}
	public function Sphere(  a_Centre:vector3,  a_Radius:float ){
		m_Centre= a_Centre;
		m_SqRadius =a_Radius * a_Radius; 
		m_Radius = a_Radius;
		m_RRadius = 1.0f / a_Radius; 
	}
	public final function GetCentre():vector3 { return m_Centre; }
	public final function GetSqRadius():float { return m_SqRadius; }
	override public virtual function Intersect( a_Ray:Ray, a_Dist:FloatRef ):int {
		var v:vector3 = SUBTRACT( a_Ray.GetOrigin() , m_Centre);
		var b:float = -DOT( v, a_Ray.GetDirection() );
		var det:float = (b * b) - DOT( v, v ) + m_SqRadius;
		var retval:int = MISS;

		if (det > 0)
		{
			det = sqrtf( det );
			var i1:float = b - det;
			var i2:float = b + det;

			if (i2 > 0)
			{
				if (i1 < 0) 
				{
					if (i2 < a_Dist.value) 
					{
						a_Dist.value = i2;
						retval = INPRIM;
					}
				}
				else
				{
					if (i1 < a_Dist.value)
					{
						a_Dist.value = i1;
						retval = HIT;
					}
				}
			}
		}
		return retval;
	}
	override public virtual function GetNormal(  a_Pos:vector3 ):vector3 { return SUBTRACT(a_Pos,m_Centre).scalarMultiply(m_RRadius); }

	private var m_Centre: vector3 ;
	private var m_SqRadius:float, m_Radius:float, m_RRadius:float;
};

// -----------------------------------------------------------
// PlanePrim primitive class definition
// -----------------------------------------------------------

public final class PlanePrim extends Primitive
{
	override public virtual function GetType():int { return Primitive.PLANE;}
	public function PlanePrim( a_Normal:vector3, a_D:float) { m_Plane = new plane(a_Normal, a_D); };
	public function  GetD():float { return m_Plane.D; }
	override public virtual function Intersect( a_Ray:Ray, a_Dist:FloatRef ):int {
		var d:float = DOT( m_Plane.N, a_Ray.GetDirection() );
		if (d != 0f)
		{
			var dist:float = -(DOT( m_Plane.N, a_Ray.GetOrigin() ) + m_Plane.D) / d;
			if (dist > 0f)
			{
				if (dist < a_Dist.value ) 
				{
					a_Dist.value = dist;
					return HIT;
				}
			}
		}
		return MISS;
	}
	override public virtual function GetNormal( a_Pos:vector3 ):vector3 {return m_Plane.N;}

	private var m_Plane:plane;
};

// -----------------------------------------------------------
// Scene class definition
// -----------------------------------------------------------

public final class Scene
{
	public function Scene() {
		m_primitiveCount = 5;
		m_Primitive = new Vector.<Primitive>(5,true);
	};
	public final function InitScene():void{
		// set number of primitives
		// ground plane
		m_Primitive[0] = new PlanePrim( new vector3( 0f, 1f, 0f ), 4.4f );
//		m_Primitive[0]->SetName( "plane" );
		m_Primitive[0].GetMaterial().SetReflection( 0f );
		m_Primitive[0].GetMaterial().SetDiffuse( 1.0f );
		m_Primitive[0].GetMaterial().SetColor( new vector3( 0.4f, 0.3f, 0.3f ) );
		// big sphere
		m_Primitive[1] = new Sphere( new vector3( 1f, -0.8f, 3f ), 2.5f );
//		m_Primitive[1]->SetName( "big sphere" );
		m_Primitive[1].GetMaterial().SetReflection( 0.6f );
		m_Primitive[1].GetMaterial().SetColor( new vector3( 0.7f, 0.7f, 0.7f ) );
		// small sphere
		m_Primitive[2] = new Sphere( new vector3( -5.5f, -0.5f, 7f ), 2f );
//		m_Primitive[2]->SetName( "small sphere" );
		m_Primitive[2].GetMaterial().SetReflection( 1.0f );
		m_Primitive[2].GetMaterial().SetDiffuse( 0.1f );
		m_Primitive[2].GetMaterial().SetColor( new vector3( 0.7f, 0.7f, 1.0f ) );
		// light source 1
		m_Primitive[3] = new Sphere( new vector3( 0f, 5f, 5f ), 0.1f );
		m_Primitive[3].Light( true );
		m_Primitive[3].GetMaterial().SetColor( new vector3( 0.6f, 0.6f, 0.6f ) );
		// light source 2
		m_Primitive[4] = new Sphere( new vector3( 2f, 5f, 1f ), 0.1f );
		m_Primitive[4].Light( true );
		m_Primitive[4].GetMaterial().SetColor( new vector3( 0.7f, 0.7f, 0.9f ) );
	}
	public function GetNrPrimitives():int { return m_primitiveCount; }
	public function GetPrimitive( a_Idx:int ):Primitive { return m_Primitive[a_Idx]; }
	
	private var m_primitiveCount:int;
	private var m_Primitive:Vector.<Primitive>;
};

// -----------------------------------------------------------
// Ray class definition
// -----------------------------------------------------------
public final class Ray
{
	public function	Ray( a_Origin:vector3, a_Dir:vector3 ) {m_Origin = a_Origin;m_Direction = a_Dir; }
	public final function SetOrigin( a_Origin:vector3 ):void { m_Origin = a_Origin; }
	public final function SetDirection( a_Direction:vector3 ):void { m_Direction = a_Direction; }
	public final function GetOrigin():vector3 { return m_Origin; }
	public final function GetDirection():vector3 { return m_Direction; }
	
	private var m_Origin:vector3;
	private var m_Direction:vector3;
};

// -----------------------------------------------------------
// Engine class definition
// Raytracer core
// -----------------------------------------------------------
public final class Engine
{
	public function Engine() { m_Scene = new Scene();}
	public final function SetTarget( a_Dest: Vector.<uint>, a_Width:int ,  a_Height:int ):void {
		// set pixel buffer address & size
		m_Dest = a_Dest;
		m_Width = a_Width;
		m_Height = a_Height;
	}
	public final function GetScene():Scene { return m_Scene; }
	
	// -----------------------------------------------------------
	// Engine::Raytrace
	// Naive ray tracing: Intersects the ray with every primitive
	// in the scene to determine the closest intersection
	// -----------------------------------------------------------
	public final function Raytrace( a_Ray:Ray, a_Acc:vector3, a_Depth:int , a_RIndex:float, a_Dist:FloatRef ):Primitive
	{
		++c1;
		if (a_Depth > TRACEDEPTH) return null;
		// trace primary ray
		a_Dist.value = 1000000.0f;
		var pi:vector3;
		var prim:Primitive = null;
		var result:int;
		// find the nearest intersection
		for ( var s:int  = 0; s < m_Scene.GetNrPrimitives(); ++s)
		{
			var pr:Primitive = m_Scene.GetPrimitive( s );
			var res:int;
			if (res = pr.Intersect( a_Ray, a_Dist )) 
			{
				prim = pr;
				result = res; // 0 = miss, 1 = hit, -1 = hit from inside primitive
			}
		}
		++c2;
		// no hit, terminate ray
		if (prim==null) return null;
		// handle intersection
		if (prim.IsLight())
		{
			++c4;
			// we hit a light, stop tracing
			a_Acc.Set( 1f, 1f, 1f );
		}
		else
		{
			++c5;
			// determine color at point of intersection
			pi = ADD_SCALED(a_Ray.GetOrigin(), a_Ray.GetDirection() , a_Dist.value);
			// trace lights
			for ( var l:int = 0; l < m_Scene.GetNrPrimitives(); l++ )
			{
				var p:Primitive = m_Scene.GetPrimitive( l );
				if (p.IsLight()) 
				{
					var light:Primitive = p;
					// calculate diffuse shading
					var L:vector3 = SUBTRACT( (light as Sphere).GetCentre() , pi);
					NORMALIZE( L );
					var N:vector3 = prim.GetNormal( pi );
					if (prim.GetMaterial().GetDiffuse() > 0)
					{
						var dot:float = DOT( N, L );
						if (dot > 0)
						{
							var diff:float = dot * prim.GetMaterial().GetDiffuse();
							// add diffuse component to ray color
							a_Acc.Add(
  							   SCALAR_MULTIPLY( diff,  prim.GetMaterial().GetColor()).Multiply(light.GetMaterial().GetColor()) 
							);
						}
					}
				}
			}
		}
		++c3;
		// return pointer to primitive hit by primary ray
		return prim;
	}
	
	// -----------------------------------------------------------
	// Engine::InitRender
	// Initializes the renderer, by resetting the line / tile
	// counters and precalculating some values
	// -----------------------------------------------------------
	public function InitRender():void
	{
		// set pixel buffer address of first pixel
		m_PPos = 20 * m_Width;
		// screen plane in world space coordinates
		m_WX1 = -4f, m_WX2 = 4f, m_WY1 = m_SY = 3f, m_WY2 = -3f;
		// calculate deltas for interpolation
		m_DX = (m_WX2 - m_WX1) / m_Width;
		m_DY = (m_WY2 - m_WY1) / m_Height;
		m_SY += 20f * m_DY;
	}
	
	// -----------------------------------------------------------
	// Engine::Render
	// Fires rays in the scene one scanline at a time, from left
	// to right
	// -----------------------------------------------------------
	public final function Render():uint
	{
		// render scene
		var o:vector3 = new vector3( 0f, 0f, -5f );

		var lastprim:Primitive = null;
		// render remaining lines
		for ( var y:int = 20; y < (m_Height - 20); y++ )
		{
			m_SX = m_WX1;
			// render pixels for current line
			for ( var x:int = 0; x < m_Width; x++ )
			{
				// fire primary ray
				var acc:vector3 = new vector3( 0f, 0f, 0f ); // Color
				var dir:vector3 = new vector3( m_SX, m_SY, 0f );
				dir.Subtract(o);
				NORMALIZE( dir );
				var r:Ray = new Ray( o, dir );
				var dist:FloatRef = new FloatRef(0f);
				var prim:Primitive = Raytrace( r, acc, 1, 1.0f, dist );
				var red:int = acc.x * 256;
				var green:int = acc.y * 256;
				var blue:int = acc.z * 256;
				if (red > 255) red = 255;
				if (green > 255) green = 255;
				if (blue > 255) blue = 255;
				var val:uint = (red << 16) + (green << 8) + blue;
				m_Dest[m_PPos++] = val;
				m_SX += m_DX;
			}
			m_SY += m_DY;
		}

		return fletcher32(m_Dest,m_Width*m_Height);
	}

	// renderer data
	private var m_WX1:float, m_WY1:float, m_WX2:float, m_WY2:float, m_DX:float, m_DY:float, m_SX:float, m_SY:float;
	private var m_Scene:Scene;
	private var m_Dest:Vector.<uint>;
	private var m_Width:int, m_Height:int, m_PPos:int;
};


const SCRWIDTH:int = 800;
const SCRHEIGHT:int = 600;
const NITER:int = 10;
const CHECKSUM:uint = 0x5c73810a;
var surface:Surface;
var buffer:Vector.<uint>;
var tracer:Engine;

public function main():void
{
	surface = new Surface( SCRWIDTH, SCRHEIGHT );
	buffer = surface.GetBuffer();
	surface.Clear( 0 );
	// prepare renderer
	tracer = new Engine();
	tracer.GetScene().InitScene();
	tracer.SetTarget( surface.GetBuffer(), SCRWIDTH, SCRHEIGHT );
	// go

	var fstart:uint  = getTimer();
	var result:uint;
	for(var idx:int=0;idx<NITER;idx++){
		tracer.InitRender();
		 result = tracer.Render() ;
	}
	
	var ftime:uint  = getTimer() - fstart;
	trace( "Average iteration time: ", ftime/NITER);
	trace( (result==CHECKSUM)? "CHECKSUM OK":"CHECKSUM FAILED, got "+result.toString(16)+" expected "+CHECKSUM.toString(16));
	var iterCheck:String="";
	if(c1!=448000*NITER) iterCheck+=" c1("+c1+")";
	if(c2!=448000*NITER) iterCheck+=" c2("+c2+")";
	if(c3!=257022*NITER) iterCheck+=" c3("+c3+")";
	if(c4!=89*NITER)     iterCheck+=" c4("+c4+")";
	if(c5!=256933*NITER) iterCheck+=" c5("+c5+")";
	if(iterCheck.length==0){
		trace("Iteration checks ok, test is likely valid");
		trace("metric time ",ftime/NITER);
	} else
		trace("TEST IRRELEVANT. Failed iteration counters:"+iterCheck);
}

main();

}; // package Raytracer