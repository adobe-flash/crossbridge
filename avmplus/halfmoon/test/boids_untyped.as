// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package {
	import avmplus.System

    var start = new Date();
    var b = new boids(100);

    b.width=5000;
    b.height=5000;
    var frames = 300;
	if (System.argv.length > 0) {
		frames = System.argv[0]
		print("simulating " + frames + " frames")
	}
    b.initialize();

    for (var i = 0;i<frames;i++) {
        b.move_boids();
    }

    print("metric time " + (new Date()-start));
	print("frames " + frames)

    public class boids {
        var width = 500;
        var height = 500;
        var maxspeed = 50;
        var n;
        var boids_objs;
        var generation = 0;
        var lastmemory = 0;
        var maxmemory = 0;
        var avgtime = 0;
        var repel = 1;

        function boids(n) {
            this.n=n;
        }

        public function initialize(){
            boids_objs=new Array();
            for (var i =0;i<n;i++) {
                var b=new boid(i);
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

        public function rule1(b){
            var c = new vec(0,0);
            for (var i =0;i<boids_objs.length;i++) {
                if (boids_objs[i].equals(b)==false) {
                    c=c.add(boids_objs[i].position);
                }
            }
            c=c.div(boids_objs.length-1);
            return c.sub(b.position).div(100).mult(repel);
        }

        public function rule2(b) {
            var c = new vec(0,0);
            for (var i = 0;i<boids_objs.length;i++) {
                if (boids_objs[i].equals(b)==false) {
                    if (boids_objs[i].position.distance(b.position)<100) {
                        c=c.sub(boids_objs[i].position.sub(b.position));
                    }
                }
            }
            return c;
        }

        public function rule3(b) {
            var v = new vec(0,0);
            for (var i:uint=0;i<boids_objs.length;i++) {
                if (boids_objs[i].equals(b)==false) {
                    v=v.add(boids_objs[i].velocity);
                }
            }
            v=v.div(boids_objs.length-1);
            return v.div(8);
        }

        public function bound_position(b) {
            if (b.position.x<0) b.velocity.x=b.velocity.x+maxspeed/5;
            if (b.position.x>width) b.velocity.x=b.velocity.x-maxspeed/5;
            if (b.position.y<0) b.velocity.y=b.velocity.y+maxspeed/5;
            if (b.position.y>height) b.velocity.y=b.velocity.y-maxspeed/5;
            return new vec(0,0);
        }

        public function checkspeed(v) {
            if (v.x>maxspeed) v.x=maxspeed;
            if (v.x<-maxspeed) v.x=-1*maxspeed;
            if (v.y>maxspeed) v.y=maxspeed;
            if (v.y<-maxspeed) v.y=-1*maxspeed;
        }

        public function move_boids() {
            var time = new Date();
            for (var i =0; i<boids_objs.length;i++) {
                checkspeed(boids_objs[i].velocity);
                var v1 = rule1(boids_objs[i]);
                var v2 = rule2(boids_objs[i]);
                var v3 = rule3(boids_objs[i]);
                boids_objs[i].velocity=boids_objs[i].velocity.add(v1).add(v2).add(v3);
                bound_position(boids_objs[i]);
                boids_objs[i].position=boids_objs[i].position.add(boids_objs[i].velocity);
            }
            time=new Date()-time;
            avgtime=(avgtime*generation+time)/(generation+1);
            generation++;
        }

        public function updateMemory() {
            lastmemory=System.totalMemory;
            maxmemory=Math.max(lastmemory,maxmemory);
        }
    }

    public class boid {
        var position;
        var velocity;
        var number;

        function boid(number) {
            position=new vec(0,0);
            velocity=new vec(0,0);
            this.number=number;
        }

        public function equals(b) {
            return b.position.equals(position) && b.velocity.equals(velocity);
        }

        public function toString() {
            return "position:"+position.toString()+" velocity:"+velocity.toString();
        }
    }

    public class vec {
        var x;
        var y;

        function vec(x,y) {
            this.x=x;
            this.y=y;
        }

        public function toString() {
            return "x:"+x+" y:"+y;
        }

        public function add(v) {
            return new vec(x+v.x,y+v.y);
        }

        public function sub(v) {
            return new vec(x-v.x,y-v.y);
        }

        public function mult(n) {
            return new vec(x*n,y*n);
        }

        public function div(n) {
            return new vec(x/n,y/n);
        }

        public function copy() {
            return new vec(x,y);
        }

        public function equals(v) {
            return (v.x==x && v.y==y);
        }

        public function distance(v) {
            return Math.sqrt((v.x-x)*(v.x-x)+(v.y-y)*(v.y-y));
        }
    }

}
