/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* -*- tab-width: 4; indent-tabs-mode: nil -*- */
// From www.laserpirate.com/as3raytracer/
// Copyright status uncertain.
//
// Modifications from the original:
//
//  - added a type annotation to numSpheres
//  - provided a definition of BlankClip
//  - constants for WIDTH and HEIGHT, and doubled those from the original
//  - use Vector.<Number> instead of Array for auxiliary data
//  - different FPS computation
//
// java -jar asc.jar -import playerglobal.abc -import builtin.abc -swf RayTracer,640,480,100,version=16 -AS3 -strict RayTracer-Num-V.as
// java -jar asc.jar -import playerglobal.abc -import builtin.abc -swf RayTracer,640,480,100,version=16 -AS3 -strict -abcfuture RayTracer-Num-V.as

package
{
import flash.display.MovieClip;
import flash.display.Sprite;
import flash.display.Bitmap;
import flash.display.BitmapData;
import flash.events.Event;
import flash.utils.getTimer;
import flash.utils.Dictionary;
import flash.system.*;
import flash.events.MouseEvent;
import flash.text.TextField;
import flash.text.TextFormat;
use namespace async;

public class BlankClip extends MovieClip {}

public class RayTracer extends Sprite
{
    private var t:Number;
    private var frames:Number;
    private var lastTime:Number;
    private var dt:Number = .01;
    private var frameTimeTxt:TextField;

    private static const WIDTH:int = 640; // 320;
    private static const HEIGHT:int = 480;  // 240;

    public static const BUFFER_WIDTH:int = WIDTH / 2;
    public static const BUFFER_HEIGHT:int = HEIGHT / 2;
    public static const BUFFER_SCALEDDOWN:int = WIDTH / BUFFER_WIDTH;
    
    public static const HALF_BUFFER_WIDTH:int = BUFFER_WIDTH / 2;
    public static const HALF_BUFFER_HEIGHT:int = BUFFER_HEIGHT / 2;
    
    private var outputBitmapData:BitmapData;
    private var outputBitmap:Bitmap;
    
    public var FOV:Number = 25;
    
    public var sphereCenterX:Vector.<Number> = new <Number> [0,       0,    0,       0,  2.5, -2.5,  0, -2.5, 2.5];
    public var sphereCenterY:Vector.<Number> = new <Number> [0,     -.2,   .4,   100.5,   -3,    3, -3,   -3,  -3];
    public var sphereCenterZ:Vector.<Number> = new <Number> [1.5,   1.5,  1.5,      10,    1,    1,  1,    1,   1];
    public var sphereRadius:Vector.<Number>  = new <Number> [.35, .35,  .25,  100, .25, .25,  .3, .2, .2];
    public var sphereR:Vector.<Number>       = new <Number> [255,   0,    0,  120,   0,   0, 120, 120, 120];
    public var sphereG:Vector.<Number>       = new <Number> [0,   150,    0,  120, 255, 255,   0, 0, 0];
    public var sphereB:Vector.<Number>       = new <Number> [0,     0,  255,  200, 255, 255,  10, 0, 0];
    public var sphereReflects:Vector.<Boolean> = new <Boolean> [false, false, false, true, true, true, true, true, true];
    public var sphere2dX:Vector.<Number> = new Vector.<Number>(sphereCenterX.length);
    public var sphere2dY:Vector.<Number> = new Vector.<Number>(sphereCenterX.length);
    public var sphere2dR:Vector.<Number> = new Vector.<Number>(sphereCenterX.length);
    
    public var numSpheres:int = sphereCenterX.length;
    
    var skyR:int =  150;
    var skyG:int =  150;
    var skyB:int =  250;
    var skyColor:int = (skyR<<16) + (skyG<<8) + skyB;
    var ambientIllumination:Number = .1;
    
    var canvas:BlankClip;

    var theta:Number = 0;
    var mouseIsDown:Boolean = false;
    var mouseDownTheta:Number = 0;
    var mouseDownX:Number = 0;

    var workers:Array=new Array();
    var promises:Array=new Array();
    var promises2:Array=new Array();
    var numWorkers:uint=4;
    
    public function RayTracer()
    {
        if (Worker.current.isPrimordial()) {
        outputBitmapData = new BitmapData(BUFFER_WIDTH, BUFFER_HEIGHT, false);
        outputBitmap = new Bitmap(outputBitmapData);
        addChild(outputBitmap);
        //outputBitmap.smoothing = true;
        
        outputBitmap.width= WIDTH;
        outputBitmap.height = HEIGHT;
        
        canvas = new BlankClip;
        addChild(canvas);
        canvas.buttonMode = true;
        canvas.useHandCursor = true;

        frameTimeTxt = new TextField();
        frameTimeTxt.defaultTextFormat = new TextFormat("Arial");
        frameTimeTxt.x = 8;
        frameTimeTxt.y = 8;
        frameTimeTxt.width = 640;
        frameTimeTxt.textColor = 0x0;
        frameTimeTxt.selectable = false;
        addChild(frameTimeTxt);
        
        t = lastTime = getTimer();
        frames = 0;
        addEventListener(Event.ENTER_FRAME, update, false, 0, true);

        while ((BUFFER_WIDTH % (numWorkers+1))>0) {
            numWorkers=numWorkers-1;
        }
        
        for (var i:uint=0;i<numWorkers;i++) {
            workers[i]=WorkerDomain.current.createWorkerFromPrimordial();
            promises[i]=workers[i].start();
        }
    }
    }
    
    public function update(e:*) {

        if (Worker.current.isPrimordial()) {
        // start frame timer and update global time
        var timer:Number = getTimer();
        t += dt;
        
        // handle mouse rotation
        if( mouseIsDown ) theta = mouseDownTheta - .0015 * (stage.mouseX - mouseDownX);
        theta += dt;
        
        // do some funky animation
        sphereCenterX[0] = .5*Math.sin(theta*5);
        sphereCenterZ[0] =1 + .5*Math.cos(theta*5);
        
        sphereCenterX[1] = .5*Math.sin(theta*5 + 2 * Math.PI / 3);
        sphereCenterZ[1] = 1 + .5*Math.cos(theta*5 + 2 * Math.PI / 3);
        
        sphereCenterX[2] = .5*Math.sin(theta*5 + 4 * Math.PI / 3);
        sphereCenterZ[2] = 1 + .5*Math.cos(theta*5 + 4 * Math.PI / 3);
        
        sphereCenterY[4] = .5*Math.sin(theta*5 + 4 * Math.PI / 3);

        sphereCenterY[5] = -.5*Math.sin(theta*5 + 4 * Math.PI / 3);

        sphereCenterX[6] = .5*Math.sin(theta*5 + 4 * Math.PI / 3);

        sphereCenterZ[7] = 1 + .5*Math.cos(theta*5 + 2 * Math.PI / 3);

        sphereCenterZ[8] = 1 - .5*Math.cos(theta*5 + 2 * Math.PI / 3);

        for (var i:uint=0;i<numWorkers;i++) {
            trace("worker "+i+" startx="+i*(BUFFER_WIDTH/(numWorkers+1))+" endx="+(i+1)*(BUFFER_WIDTH/(numWorkers+1)));
            promises2[i]=promises[i].updatePixels(i*(BUFFER_WIDTH/(numWorkers+1)),(i+1)*(BUFFER_WIDTH/(numWorkers+1)),sphereCenterX,sphereCenterY,sphereCenterZ);
        }

        trace("primordial: x="+(BUFFER_WIDTH*numWorkers/(numWorkers+1))+" endx="+BUFFER_WIDTH);
        var results0:Vector.<uint>=updatePixels(BUFFER_WIDTH*numWorkers/(numWorkers+1),BUFFER_WIDTH,sphereCenterX,sphereCenterY,sphereCenterZ);

        var results:Array=new Array();
        for (var i:uint=0;i<numWorkers;i++) {
            results.push(promises2[i].receive());
        }
        results.push(results0);

        outputBitmapData.lock();
        for (var i:uint=0;i<numWorkers+1;i++) {
            for (var y=0;y<BUFFER_HEIGHT;y++) {
                for (var x=0;x<BUFFER_WIDTH/(numWorkers+1);x++) {
                    outputBitmapData.setPixel(x+(i*BUFFER_WIDTH/(numWorkers+1)),y,results[i][x+y*BUFFER_WIDTH/(numWorkers+1)]);
                }
            }
        }
        outputBitmapData.unlock();
        
        // compute FPS
        frames++;
        var now:Number = getTimer();
        if (now - lastTime > 1000)
        {
            var fps:Number = frames/((now - t) / 1000.0);
            frameTimeTxt.text = "Workers: "+numWorkers+" , FPS: " + int(fps);
            lastTime = now;
        }
        }
        
    }
    public function updatePixels(startx:uint,endx:uint,sphereCenterX:Vector.<Number>,sphereCenterY:Vector.<Number>,sphereCenterZ:Vector.<Number>):Vector.<uint> {
        var results:Vector.<uint>=new Vector.<uint>();
        // reused variables
        var x:int;
        var y:int;
        var i:int;
        var j:int;
        
        var r:int;
        var g:int;
        var b:int;
        
        var dx:Number;
        var dy:Number;
        
        var rayDirX:Number;
        var rayDirY:Number;
        var rayDirZ:Number;
        var rayDirMag:Number;
        
        var reflectRayDirX:Number;
        var reflectRayDirY:Number;
        var reflectRayDirZ:Number;
        
        var intersectionX:Number;
        var intersectionY:Number;
        var intersectionZ:Number;
        
        var reflectIntersectionX:Number;
        var reflectIntersectionY:Number;
        var reflectIntersectionZ:Number;
        
        var rayToSphereCenterX:Number;
        var rayToSphereCenterY:Number;
        var rayToSphereCenterZ:Number;
        
        var lengthRTSC2:Number;
        var closestApproach:Number;
        var halfCord2:Number;
        var dist:Number;
        
        var normalX:Number;
        var normalY:Number;
        var normalZ:Number;
        var normalMag:Number;
        
        var illumination:Number;
        var reflectIllumination:Number;
        
        var reflectR:Number;
        var reflectG:Number;
        var reflectB:Number;
        
        // setup light dir
        var lightDirX:Number = .3;
        var lightDirY:Number = -1;
        var lightDirZ:Number = -.5;
        var lightDirMag:Number = 1/Math.sqrt(lightDirX*lightDirX +lightDirY*lightDirY +lightDirZ*lightDirZ);
        lightDirX *= lightDirMag;
        lightDirY *= lightDirMag;
        lightDirZ *= lightDirMag;
        
        // vars used to in intersection tests
        var closestIntersectionDist:Number;
        var closestSphereIndex:int;
        var reflectClosestSphereIndex:int;
        
        
        // compute screen space bounding circles
        //canvas.graphics.clear();
        //canvas.graphics.lineStyle(1, 0xFF0000, .25);
        //for(i = 0; i < numSpheres; ++i)
        //{
        //  sphere2dX[i] = (BUFFER_WIDTH / 2 + FOV * sphereCenterX[i] / sphereCenterZ[i]); 
        //  sphere2dY[i] = (BUFFER_HEIGHT /2 + FOV * sphereCenterY[i] / sphereCenterZ[i]); 
        //  sphere2dR[i] = (4 * FOV * sphereRadius[i] / sphereCenterZ[i]);
        //  canvas.graphics.drawCircle(sphere2dX[i]*BUFFER_SCALEDDOWN, sphere2dY[i]*BUFFER_SCALEDDOWN, sphere2dR[i]*BUFFER_SCALEDDOWN);
        //  sphere2dR[i] *= sphere2dR[i]; // store the squared value
        //}
        
        // write to each pixel
        for(y = 0; y < BUFFER_HEIGHT; ++y)
        {
            for(x = startx; x < endx; ++x)
            {
                // compute ray direction
                rayDirX = x - HALF_BUFFER_WIDTH;
                rayDirY = y - HALF_BUFFER_HEIGHT;
                rayDirZ = FOV;
                
                rayDirMag = 1/Math.sqrt(rayDirX * rayDirX + rayDirY * rayDirY +rayDirZ * rayDirZ);
                rayDirX *= rayDirMag;
                rayDirY *= rayDirMag;
                rayDirZ *= rayDirMag;
                
                /// trace the primary ray ///
                closestIntersectionDist = Number.POSITIVE_INFINITY;
                closestSphereIndex = -1
                for(i = 0; i < numSpheres; ++i)
                {
                    // check against screen space bounding circle
                    //dx = x - sphere2dX[i];
                    //dy = y - sphere2dY[i];
                    //if( dx * dx + dy * dy > sphere2dR[i] ) continue;
                    
                    // begin actual ray tracing if its inside the bounding circle
                    
                    lengthRTSC2 =       sphereCenterX[i] * sphereCenterX[i] +
                                        sphereCenterY[i] * sphereCenterY[i] +
                                        sphereCenterZ[i] * sphereCenterZ[i];
                                                
                    closestApproach =   sphereCenterX[i] * rayDirX +
                                        sphereCenterY[i] * rayDirY +
                                        sphereCenterZ[i] * rayDirZ;
                                        
                    if( closestApproach < 0 ) // intersection behind the origin
                        continue;
                        
                    halfCord2 = sphereRadius[i] * sphereRadius[i] - lengthRTSC2 + (closestApproach * closestApproach);
                    if( halfCord2 < 0 ) // ray misses the sphere
                        continue;
                        
                    // ray hits the sphere
                    dist = closestApproach - Math.sqrt(halfCord2);
                    if( dist < closestIntersectionDist )
                    {
                        closestIntersectionDist = dist;
                        closestSphereIndex=i;
                    }
                }
                /// end of trace primary ray ///

                // primary ray doesn't hit anything
                if( closestSphereIndex == - 1)
                {
//                    trace(""+x+" "+startx+" "+y);
                    results[(x-startx)+(y*(endx-startx))]=skyColor;
                }
                else // primary ray hits a sphere.. calculate shading, shadow and reflection
                {
                    // location of ray-sphere intersection
                    intersectionX = rayDirX * closestIntersectionDist;
                    intersectionY = rayDirY * closestIntersectionDist;
                    intersectionZ = rayDirZ * closestIntersectionDist;
                
                    // sphere normal at intersection point
                    normalX = intersectionX - sphereCenterX[closestSphereIndex];
                    normalY = intersectionY - sphereCenterY[closestSphereIndex];
                    normalZ = intersectionZ - sphereCenterZ[closestSphereIndex];
                    normalX /= sphereRadius[closestSphereIndex]; // could be multiply by precacluated 1/rad
                    normalY /= sphereRadius[closestSphereIndex];
                    normalZ /= sphereRadius[closestSphereIndex];
                    
                    // diffuse illumination coef
                    illumination =  normalX * lightDirX + 
                                    normalY * lightDirY + 
                                    normalZ * lightDirZ;
                                    
                    if( illumination < ambientIllumination ) 
                        illumination = ambientIllumination;
                    
                        
                    
                    /// trace a shadow ray ///
                    var isInShadow:Boolean = false;
                    for(j = 0; j < numSpheres; ++j)
                    {
                        if( j == closestSphereIndex ) continue;
                        
                        rayToSphereCenterX = sphereCenterX[j] - intersectionX;
                        rayToSphereCenterY = sphereCenterY[j] - intersectionY;
                        rayToSphereCenterZ = sphereCenterZ[j] - intersectionZ;
                        
                        lengthRTSC2 =       rayToSphereCenterX * rayToSphereCenterX + 
                                            rayToSphereCenterY * rayToSphereCenterY +
                                            rayToSphereCenterZ * rayToSphereCenterZ;
                        
                        
                        closestApproach =   rayToSphereCenterX * lightDirX +
                                            rayToSphereCenterY * lightDirY +
                                            rayToSphereCenterZ * lightDirZ;
                        if( closestApproach < 0 ) // intersection behind the origin
                            continue;
                            
                        
                        halfCord2 = sphereRadius[j] * sphereRadius[j] - lengthRTSC2 + (closestApproach * closestApproach);
                        if( halfCord2 < 0 ) // ray misses the sphere
                            continue;
                            
                        isInShadow = true; 
                        break;
        
                    }
                    
                    /// end of shadow ray ///
                    
                    if( isInShadow ) illumination *= .5;
                    
                    /// trace reflected ray ///
                    if( sphereReflects[closestSphereIndex] )
                    {
                        // calculate reflected ray direction
                        var reflectCoef:Number = 2 * (rayDirX * normalX + rayDirY * normalY + rayDirZ * normalZ);
                        reflectRayDirX = rayDirX - normalX * reflectCoef;
                        reflectRayDirY = rayDirY - normalY * reflectCoef;
                        reflectRayDirZ = rayDirZ - normalZ * reflectCoef;
                        
                        
                        closestIntersectionDist = Number.POSITIVE_INFINITY;
                        reflectClosestSphereIndex = -1
                        for(j = 0; j < numSpheres; ++j)
                        {
                            if( j == closestSphereIndex ) continue;
                            
                            rayToSphereCenterX = sphereCenterX[j] - intersectionX;
                            rayToSphereCenterY = sphereCenterY[j] - intersectionY;
                            rayToSphereCenterZ = sphereCenterZ[j] - intersectionZ;
                            
                            lengthRTSC2 =       rayToSphereCenterX * rayToSphereCenterX + 
                                                rayToSphereCenterY * rayToSphereCenterY + 
                                                rayToSphereCenterZ * rayToSphereCenterZ;
                                                        
                            closestApproach =   rayToSphereCenterX * reflectRayDirX + 
                                                rayToSphereCenterY * reflectRayDirY + 
                                                rayToSphereCenterZ * reflectRayDirZ;
                            
                            if( closestApproach < 0 ) // intersection behind the origin
                                continue;
                                
                            halfCord2 = sphereRadius[j] * sphereRadius[j] - lengthRTSC2 + (closestApproach * closestApproach);
                            if( halfCord2 < 0 ) // ray misses the sphere
                                continue;
                                
                            // ray hits the sphere
                            dist = closestApproach - Math.sqrt(halfCord2);
                            if( dist < closestIntersectionDist )
                            {
                                closestIntersectionDist = dist;
                                reflectClosestSphereIndex=j;
                            }
                        } // end loop through spheres for reflect ray
                        
                        
                        if( reflectClosestSphereIndex == - 1) // reflected ray misses
                        {
                            r = .5 * sphereR[closestSphereIndex] * illumination;
                            g = .5 * sphereG[closestSphereIndex] * illumination;
                            b = .5 * sphereB[closestSphereIndex] * illumination;
                        
                        }
                        else
                        {
                            //trace("ref hit");
                            // location of ray-sphere intersection
                            reflectIntersectionX = reflectRayDirX * closestIntersectionDist + intersectionX;
                            reflectIntersectionY = reflectRayDirY * closestIntersectionDist + intersectionY;
                            reflectIntersectionZ = reflectRayDirZ * closestIntersectionDist + intersectionZ;
                        
                            // sphere normal at intersection point
                            normalX = reflectIntersectionX - sphereCenterX[reflectClosestSphereIndex];
                            normalY = reflectIntersectionY - sphereCenterY[reflectClosestSphereIndex];
                            normalZ = reflectIntersectionZ - sphereCenterZ[reflectClosestSphereIndex];
                            
                            normalX /= sphereRadius[reflectClosestSphereIndex]; // could be multiply by precacluated 1/rad
                            normalY /= sphereRadius[reflectClosestSphereIndex];
                            normalZ /= sphereRadius[reflectClosestSphereIndex];
                            
                            // diffuse illumination coef
                            reflectIllumination =   normalX * lightDirX + 
                                                    normalY * lightDirY + 
                                                    normalZ * lightDirZ;
                                                
                            
                            if( reflectIllumination < ambientIllumination ) 
                                reflectIllumination = ambientIllumination;
                                                    
                            r = .5 * sphereR[closestSphereIndex] * illumination + .5 * sphereR[reflectClosestSphereIndex] * reflectIllumination;
                            g = .5 * sphereG[closestSphereIndex] * illumination + .5 * sphereG[reflectClosestSphereIndex] * reflectIllumination;
                            b = .5 * sphereB[closestSphereIndex] * illumination + .5 * sphereB[reflectClosestSphereIndex] * reflectIllumination;
                            if( r > 255 ) r = 255;
                            if( g > 255 ) g = 255;
                            if( b > 255 ) b = 255;
                            
                        }  // end if reflected ray hits
                        
                        
                        
                    } /// end if reflects
                    else // primary ray doesn't reflect
                    {
                        r = sphereR[closestSphereIndex] * illumination;
                        g = sphereG[closestSphereIndex] * illumination;
                        b = sphereB[closestSphereIndex] * illumination;
                    }

//                    trace(""+x+" "+y+" "+((x-startx)+(y*BUFFER_HEIGHT))+" = " + ((r<<16) + (g<<8) + b));
                    results[(x-startx)+(y*(endx-startx))]=(r<<16) + (g<<8) + b;
                    
                } // end if primary ray hit
            } // end x loop
        } // end y loop
        return results;
    }
}

if (Worker.current.isPrimordial()==false) {
    var r:RayTracer=new RayTracer();
    public function updatePixels(startx:uint,starty:uint,sphereX:Vector.<Number>,sphereY:Vector.<Number>,sphereZ:Vector.<Number>):Vector.<uint> {
        return r.updatePixels(startx,starty,sphereX,sphereY,sphereZ);
    }

}

}