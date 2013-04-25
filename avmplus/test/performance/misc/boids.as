/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {
  
    if (CONFIG::desktop) {
        var start:Number=new Date();
        var b:boids=new boids(100);
    }
    else { // mobile
        var start:int=getTimer();
        var b:boids=new boids(10);
    }

        b.width=5000;
        b.height=5000;
        var frames:uint=300;
        b.initialize();

        for (var i:int=0;i<frames;i++) {
                b.move_boids();
        }
    
    if (CONFIG::desktop)
        print("metric time " + (new Date()-start));
    else // mobile
        print("metric time " + (getTimer()-start));
        


        public class boids {
                var width:uint=500;
                var height:uint=500;
                var maxspeed:uint=50;
                var n:uint;
                var boids_objs:Array;
                var generation:uint=0;
                var lastmemory:uint=0;
                var maxmemory:uint=0;
                var avgtime:uint=0;
                var repel:int=1;

                function boids(n:uint) {
                        this.n=n;
                }
                
                public function initialize():void {
                        boids_objs=new Array();
                        for (var i:uint=0;i<n;i++) {
                                var b:boid=new boid(i);
                                switch (i%4) {
                                        case 0:
                                                b.position=new vec(width*(i/4)/(n/4),0);
                                                break;
                                        case 1:
                                                b.position=new vec(0,height*(i/4)/(n/4));
                                                break;
                                        case 2:
                                                b.position=new vec(width*(i/4)/(n/4),height);
                                                break;
                                        case 3:
                                                b.position=new vec(width,height*(i/4)/(n/4));
                                                break;
                                }
                                boids_objs[boids_objs.length]=b;
                        }
                }
                
                public function rule1(b:boid):vec {
                        var c:vec=new vec(0,0);
                        for (var i:uint=0;i<boids_objs.length;i++) {
                                if (boids_objs[i].equals(b)==false) {
                                        c=c.add(boids_objs[i].position);
                                }
                        }
                        c=c.div(boids_objs.length-1);
                        return c.sub(b.position).div(100).mult(repel);
                }
                
                public function rule2(b:boid):vec {
                        var c:vec=new vec(0,0);
                        for (var i:uint=0;i<boids_objs.length;i++) {
                                if (boids_objs[i].equals(b)==false) {
                                        if (boids_objs[i].position.distance(b.position)<100) {
                                                c=c.sub(boids_objs[i].position.sub(b.position));
                                        }
                                }
                        }
                        return c;
                }
                
                public function rule3(b:boid):vec {
                        var v:vec=new vec(0,0);
                        for (var i:uint=0;i<boids_objs.length;i++) {
                                if (boids_objs[i].equals(b)==false) {
                                        v=v.add(boids_objs[i].velocity);
                                }
                        }
                        v=v.div(boids_objs.length-1);
                        return v.div(8);
                }
                
                public function bound_position(b:boid):vec {
                        if (b.position.x<0) b.velocity.x=b.velocity.x+maxspeed/5;
                        if (b.position.x>width) b.velocity.x=b.velocity.x-maxspeed/5;
                        if (b.position.y<0) b.velocity.y=b.velocity.y+maxspeed/5;
                        if (b.position.y>height) b.velocity.y=b.velocity.y-maxspeed/5;
                        return new vec(0,0);
                }

                public function checkspeed(v:vec):void {
                        if (v.x>maxspeed) v.x=maxspeed;
                        if (v.x<-maxspeed) v.x=-1*maxspeed;
                        if (v.y>maxspeed) v.y=maxspeed;
                        if (v.y<-maxspeed) v.y=-1*maxspeed;
                }
                
                public function move_boids():void {
                        var time:Number=new Date();
                        for (var i:uint=0;i<boids_objs.length;i++) {
                                checkspeed(boids_objs[i].velocity);
                                var v1:vec=rule1(boids_objs[i]);
                                var v2:vec=rule2(boids_objs[i]);
                                var v3:vec=rule3(boids_objs[i]);
                                boids_objs[i].velocity=boids_objs[i].velocity.add(v1).add(v2).add(v3);
                                bound_position(boids_objs[i]);
                                boids_objs[i].position=boids_objs[i].position.add(boids_objs[i].velocity);
                        }
                        time=new Date()-time;
                        avgtime=(avgtime*generation+time)/(generation+1);
                        generation++;
                }
                
                public function updateMemory():void {
                        lastmemory=System.totalMemory;
                        maxmemory=Math.max(lastmemory,maxmemory);
                }
        }
  
        public class boid {
                var position:vec;
                var velocity:vec;
                var number:uint;

                function boid(number:uint) {
                        position=new vec(0,0);
                        velocity=new vec(0,0);
                        this.number=number;
                }

                public function equals(b:boid):Boolean {
                        return b.position.equals(position) && b.velocity.equals(velocity);
                }
                
                public function toString():String {
                        return "position:"+position.toString()+" velocity:"+velocity.toString();
                }
        }
  
        public class vec {
                var x:int;
                var y:int;

                function vec(x:uint,y:uint) {
                        this.x=x;
                        this.y=y;
                }

                public function toString():String {
                        return "x:"+x+" y:"+y;
                }
        
                public function add(v:vec):vec {
                        return new vec(x+v.x,y+v.y);
                }
                
                public function sub(v:vec):vec {
                        return new vec(x-v.x,y-v.y);
                }
                
                public function mult(n:Number):vec {
                        return new vec(x*n,y*n);
                }
                
                public function div(n:Number):vec {
                        return new vec(x/n,y/n);
                }
                
                public function copy():vec {
                        return new vec(x,y);
                }
                
                public function equals(v:vec):Boolean {
                        return (v.x==x && v.y==y);
                }
                
                public function distance(v:vec):Number {
                        return Math.sqrt((v.x-x)*(v.x-x)+(v.y-y)*(v.y-y));
                }
        }

}