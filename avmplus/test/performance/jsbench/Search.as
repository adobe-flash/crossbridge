var startSearchBench = new JGFSearchBench();

var start=new Date();
startSearchBench.JGFrun(0);
var elapsed=new Date()-start;
print("metric time "+elapsed);
        
class Game {
        static var TRANSIZE = 1050011;

        static var PROBES = 8;

        static var REPORTPLY = 8;

        static var UNK = -4;

        static var LOSE = -2;

        static  var DRAWLOSE = -1;

        static var DRAW = 0;

        static var DRAWWIN = 1;

        static var WIN = 2;

        static var EMPTY = 0;

        static var BLACK = 1;

        static var WHITE = 2;

        static var EDGE = 3;

        var startingMoves = new Array( "444333377", "44433337" );
        
        static var colthr;

        static var colwon;
        static var MIN_VALUE = -2147483648;

        var i;


        var numMoves=0;

        var moves, plycnt;

        var rows, dias;

        var columns, height;
        var counter =0;
        function printStuff(stuff)
        {
                var n=0;
                var k=5;
                if(counter==0)
                {
                        counter=1000*n;
                }
                if(counter>=1000*n&&counter<=1000*k)
                {
                        print(stuff);
                        counter++;
                }
        }
        function Game() {
                //print("calling Game()");
                moves = new Array(44);
                rows = new Array(8);
                dias = new Array(19);
                columns = new Array(8);
                height = new Array(8);
                        colthr = new Array(128);
                for(i=0;i<128;i++)
                {
                        colthr[i]=0;
                }
                for (i = 8; i < 128; i += 8) {
                        colthr[i] = 1;
                        colthr[i + 7] = 2;
                }
                colwon = new Array(128);
                for (i = 16; i < 128; i += 16)
                        colwon[i] = colwon[i + 15] = true;
                reset();
        }

        function reset() {
                plycnt = 0;
                for (var i = 0; i < 19; i++)
                        dias[i] = 0;
                for (var i = 0; i < 8; i++) {
                        columns[i] = 1;
                        height[i] = 1;
                        rows[i] = 0;
                }
        }

        //@Override
        function toString() {
                var buf = "";

                for (var i = 1; i <= plycnt; i++)
                        buf+=moves[i]+"";
                return buf;
        }

        // return whether a move by player side in column n at height h would win
        // non-vertically
        function wins(n, h, sidemask) {
                //printStuff("execuitng wins: n-"+n+":h-"+h+":sidemask-"+sidemask);
                var x, y;

                sidemask <<= (2 * n);
                x = rows[h] | sidemask;
                //printStuff("x1 is: "+x);
                y = x & (x << 2);
                if ((y & (y << 4)) != 0)
                        return true;
                x = dias[5 + n + h] | sidemask;
                //printStuff("x2 is: "+x);
                y = x & (x << 2);
                //printStuff("y2 is: "+y);
                var _temp=y & (y << 4);
                //printStuff("_temp is: "+_temp);
                if( _temp != 0)
                {
                        //printStuff("exit wins");
                        return true;
                }
                x = dias[5 + n - h] | sidemask;
                //printStuff("x3 is: "+x);
                y = x & (x << 2);
                return (y & (y << 4)) != 0;
        }
        var runNumber=0;
        function backmove() {
                //printStuff("executing backmove");
                runNumber++;
                var mask, d, h, n, side;
                d=0;
                side = plycnt & 1;
                n = moves[plycnt--];
                h = --height[n];
                //printStuff("backmove height["+n+"]: "+height[n]);
                columns[n] >>= 1;
                mask = ~(1 << (2 * n + side));
                rows[h] &= mask;
                dias[5 + n + h] &= mask;
                dias[5 + n - h] &= mask;
                //print("mask-"+mask+":plycnt-"+plycnt+":side-"+side+":runNumber-"+runNumber);
                //print("mask-"+mask+":d-"+d+":h-"+h+":n-"+n+":side-"+side+":columns[n]-"+columns[n]+":rows[h]-"+rows[h]+":dias[5+n+h]-"+dias[5+n+h]+":dias[5+n-h]-"+dias[5+n-h]);
        }

