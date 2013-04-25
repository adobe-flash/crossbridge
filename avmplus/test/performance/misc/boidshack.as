/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

 // --------------------------------------------------------
 //   Name: Headless Boids
 //   Desc: Version of Boids that has no graphic output.  Used for testing the avmplus.
 // --------------------------------------------------------


//
// Vector.as
//

var updateCount=0

class Vector1
{
    function Vector1()
    {
       this.x = 0;
       this.y = 0;
    }

    function copyVector( v:Vector1 ):void
    {
        this.x = v.x;
        this.y = v.y;
    }

    function setMembers( x:Number, y:Number ):void
    {
    
        this.x = x;
        this.y = y;
    }

    function addVector( v:Vector1 ):void
    {
        this.x += v.x;
        this.y += v.y;
    }

    function subVector( v:Vector1 ):void
    {
        this.x -= v.x;
        this.y -= v.y;
    }

    function divScalar( i:Number ):void
    {
        this.x /= i;
        this.y /= i;
    }

    function mulScalar( i:Number ):void
    {
        this.x *= i;
        this.y *= i;
    }

    function magnitude():Number
    {
        return Math.sqrt( this.x*this.x + this.y*this.y );
    }

    function magnitude2():Number
    {
        return this.x*this.x + this.y*this.y;
    }

    var x:Number;
    var y:Number;
};

dynamic class MovieClip
{
        var _xscale:Number
        var _yscale:Number
        var _width:Number
        var _height:Number
        var _x:Number
        var _y:Number
}

//
// Entity.as
//

// HACK no intrinsic definition for MovieClip
class Entity extends MovieClip
{
        var _rotation:Number;
        var _alpha:uint;
        var _root:MovieClip;
        
    function Entity()
    {
        position = new Vector1();
        velocity = new Vector1();
        direction = new Vector1();
        tmp = new Vector1();
    }

    function move( elapsed:Number ):void
    {
        tmp.copyVector( velocity );
        
        tmp.mulScalar( elapsed );
   
        position.addVector( tmp );

        _x = position.x;
        _y = position.y;

        var mag:Number = velocity.magnitude();

        direction.x = velocity.x;
        direction.y = velocity.y;
        direction.divScalar( mag );

//        if ( mag < 5 )
//            continue;

        var angle = Math.atan2( direction.y, direction.x );

        // account for the fact that the fish natively points
        // at -pi/2

        angle += (Math.PI/2);

        var degree = (angle / Math.PI) * 180;

        _rotation = degree;
    }

    static function rnd( min:Number, max:Number ):Number
    {
        var r:Number = Math.random();

        // Flash 5R30 has a bug where it will occasionally
        // generate a 1.0.  Oops.

        while (r == 1)
        {
            r = Math.random();
        }
        return min + Math.floor(r * (max + 1 - min));
    }


    var position:Vector1;
    var velocity:Vector1;
    var direction:Vector1;
    var tmp:Vector1;

    private var frame:Number;
    var lastTime:Number;
    var id:Number;
};

//
// Food.as
//

class Food extends Entity
{
    function Food()
    {
        lastTime = new Date();
    }

    function initialize( boids:Boids, id:Number, x:Number, y:Number )
    {
        this.boids = boids;
        this.id = id;

        reset( x, y );
    }

    function reset( x:Number, y:Number )
    {
        next = null;
        prev = null;
        
        startTime = new Date();
        lastTime = startTime;
        position.setMembers( x, y );
        velocity.x = rnd( -5, 5 );
        velocity.y = -5 * rnd( -5, -10 );

        alive = true;
        _alpha = 100;
    }
    
    
    function update( elapsed:Number )
    {
        if (alive == false)
            return;
        
        var lifetime:Number = (new Date() - startTime) / 1000;

        velocity.x += rnd( -1, 1 );


        var newAlpha:Number = 100 - (lifetime * 10);

        if (position.y > 560)
        {
            die();
            return;
        }
        
        if (newAlpha < 5)
        {
            // put on recycle list
            die();
        }
        else
            _alpha = newAlpha;
    }

    function die()
    {
        _alpha = 0;

        if (next != null)
            next.prev = prev;
        if (prev != null)
            prev.next = next;
        prev = null;
        if (boids.liveFood == this)
            boids.liveFood = next;
            
        next = boids.deadFood;
        if (next)
            next.prev = this;
        boids.deadFood = this;

        alive = false;
    }
    

    function onEnterFrame():void
    {
        ++frame;

        var thisTime:Number = new Date();
        var elapsed:Number = (thisTime - lastTime) / 1000;
        lastTime = thisTime;

        if (alive)
        {
            update( elapsed );
            move( elapsed );
        }
    }

