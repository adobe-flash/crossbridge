var ITERS:int = 100;

    var LENGTH:Number = 50e-10;

    var m:Number = 4.0026;

    var mu:Number = 1.66056e-27;

    var kb:Number = 1.38066e-23;
 
    var TSIM:Number = 50;

    var deltat:Number = 5e-16;


    var epot:Number = 0.0;

    var vir:Number = 0.0;

    var count:Number = 0.0;

    var size:int;

    var datasizes:Vector.<int> = new Vector.<int>(4);
    datasizes[0] = 6;
    datasizes[1] = 8;
    datasizes[2] = 13;
    datasizes[3] = 3;

    var interactions:int = 0;

    var i:int, j:int, k:int, lg:int, mdsize:int, move:int, mm:int;

    var l:Number, rcoff:Number, rcoffs:Number, side:Number, sideh:Number, hsq:Number, hsq2:Number, vel:Number;

    var a:Number, r:Number, sum:Number, tscale:Number, sc:Number, ekin:Number, ek:Number, ts:Number, sp:Number;

    var den:Number = 0.83134;

    var tref:Number = 0.722;

    var h:Number = 0.064;

    var vaver:Number, vaverh:Number, rand:Number;

    var etot:Number, temp:Number, pres:Number, rp:Number;

    var u1:Number, u2:Number, v1:Number, v2:Number, s:Number;

    var ijk:int, npartm:int, PARTSIZE:int, iseed:int, tint:int;

    var irep:int = 10;

    var istop:int = 19;

    var iprint:int = 10;

    var movemx:int = 50;
   
    var one:Vector.<particle> = createParticleArray(10);
   
    var randnum:random = new random;

    function JGFsetsize(sizeI):void {
        size = sizeI;
    }

    function JGFinitialise():void {

        initialise();

    }

    function JGFapplication():void {

        print("Entering JFGapplication:Section3:MolDyn:Run");

        runiters();

    }

    function JGFvalidate():void {
        var refval:Vector.<Number> = new Vector.<Number>(4);
          refval[0] = 1731.4306625334357;
          refval[1] = 7397.392307839352;
          refval[2] = 7397.392307839352;
          refval[3] = 7397.392307839352;
        var dev:Number = Math.abs(ek - refval[size]);
        if (dev > 1.0e-12) {
            print("Validation failed");
            print("Kinetic Energy = " + ek + "  " + dev + "  "
                    + size);
        }
    }

    function JGFtidyup():void {
        print("tidyup");

        one = null;
    }

    function JGFrun(size):void {

        JGFsetsize(size);

        JGFinitialise();
        JGFapplication();
        JGFvalidate();
        JGFtidyup();
        print("THE END");
    }

    function initialise():void {

        /* Parameter determination */
                print("inside initialise");
        mm = datasizes[size];
        PARTSIZE = mm * mm * mm * 4;
        mdsize = PARTSIZE;
        print("MDSIZE: "  + mdsize);
        one = createParticleArray(mdsize);
        l = LENGTH;

        side = Math.pow((mdsize / den), 0.3333333);
        rcoff = mm / 4.0;

        a = side / mm;
        sideh = side * 0.5;
        hsq = h * h;
        hsq2 = hsq * 0.5;
        npartm = mdsize - 1;
        rcoffs = rcoff * rcoff;
        tscale = 16.0 / (1.0 * mdsize - 1.0);
        vaver = 1.13 * Math.sqrt(tref / 24.0);
        vaverh = vaver * h;
        
        /* Particle Generation */
                print("particle generation");
        ijk = 0;
        var loopLimit = 1;
        for (lg = 0; lg <= loopLimit; lg++) {
            for (i = 0; i < mm; i++) {
                for (j = 0; j < mm; j++) {
                    for (k = 0; k < mm; k++) {
                        one[ijk].setValues((i * a + lg * a * 0.5),
                                (j * a + lg * a * 0.5), (k * a), 0.0, 0.0, 0.0,
                                0.0, 0.0, 0.0);
                        ijk = ijk + 1;
                    }
                }
            }
        }
        loopLimit = 2;
        for (lg = 1; lg <= loopLimit; lg++) {
            for (i = 0; i < mm; i++) {
                for (j = 0; j < mm; j++) {
                    for (k = 0; k < mm; k++) {
                        one[ijk].setValues((i * a + (2 - lg) * a * 0.5),
                                (j * a + (lg - 1) * a * 0.5),
                                (k * a + a * 0.5), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
                        ijk = ijk + 1;
                    }
                }
            }
        }

        /* Initialise velocities */
                print("initialize velocities");
        iseed = 0;
        v1 = 0.0;
        v2 = 0.0;

        randnum = new random;
        randnum.random(iseed, v1, v2);

        for (i = 0; i < mdsize; i += 2) {
            r = randnum.seed();
            one[i].xvelocity = r * randnum.v1;
            one[i + 1].xvelocity = r * randnum.v2;
        }

        for (i = 0; i < mdsize; i += 2) {
            r = randnum.seed();
            one[i].yvelocity = r * randnum.v1;
            one[i + 1].yvelocity = r * randnum.v2;
        }

        for (i = 0; i < mdsize; i += 2) {
            r = randnum.seed();
            one[i].zvelocity = r * randnum.v1;
            one[i + 1].zvelocity = r * randnum.v2;
        }

        /* velocity scaling */

        ekin = 0.0;
        sp = 0.0;

        for (i = 0; i < mdsize; i++) {
            sp = sp + one[i].xvelocity;
        }
        sp = sp / mdsize;

        for (i = 0; i < mdsize; i++) {
            one[i].xvelocity = one[i].xvelocity - sp;
            ekin = ekin + one[i].xvelocity * one[i].xvelocity;
        }

        sp = 0.0;
        for (i = 0; i < mdsize; i++) {
            sp = sp + one[i].yvelocity;
        }
        sp = sp / mdsize;

        for (i = 0; i < mdsize; i++) {
            one[i].yvelocity = one[i].yvelocity - sp;
            ekin = ekin + one[i].yvelocity * one[i].yvelocity;
        }

        sp = 0.0;
        for (i = 0; i < mdsize; i++) {
            sp = sp + one[i].zvelocity;
        }
        sp = sp / mdsize;

        for (i = 0; i < mdsize; i++) {
            one[i].zvelocity = one[i].zvelocity - sp;
            ekin = ekin + one[i].zvelocity * one[i].zvelocity;
        }

        ts = tscale * ekin;
        sc = h * Math.sqrt(tref / ts);

        for (i = 0; i < mdsize; i++) {

            one[i].xvelocity = one[i].xvelocity * sc;
            one[i].yvelocity = one[i].yvelocity * sc;
            one[i].zvelocity = one[i].zvelocity * sc;

        }

        /* MD simulation */

    }

    function runiters():void {
        print("in run iterations");
        move = 0;
        for (move = 0; move < movemx; move++) {

            for (i = 0; i < mdsize; i++) {
                one[i].domove(side); /*
                                         * move the particles and update
                                         * velocities
                                         */
            }

            epot = 0.0;
            vir = 0.0;
                        one[0].force(0,1,2,3);
            for (i = 0; i < mdsize; i++) {
                //print("i: " + i + ", side: " + side + ", rcoff: " + rcoff + ", mdsize: " + mdsize);
                one[i].force(side, rcoff, mdsize, i); /* compute forces */
            }

            sum = 0.0;
            for (i = 0; i < mdsize; i++) {
                //print(i);
                sum = sum + one[i].mkekin(hsq2); /*
                                                     * scale forces, update
                                                     * velocities
                                                     */
            }

            ekin = sum / hsq;

            vel = 0.0;
            count = 0.0;
            for (i = 0; i < mdsize; i++) {
                vel = vel + one[i].velavg(vaverh, h); /* average velocity */
            }

            vel = vel / h;

            /* tmeperature scale if required */
            if ((move < istop) && (((move + 1) % irep) == 0)) {
                sc = Math.sqrt (tref / (tscale * ekin));
                for (i = 0; i < mdsize; i++) {
                    one[i].dscal(sc, 1);
                }
                ekin = tref / tscale;
            }

            /* sum to get full potential energy and virial */

            if (((move + 1) % iprint) == 0) {
                ek = 24.0 * ekin;
                epot = 4.0 * epot;
                etot = ek + epot;
                temp = tscale * ekin;
                pres = den * 16.0 * (ekin - vir) / mdsize;
                vel = vel / mdsize;
                rp = (count / mdsize) * 100.0;
            }

        }

    }
   
    function createParticleArray(sizeI:int):Vector.<particle>{
        var x = new Vector.<particle>(sizeI+1);
        var i:int=0;
        for (i=0; i<sizeI; i++){
            x[i] = new particle;
        }
        print("Array length: " + x.length);
        return x;
    }
   
class particle {

        var xcoord:Number, ycoord:Number, zcoord:Number;

        var xvelocity:Number, yvelocity:Number, zvelocity:Number;

        var xforce:Number, yforce:Number, zforce:Number;
        
        function particle():void{
        }

       prototype.setValues = function(xcoordI:Number, ycoordI:Number, zcoordI:Number,
                xvelocityI:Number, yvelocityI:Number, zvelocityI:Number,
                xforceI:Number, yforceI:Number, zforceI:Number):void {
            xcoord = xcoordI;
            ycoord = ycoordI;
            zcoord = zcoordI;
            xvelocity = xvelocityI;
            yvelocity = yvelocityI;
            zvelocity = zvelocityI;
            xforce = xforceI;
            yforce = yforceI;
            zforce = zforceI;

        }

        prototype.domove = function(sideI:Number) {

            xcoord = xcoord + xvelocity + xforce;
            ycoord = ycoord + yvelocity + yforce;
            zcoord = zcoord + zvelocity + zforce;

            if (xcoord < 0) {
                xcoord = xcoord + sideI;
            }
            if (xcoord > sideI) {
                xcoord = xcoord - sideI;
            }
            if (ycoord < 0) {
                ycoord = ycoord + sideI;
            }
            if (ycoord > sideI) {
                ycoord = ycoord - sideI;
            }
            if (zcoord < 0) {
                zcoord = zcoord + sideI;
            }
            if (zcoord > sideI) {
                zcoord = zcoord - sideI;
            }

            xvelocity = xvelocity + xforce;
            yvelocity = yvelocity + yforce;
            zvelocity = zvelocity + zforce;

            xforce = 0.0;
            yforce = 0.0;
            zforce = 0.0;

        }

        prototype.force = function(side, rcoff, mdsize, x):void {

            var sideh:Number;
            var rcoffs:Number;

            var xx:Number, yy:Number, zz:Number, xi:Number, yi:Number, zi:Number
            var fxi:Number, fyi:Number, fzi:Number;
            var rd:Number, rrd:Number, rrd2:Number, rrd3:Number, rrd4:Number
            var rrd6:Number, rrd7:Number, r148:Number;
            var forcex:Number, forcey:Number, forcez:Number;

            var i:int;
            sideh = 0.5 * side;
            rcoffs = rcoff * rcoff;

            xi = xcoord;
            yi = ycoord;
            zi = zcoord;
            fxi = 0.0;
            fyi = 0.0;
            fzi = 0.0;

            for (i = x + 1; i < mdsize; i++) {
                xx = xi - one[i].xcoord;
                yy = yi - one[i].ycoord;
                zz = zi - one[i].zcoord;
                if (xx < (-sideh)) {
                    xx = xx + side;
                }
                if (xx > (sideh)) {
                    xx = xx - side;
                }
                if (yy < (-sideh)) {
                    yy = yy + side;
                }
                if (yy > (sideh)) {
                    yy = yy - side;
                }
                if (zz < (-sideh)) {
                    zz = zz + side;
                }
                if (zz > (sideh)) {
                    zz = zz - side;
                }

                rd = xx * xx + yy * yy + zz * zz;

                if (rd <= rcoffs) {
                    rrd = 1.0 / rd;
                    rrd2 = rrd * rrd;
                    rrd3 = rrd2 * rrd;
                    rrd4 = rrd2 * rrd2;
                    rrd6 = rrd2 * rrd4;
                    rrd7 = rrd6 * rrd;
                    epot = epot + (rrd6 - rrd3);
                    r148 = rrd7 - 0.5 * rrd4;
                    vir = vir - rd * r148;
                    forcex = xx * r148;
                    fxi = fxi + forcex;
                    one[i].xforce = one[i].xforce - forcex;
                    forcey = yy * r148;
                    fyi = fyi + forcey;
                    one[i].yforce = one[i].yforce - forcey;
                    forcez = zz * r148;
                    fzi = fzi + forcez;
                    one[i].zforce = one[i].zforce - forcez;
                    interactions++;
                }

            }
            xforce = xforce + fxi;
            yforce = yforce + fyi;
            zforce = zforce + fzi;

        }

       prototype.mkekin = function(hsq2:Number):Number {

            var sumt:Number = 0.0;

            xforce = xforce * hsq2;
            yforce = yforce * hsq2;
            zforce = zforce * hsq2;

            xvelocity = xvelocity + xforce;
            yvelocity = yvelocity + yforce;
            zvelocity = zvelocity + zforce;

            sumt = (xvelocity * xvelocity) + (yvelocity * yvelocity)
                    + (zvelocity * zvelocity);
            return sumt;
        }

       prototype.velavg = function(vaverh:Number, h:Number):Number {

            var velt:Number;
            var sq:Number;

            sq = Math.sqrt(xvelocity * xvelocity + yvelocity * yvelocity
                    + zvelocity * zvelocity);

            if (sq > vaverh) {
                count = count + 1.0;
            }

            velt = sq;
            return velt;
        }

       prototype.dscal = function(sc:Number, incx:int):void {

            xvelocity = xvelocity * sc;
            yvelocity = yvelocity * sc;
            zvelocity = zvelocity * sc;
        }
        }

    class random {

        var iseed:int;

        var v1:Number, v2:Number;
       
        prototype.random = function(iseedI:int, v1I:Number, v2I:Number):void {
            iseed = iseedI;
            v1 = v1I;
            v2 = v2I;
        }

       prototype.update = function():Number {

            var rand:Number;
            var scale:Number = 4.656612875e-10;

            var is1:int, is2:int, iss2:int;
            var imult:int = 16807;
            var imod:int = 2147483647;

            if (iseed <= 0) {
                iseed = 1;
            }

            is2 = iseed % 32768;
            is1 = (iseed - is2) / 32768;
            iss2 = is2 * imult;
            is2 = iss2 % 32768;
            is1 = (is1 * imult + (iss2 - is2) / 32768) % (65536);

            iseed = (is1 * 32768 + is2) % imod;

            rand = scale * iseed;

            return rand;

        }

        prototype.seed = function():Number {

            var s:Number, u1:Number, u2:Number, r:Number;
            s = 1.0;
            do {
                u1 = this.update();
                u2 = this.update();

                v1 = 2.0 * u1 - 1.0;
                v2 = 2.0 * u2 - 1.0;
                s = v1 * v1 + v2 * v2;

            } while (s >= 1.0);

            r = Math.sqrt(-2.0 * Math.log(s) / s);

            return r;

        }
        }
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