        function makemove(n) {
                //printStuff("executing move");
                numMoves++;
                //7321081
                if(numMoves%10000==0)
                {
                        //print("numMoves: "+numMoves);
                }
                var mask, d, h, side;

                moves[++plycnt] = n;
                side = plycnt & 1;
                h = height[n]++;
                //printStuff("makemove height["+n+"]: "+height[n]);
                columns[n] = (columns[n] << 1) + side;
                mask = 1 << (2 * n + side);
                rows[h] |= mask;
                dias[5 + n + h] |= mask;
                dias[5 + n - h] |= mask;
        }
}
class TransGame extends Game {
        function d2b(d) {return d.toString(2);}
        function b2d(h) {return parseInt(h,2);}
        function rightShift(num, ammount)
        {
                var hexNum = d2b(num);
                if(hexNum.length-ammount<=0)
                {
                        return 0;
                }
                var remBits = ammount%4;
                var numBytes = (ammount-remBits)/4;
                var shiftString="";
                for(var i=0;i<numBytes;i++)
                {
                        shiftString+="0000";
                }
                for(var i=0;i<remBits;i++)
                {
                        shiftString +="0";
                }
                return b2d(shiftString+hexNum.substr(0,hexNum.length-ammount));
                
        }
        function leftShift(num, ammount)
        {
                var hexNum = d2b(num);
                var remBits = ammount%4;
                var numBytes = (ammount-remBits)/4;
                var shiftString="";
                for(var i=0;i<numBytes;i++)
                {
                        shiftString+="0000";
                }
                for(var i=0;i<remBits;i++)
                {
                        shiftString +="0";
                }
                return b2d(hexNum+shiftString);
                
        }
        static var NSAMELOCK = 0x20000;

        static var STRIDERANGE = Math.floor((TRANSIZE / PROBES - NSAMELOCK));

         var INTMODSTRIDERANGE = (leftShift(1,32) % STRIDERANGE);

        static var ABSENT = -128;

        var ht; // hash locks int array

        var he; // hash entries byte array

        var stride;

        var htindex, lock;

        var posed, hits; // counts transtore calls

        function TransGame() {
                super();
                //print("calling TransGame()");
                ht = new Array(TRANSIZE);//int[TRANSIZE];
                he = new Array(TRANSIZE);//byte[TRANSIZE];
                for(var i=0;i<TRANSIZE;i++)
                {
                        ht[i]=0;
                        he[i]=0;
                }
        }

        function emptyTT() {
                var i, h, work;

                for (i = 0; i < TRANSIZE; i++)
                        if ((work = (h = he[i]) & 31) < 31) // bytes are signed!!!
                        {
                                he[i] =  (h - (work < 16 ? work : 4)); // work = work
                                                                                                                                // monus 4
                                he[i] = he[i] & 0xFF; //converting to an int
                                //negating
                                if(he[i]&0x40)
                                {
                                        he[i]*=-1;
                                }
                                //print("emptyTT he[i]="+he[i]);
                                
                        }
                posed = hits = 0;
                print("exit emptyTT()");
        }

        function hitRate() {
                return posed != 0 ? hits / posed : 0.0;
        }