    var boids:Boids;
    var next:Food;
    var prev:Food;
    var alive:Boolean;
    var startTime:Number;
    var frame:Number;
};

//
// Fish.as
//

class Fish extends Entity
{
    function Fish()
    {
        chase = new Vector1();
        rule1 = new Vector1();
        rule2 = new Vector1();
        rule3 = new Vector1();
        visibility = [];
    }

    // simulate 1/24th since the benchmark doesn't use real frame ticks
    static var tick = 1;
    static function getTick()
    {
        return tick += 1000/24;
    }

    function onEnterFrame():void
    {
                ++frame;

        var thisTime:Number = getTick();
        var elapsed:Number = (thisTime - lastTime) / 1000;
        lastTime = thisTime;

        if (( ( id == 0 ) || ( (frame&3) == (id&3) ) ))
        {
            update();
        }

        move( elapsed );
    }

    function hungry():Boolean
    {
        return (((getTick() - lastAte) / 1000) > 10);
    }
            
    
    function initialize( boids:Boids, id:Number )
    {
        this.boids = boids;
        this.id = id;
        
        position.setMembers( rnd( 50, 650 ), -10 ); //rnd( 50, 450 ) );

        velocity.x = rnd( 1, 10 );
        velocity.y = rnd( 1, 10 );

        direction.x = velocity.x / velocity.magnitude();
        direction.y = velocity.y / velocity.magnitude();

        scale = Math.min( 0.6, Math.max( 0.3, Math.random() ) );
        _xscale *= scale;
        _yscale *= scale;

        lastTime = getTick();
        targetFood = null;
        lastAte = lastTime - rnd( 20, 30 );
    }

    function update()
    {
                updateCount++
        rule1.setMembers( 0, 0 );
        rule2.setMembers( 0, 0 );
        rule3.setMembers( 0, 0 );

        var visible:Number = 0;

        for ( var i = 0; i < boids.fishArray.length; i++ )
        {
            if ( i == id )
                continue;

                        // HACK self-ref type removed due to compiler bug
            var otherfish/*:Fish*/ = boids.fishArray[i];

            // Rule 1 + 3

            tmp.setMembers( otherfish.position.x,
                            otherfish.position.y );
            tmp.subVector( position );

            var influence2:Number = (boids.influence*boids.influence*100);

            if (boids.omniscient)
                visibility[i] = true;
            else
            {
                var dot:Number = ( direction.x * tmp.x + direction.y * tmp.y );
                visibility[i] = ( dot > 0 );
            }

            if (tmp.magnitude2() > influence2)
                visibility[i] = false;

            if (visibility[i])
            {
                visible++;
                rule1.addVector( otherfish.position );
                rule3.addVector( otherfish.velocity );
            }
      
            // Rule 2

//         if ( visibility[i] )
//         {
            tmp.copyVector( position );
            tmp.subVector( otherfish.position );

            var mag2:Number = tmp.magnitude2();

            var scale2:Number = scale*scale;

            if ( mag2 < ( scale2 * 100 ) )
            {
                rule2.addVector( tmp );
            }
            else if ( mag2 < ( scale2 * 10000 ) )
            {
                tmp.divScalar( 4 );
                rule2.addVector( tmp );
            }

//         }
        }
        if ( visible > 0 )

        {
            rule1.divScalar( visible );
            rule1.subVector( position );

            rule3.divScalar( visible );
            rule3.subVector( velocity );
        }

        rule1.mulScalar( boids.rule1weight );
        rule2.mulScalar( boids.rule2weight );
        rule3.mulScalar( boids.rule3weight );

        if (targetFood == null)
            velocity.addVector( rule1 );

        velocity.addVector( rule2 );

        if (targetFood == null)
            velocity.addVector( rule3 );

        if (boids.doChase && (id == 0))
        {
            chase.setMembers( _root._xmouse, _root._ymouse );
            chase.subVector( position );
            chase.mulScalar( 0.05 );
            velocity.addVector( chase );
        }
        

        if (hungry() && (targetFood == null))
        {
            var food:Food = boids.liveFood;
            var closest:Food = null;
            var dist2:Number = 250000;
            var toFood:Vector1 = new Vector1();
        
            while (food != null)
            {
                tmp.copyVector( food.position );
                tmp.subVector( position );

                if (!boids.omniscient)
                {
                    dot = ( direction.x * tmp.x + direction.y * tmp.y );
                    if (dot < 0)
                    {
                        food = food.next;
                        continue;
                    }
                }

                mag2 = tmp.magnitude2();


                if (mag2 < dist2)
                {
                    dist2 = mag2;
                    closest = food;
                    toFood = tmp;
                    targetFood = food;
                }
                food = food.next;
            }
        }
        
            
        if ((targetFood != null) && targetFood.alive)
        {
            tmp.copyVector( targetFood.position );
            tmp.subVector( position );
            mag2 = tmp.magnitude2();
            
            if (mag2 < 50)
            {
                lastAte = getTick();
                targetFood.die();
                trace("yum!");
                
            }
            else
            {
                tmp.mulScalar( 1.5 );
                velocity.addVector( tmp );
            }
        }
        else
        {
            targetFood = null;
        }
        
        

        // store half the clip's height and width in convenient variables
        var halfWidth:Number = _width / 2;
        var halfHeight:Number = _height / 2;

        // record the outer dimensions of the clip
        var fishLeftX:Number = _x - halfWidth;
        var fishRightX:Number = _x + halfWidth;
        var fishBottomY:Number = _y + halfHeight;
        var fishTopY:Number = _y - halfHeight;

        // check for horizontal bounce
        if (fishLeftX < leftBorder)
        {
            velocity.x += 40;
        }
        else if (fishRightX > rightBorder)
        {
            velocity.x -= 40;
        }

        // check for vertical bounce
        if (fishTopY < topBorder)
        {
            velocity.y += 40;
        }
        else if (fishBottomY > bottomBorder)
        {
            velocity.y -= 40;
        }

        var mag:Number = velocity.magnitude();
        var maxvel:Number = boids.maxVel * 0.8;
        if ( id != 0 )
            maxvel = scale * boids.maxVel;

        if (targetFood)
            maxvel *= 2;

        if ( mag > maxvel )
        {
            velocity.divScalar( mag );
            velocity.mulScalar( maxvel );
        }
    }