        function bitOr(num1,num2)
        {
                var binNum1 = d2b(num1);
                var binNum2 = d2b(num2);
                var binNum1Length = binNum1.length;
                var binNum2Length = binNum2.length;
                var diffString="";
                for(var i=0;i<Math.abs(binNum1Length-binNum2Length);i++)
                {
                        diffString += "0";
                }
                if(binNum1Length>binNum2Length)
                {
                        binNum2 = diffString+binNum2;
                }else
                {
                        binNum1 = diffString+binNum1;
                }
                var slength=binNum1.length;
                var newString="";
                for(var i=0;i<slength;i++)
                {
                        if(binNum1.charAt(i)=='1' || binNum2.charAt(i)=='1')
                        {
                                newString = newString+"1";
                        }else
                        {
                                newString = newString+"0";
                        }
                }
                return b2d(newString);
        }
        function hash() {
                var t1, t2;

                var htemp;

                t1 = (columns[1] << 7 | columns[2]) << 7 | columns[3];
                t2 = (columns[7] << 7 | columns[6]) << 7 | columns[5];
                //printStuff("t1: "+t1+" t2: "+t2);
                htemp = t1 > t2 ? bitOr(leftShift((leftShift(t1, 7) | columns[4]),21) , t2) : bitOr(leftShift((leftShift(t2, 7) | columns[4]),21) , t1);
                //var __tempa = leftShift(t2, 7);//t2 << 7;
                //printStuff("(t2 << 7): "+__tempa);
                //var __tempb =1.0;
                //__tempb = (leftShift(t2, 7) | columns[4]);
                //printStuff("((t2 << 7) | columns[4]): "+__tempb)
                
                //var __temp=1.0;
                //__temp=leftShift(__tempb, 21);//__tempb << 21;
                //__temp=__temp&0xFFFFFFFF;
                //printStuff("((t2 << 7) | columns[4]) << 21:q "+__temp);
                //var __temp2 = bitOr(leftShift((leftShift(t2, 7) | columns[4]),21) , t1);
                //printStuff("(t2 << 7 | columns[4]) << 21 | t1 :"+__temp2);
                
                //printStuff("htemp: "+htemp+"columns[4]: "+columns[4]);
                lock =  rightShift(htemp , 17);
                lock = lock&0xFFFFFFFF; //In Java it converts it to a signed int from a double, this this simulates this. It takes advangate of the fact that bit wise opeerators are converted to "int" inernally
                //printStuff("lock: "+lock);
                //printStuff("lock2: "+(lock&0xFFFFFFFF));
                htindex =  (htemp % TRANSIZE);
                //printStuff("htindex: "+htindex);
                stride = NSAMELOCK + lock % STRIDERANGE;
                //printStuff("STRIDERANGE: "+STRIDERANGE);
                //printStuff("stride: "+stride+"mod is: "+lock % STRIDERANGE);
                if (lock < 0) { // can't take unsigned mod in Java :(
                        if ((stride += INTMODSTRIDERANGE) < NSAMELOCK)
                        {
                                stride += STRIDERANGE;
                                //printStuff("stride in if: stride="+stride+ " lock="+lock+" INTMODSTRIDERANGE="+INTMODSTRIDERANGE+" STRIDERANGE="+STRIDERANGE);
                        }
                }
        }

        function transpose() {
                hash();
                for (var x = htindex, i = 0; i < PROBES; i++) {
                        if (ht[x] == lock)
                        {
                                //printStuff("transpose: lock="+lock+" :he[x]="+he[x]);
                                return he[x];
                        }
                        if ((x += stride) >= TRANSIZE)
                                x -= TRANSIZE;
                }
                //printStuff("ABSENT is returned lock is: "+lock);
                return ABSENT;
        }

        function result() {
                var x;

                return (x = transpose()) == ABSENT ? "n/a" : result1(x);
        }

        function result1(x) {
                return "" + "##-<=>+#".charAt(4 + (x >> 5)) + "(" + (x & 31) + ")";
        }

        function transrestore( score,  work) {
                //print("score: "+score+":work"+work);
                var i, x;

                if (work > 31)
                        work = 31;
                posed++;
                hash();
                for (x = htindex, i = 0; i < PROBES; i++) {
                        if (ht[x] == lock) {
                                hits++;
                                he[x] = (score << 5 | work);
                                he[x] = he[x]&0xFF; //converting to byte
                                //Alex: Well since java treates byte type as signed and this program actually relies on this
                                //Alex: I need to extend it to an int and "sign" the damn thing.
                                if(he[x]&0x80)
                                {
                                        he[x]=he[x] | 0xFFFFFF00;
                                }
                                //printStuff("transrestore he is: "+he[x]);
                                return;
                        }
                        if ((x += stride) >= TRANSIZE)
                                x -= TRANSIZE;
                }
                transput(score, work);
        }

        function transtore(score, work) {
                //print("executing transtore");
                if (work > 31)
                        work = 31;
                posed++;
                hash();
                transput(score, work);
        }

        function transput(score, work) {
                for (x = htindex, i = 0; i < PROBES; i++) {
                        if (work > (he[x] & 31)) {
                                hits++;
                                //printStuff("transput lock = "+lock+" work ="+work+" he[x]&31 ="+(he[x]&31)+" he[x] ="+he[x]+" score="+score);
                                ht[x] = lock;
                                he[x] = (score << 5 | work);
                                he[x] = he[x]&0xFF; //converting to byte
                                //printStuff("transput before convert he is: "+he[x]);
                                //Alex: Well since java treates byte type as signed and this program actually relies on this
                                //Alex: I need to extend it to an int and "sign" the damn thing.
                                if(he[x]&0x80)
                                {
                                        he[x]=he[x] | 0xFFFFFF00;
                                }
                                //printStuff("transput after convert he is: "+he[x]+" stride="+stride+" TRANSIZE="+TRANSIZE+" x"+x);
                                return;
                        }
                        if ((x += stride) >= TRANSIZE)
                                x -= TRANSIZE;
                }
        }

        function htstat() /* some statistics on hash table performance */
        {
                var total, i;
                var buf = ""; //string buffer
                var works;
                var typecnt; // bound type stats

                works = new Array(32);
                typecnt = new Array(8);
                for (i = 0; i < 32; i++)
                        works[i] = 0;
                for (i = 0; i < 8; i++)
                        typecnt[i] = 0;
                for (i = 0; i < TRANSIZE; i++) {
                        works[he[i] & 31]++;
                        if ((he[i] & 31) != 0)
                        {
                                typecnt[4 + (he[i] >> 5)]++;
                                //printStuff("he shifted is: "+(he[i]>>5));
                        }
                }
                for (total = i = 0; i < 8; i++)
                        total += typecnt[i];
                var num1 = typecnt[4 + LOSE]/total;
                var num2 = typecnt[4 + DRAWLOSE] / total;
                var num3 = typecnt[4 + DRAW]    / total;
                var num4 = typecnt[4 + DRAWWIN]/ total;
                var num5 = typecnt[4 + WIN]     / total;
                if (total > 0)
                        buf+="store rate = " + hitRate() + "\n- " + num1 + " < " + num2 + " = " + num3 + " > " + num4 + " + " + num5 + "\n";
                for (i = 0; i < 32; i++) {
                        buf+=works[i];
                        buf+=(i & 7) == 7 ? '\n' : '\t';
                }
                return buf;
        }
}

class SearchGame extends TransGame {

        var _history = new Array(2);
                
        var nodes, msecs;

        function SearchGame() {
                super();
                //print("calling SearchGame");
                _history[0] = new Array(-1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 4, 2, 1, 0, -1, 1, 3, 5, 7, 5, 3, 1, -1, 2, 5, 8, 10, 8, 5, 2, -1, 2, 5, 8, 10, 8, 5, 2, -1, 1, 3, 5, 7, 5, 3, 1, -1, 0, 1, 2, 4, 2, 1, 0);
                _history[1] = new Array(-1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 4, 2, 1, 0, -1, 1, 3, 5, 7, 5, 3, 1, -1, 2, 5, 8, 10, 8, 5, 2, -1, 2, 5, 8, 10, 8, 5, 2, -1, 1, 3, 5, 7, 5, 3, 1, -1, 0, 1, 2, 4, 2, 1, 0);
        
        }