    var symbol:String;
    var boids:Boids;
    var scale:Number;
    var leftBorder:Number = 20;
    var rightBorder:Number = 780;
    var bottomBorder:Number = 540;
    var topBorder:Number = 20;

    var rule1:Vector1;
    var rule2:Vector1;
    var rule3:Vector1;
    var chase:Vector1;
    var visibility:Array;
    var frame:Number = 0;
    var targetFood:Food;
    var lastAte:Number;
};

//
// Boids.as
//


dynamic class Boids
{
    function Boids(numFish:Number)
    {
        this.numFish = numFish;
                initialize();
    }

    var numFish:Number;
    function initialize()
    {
        for ( var i:Number = 0; i < numFish; i++ )
        {
            addFish();
        }

        this.foodList = null;
    }
    function addFish()
    {
        var id:Number = fishArray.length;

        this["fish"+id] = new Fish();
        var fish = this["fish" + id];
        fish.initialize( this, id );
        fishArray.push( fish );
    }

    function setChase(e)
    {
        doChase = e.target.selected;
    }
    function setOmniscient(e)
    {
        omniscient = e.target.selected;
    }

    function setRule1(e)
    {
        rule1weight = e.target.value;
    }
    function setRule2(e)
    {
        rule2weight = e.target.value;
    }
    function setRule3(e)
    {
        rule3weight = e.target.value;
    }

    function setInfluence(e)
    {
        influence = e.target.value;
    }
    function setMaxVel(e)
    {
        maxVel = e.target.value;
    }

        function onEnterFrame()
        {
                trace("onEnterFrame  name");
        }

    var fishSymbol:String;
    var fishArray:Array = [];

    var foodSymbol:String;
    var foodArray:Array = [];//

    var bgSymbol:String;

// ### set up legal movement area
    var leftBorder:Number = 20;
    var rightBorder:Number = 780;
    var bottomBorder:Number = 580;
    var topBorder:Number = 20;

    var fish:Fish;
    var doChase:Boolean = false;
    var omniscient:Boolean = true;

    var rule1weight:Number = 0.02;
    var rule2weight:Number = 1.0;
    var rule3weight:Number = 0.125;
    var liveFood:Food = null;
    var deadFood:Food = null;
    var foodCounter:Number = 0;

    var maxVel:Number = 100;
    var influence:Number = 100;
};

//
// boidbench.as
//


var numFish = 500;
var boids = new Boids(numFish);

if (CONFIG::desktop){
    var frames = 20;
    var t:Number = new Date();
}
else {// mobile
    var frames = 5;
    var t:int = getTimer();
}

for (var i=0; i < frames; i++)
{
    for (var j=0; j < numFish; j++)
    {
        boids.fishArray[j].onEnterFrame();
    }
}


if (CONFIG::desktop)
    t = new Date() - t;
else // mobile
    t = getTimer() - t;

print("metric time "+t);