        function solve() {
                //print("executing solve");
                var i, side;
                var x, work, score;
                var poscnt;

                nodes = 0; //long
                msecs = 1; //long
                side = (plycnt + 1) & 1;
                for (i = 0; ++i <= 7;)
                        if (height[i] <= 6) {
                                if (wins(i, height[i], 1 << side) || colthr[columns[i]] == (1 << side))
                                        return (side != 0 ? WIN : LOSE) << 5; // all score, no
                                                                                                                        // work:)
                        }
                if ((x = transpose()) != ABSENT) {
                        if ((x & 32) == 0) // exact score
                                return x;
                }
                score = ab(LOSE, WIN);
                poscnt = posed;
                for (work = 1; (poscnt >>= 1) != 0; work++)
                        ; // work = log of #positions stored
                return score << 5 | work;
        }

        function ab( alpha,  beta) {
                //print("executing ab");
                var besti, i, j, h, k, l, val, score;
                var x, v, work;
                var nav;
                var av = new Array(8);
                var _temp;
                for(var _i;_i++;_i<8)
                {
                        av[_i]=0;
                }
                var poscnt;
                var side, otherside;

                nodes++;
                if (plycnt == 41)
                        return DRAW;
                side = (otherside = plycnt & 1) ^ 1;
                for (i = nav = 0; ++i <= 7;) {
                        if ((h = height[i]) <= 6) {
                                //printStuff("wins1");
                                if (wins(i, h, 3) || colthr[columns[i]] != 0) {
                                        //printStuff("wins2");
                                        if (h + 1 <= 6 && (_temp=wins(i, h + 1, 1 << otherside)))
                                        {
                                                //printStuff("exit1: h-"+h+":_temp-"+_temp);
                                                return LOSE; // for 'o'
                                        }
                                        av[0] = i; // forget other moves
                                        //printStuff("wins3");
                                        while (++i <= 7)
                                                if ((h = height[i]) <= 6&& (wins(i, h, 3) || colthr[columns[i]] != 0))
                                                {
                                                        //printStuff("exit2: h-"+h);
                                                        return LOSE;
                                                }
                                        nav = 1;
                                        break;
                                }
                                //printStuff("wins("+i+","+h+",3) = "+wins(i, h, 3)+":::colthr[columns[i]]="+colthr[columns[i]]);
                                //printStuff("wins4");
                                if (!(h + 1 <= 6 && wins(i, h + 1, 1 << otherside)))
                                {
                                        av[nav++] = i;
                                        //printStuff("nav is incremented:"+nav+" av is: "+av[nav-1]);
                                }else
                                {
                                        //printStuff("Nav not incremented");
                                }
                        }
                }
                //print("after loop");
                if (nav == 0)
                {
                        //printStuff("exit3");
                        return LOSE;
                }
                        
                if (nav == 1) {
                        //printStuff("makeMove1");
                        makemove(av[0]);
                        score = -ab(-beta, -alpha);
                        backmove();
                        return score;
                }
                if ((x = transpose()) != ABSENT) {
                        score = x >> 5;
                        //printStuff("score in ab is: "+score);
                        if (score == DRAWLOSE) {
                                if ((beta = DRAW) <= alpha)
                                {
                                        //printStuff("exit4");
                                        return score;
                                }
                        } else if (score == DRAWWIN) {
                                if ((alpha = DRAW) >= beta)
                                {
                                        //printStuff("exit5");
                                        return score;
                                }
                        } else
                        {
                                //printStuff("exit6 x-"+x+": score-"+score);
                                return score; // exact score
                        }
                }else
                {
                        //printStuff("else part, x is: "+x);
                }
                poscnt = posed;
                l = besti = 0; // initialize arbitrarily for silly javac
                score = MIN_VALUE; // try to get the best bound if score > beta
                for (i = 0; i < nav; i++) {
                        for (j = i, val = MIN_VALUE; j < nav; j++) {
                                k = av[j];
                                v = _history[side][height[k] << 3 | k];
                                if (v > val) {
                                        val = v;
                                        l = j;
                                }
                        }
                        j = av[l];
                        if (i != l) {
                                av[l] = av[i];
                                av[i] = j;
                        }
                        //printStuff("makeMove2");
                        makemove(j);
                        val = -ab(-beta, -alpha);
                        backmove();
                        if (val > score) {
                                besti = i;
                                if ((score = val) > alpha && (alpha = val) >= beta) {
                                        if (score == DRAW && i < nav - 1)
                                                score = DRAWWIN;
                                        break;
                                }
                        }
                }
                if (besti > 0) {
                        for (i = 0; i < besti; i++)
                                _history[side][height[av[i]] << 3 | av[i]]--; // punish bad
                                                                                                                                // historiess
                        _history[side][height[av[besti]] << 3 | av[besti]] += besti;
                }
                poscnt = posed - poscnt;
                for (work = 1; (poscnt >>= 1) != 0; work++)
                        ; // work=log #positions stored
                if (x != ABSENT) {
                        if (score == -(x >> 5)) // combine < and >
                                score = DRAW;
                        transrestore(score, work);
                } else
                        transtore(score, work);
                // if (plycnt == REPORTPLY) {
                // System.out.println(toString() + "##-<=>+#".charAt(4+score) + work);
                // }
                //printStuff("exit7: score="+score);
                return score;
        }

}

class JGFSearchBench extends SearchGame
{
        var size;
        function JGFSearchBench(){}
        function initDoubleArray(size1, size2)
        {
                var mainArray = new Array(size1);
                for(var i=0;i<size1;i++)
                {
                        mainArray[i] = new Array(size2);
                }
                return mainArray;
        }
        function JGFsetsize(size) {
                this.size = size;
        }

        function JGFinitialise() {
                //print("execute JGFInitialize");
                reset();
                //print("after reset");
                for (var i = 0; i < startingMoves[size].length; i++)
                        makemove(startingMoves[size].charAt(i) - '0');
                emptyTT();
        }

        function JGFapplication() {
                //print("Execute JGFapplication");
                //solve();
                var result = solve();
        }

        function JGFvalidate() {
                var i, works;
                var ref = new Array(2);
                ref[0] = new Array(422, 97347, 184228, 270877, 218810, 132097, 72059, 37601, 18645, 9200, 4460, 2230, 1034, 502, 271, 121, 55, 28,11, 6, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
                ref[1] = new Array(0, 1, 9, 2885, 105101, 339874, 282934, 156627, 81700, 40940, 20244, 10278, 4797, 2424, 1159, 535, 246, 139, 62, 28, 11, 11, 3, 0, 3, 0, 0, 0, 0, 0, 0, 0);
                var failed=false;
                works = new Array(32);
                for (i = 0; i < 32; i++)
                        works[i] = 0;
                for (i = 0; i < TRANSIZE; i++)
                {
                        works[he[i] & 31]++;
                        //printStuff("he["+i+"] ="+he[i]);
                }

                for (i = 0; i < 32; i++) {
                        var error = works[i] - ref[size][i];
                        if (error != 0) {
                                print("Validation failed for work count " + i);
                                print("Computed value = " + works[i]);
                                print("Reference value = " + ref[size][i]);
                                failed=true;
                        }
                }
                if(!failed)
                {
                        print("Run succeded");
                }
        }

        function JGFtidyup() {
                // Make sure large arrays are gc'd.
                ht = null;
                he = null;
        }

        function JGFrun(size) {
                print("Java Grande Forum Benchmark Suite - Version 2.0 - Section 3 - Size A");
                print("");
                JGFsetsize(size);
                JGFinitialise();
                JGFapplication();
                JGFvalidate();
                //JGFtidyup();
        }
}



//interface ConnectFourConstants {

//}