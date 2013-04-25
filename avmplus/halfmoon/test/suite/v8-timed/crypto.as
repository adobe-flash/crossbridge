/*
 * Copyright (c) 2003-2005  Tom Wu
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL TOM WU BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF
 * THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * In addition, the following condition applies:
 *
 * All redistributions must retain an intact copy of this copyright notice
 * and disclaimer.
 */
 
package crypto {

  public class BigInteger {
  
    public static const LOWPRIMES = [2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509];
    public static const BI_RM = "0123456789abcdefghijklmnopqrstuvwxyz";
    
    public static var ZERO;
    public static var ONE;
    public static var constsInitialized = false;
    public static var defaultAm = 3;
    public static var dbits;
    public static var BI_DB;
    public static var BI_DM;
    public static var BI_DV;
    public static var BI_FP;
    public static var BI_FV;
    public static var BI_F1;
    public static var BI_F2;
    public static var BI_RC;
    
    public var array;
    public var t;
    public var s;
    public var am; // amX function to use
    
    public function BigInteger(a=null, b=null, c=null) {
      this.array = new Array();
      if(a != null)
        if(a is Number) fromNumber(a,b,c);
      else if(b == null && !(a is String))
        fromString(a,256);
      else fromString(a,b);
      
      // set the am function
      switch (defaultAm) {
        case 1:
          this.am = am1;
          break;
        case 2:
          this.am = am2;
          break;
        case 3:
          this.am = am3;
          break
        case 4:
          this.am = am4;
          break;
        default:
          trace('Error: no am value is set');
      }
    }   
    
    // (public) alternate constructor
    // bnpFromNumber
    public function fromNumber(a, b, c = null) {
      if(b is Number) {
        // new BigInteger(int,int,RNG)
        if(a < 2) this.fromInt(1);
        else {
          this.fromNumber(a,c);
          if(!this.testBit(a-1))	// force MSB set
            this.bitwiseTo(BigInteger.ONE.shiftLeft(a-1),op_or,this);
          if(this.isEven()) this.dAddOffset(1,0); // force odd
          while(!this.isProbablePrime(b)) {
            this.dAddOffset(2,0);
            if(this.bitLength() > a) this.subTo(BigInteger.ONE.shiftLeft(a-1),this);
          }
        }
      } else {
        // new BigInteger(int,RNG)
        var x = new Array(), t = a&7;
        x.length = (a>>3)+1;
        b.nextBytes(x);
        if(t > 0) x[0] &= ((1<<t)-1); else x[0] = 0;
        //TODO: why is an array passed when a string is expected?
        //this.fromString(x,256);
      }
    }
    
    // am: Compute w_j += (x*this_i), propagate carries,
    // c is initial carry, returns final carry.
    // c < 3*dvalue, x < 2*dvalue, this_i < dvalue
    // We need to select the fastest one that works in this environment.
    
    // am1: use a single mult and divide to get the high bits,
    // max digit bits should be 26 because
    // max internal value = 2*dvalue^2-2*dvalue (< 2^53)
    public function am1(i,x,w,j,c,n) {
      var this_array = this.array;
      var w_array    = w.array;
      while(--n >= 0) {
        var v = x*this_array[i++]+w_array[j]+c;
        c = Math.floor(v/0x4000000);
        w_array[j++] = v&0x3ffffff;
      }
      return c;
    }
    
    // am2 avoids a big mult-and-extract completely.
    // Max digit bits should be <= 30 because we do bitwise ops
    // on values up to 2*hdvalue^2-hdvalue-1 (< 2^31)
    public function am2(i,x,w,j,c,n) {
      var this_array = this.array;
      var w_array    = w.array;
      var xl = x&0x7fff, xh = x>>15;
      while(--n >= 0) {
        var l = this_array[i]&0x7fff;
        var h = this_array[i++]>>15;
        var m = xh*l+h*xl;
        l = xl*l+((m&0x7fff)<<15)+w_array[j]+(c&0x3fffffff);
        c = (l>>>30)+(m>>>15)+xh*h+(c>>>30);
        w_array[j++] = l&0x3fffffff;
      }
      return c;
    }
    
    // Alternately, set max digit bits to 28 since some
    // browsers slow down when dealing with 32-bit numbers.
    public function am3(i,x,w,j,c,n) {
      var this_array = this.array;
      var w_array    = w.array;
    
      var xl = x&0x3fff, xh = x>>14;
      while(--n >= 0) {
        var l = this_array[i]&0x3fff;
        var h = this_array[i++]>>14;
        var m = xh*l+h*xl;
        l = xl*l+((m&0x3fff)<<14)+w_array[j]+c;
        c = (l>>28)+(m>>14)+xh*h;
        w_array[j++] = l&0xfffffff;
      }
      return c;
    }
    
    // This is tailored to VMs with 2-bit tagging. It makes sure
    // that all the computations stay within the 29 bits available.
    public function am4(i,x,w,j,c,n) {
      var this_array = this.array;
      var w_array    = w.array;
    
      var xl = x&0x1fff, xh = x>>13;
      while(--n >= 0) {
        var l = this_array[i]&0x1fff;
        var h = this_array[i++]>>13;
        var m = xh*l+h*xl;
        l = xl*l+((m&0x1fff)<<13)+w_array[j]+c;
        c = (l>>26)+(m>>13)+xh*h;
        w_array[j++] = l&0x3ffffff;
      }
      return c;
    }
    
    // am3/28 is best for SM, Rhino, but am4/26 is best for v8.
    // Kestrel (Opera 9.5) gets its best result with am4/26.
    // IE7 does 9% better with am3/28 than with am4/26.
    // Firefox (SM) gets 10% faster with am3/28 than with am4/26.
    
    public static function setupEngine(selectedAm, bits) {
      defaultAm = selectedAm;
      dbits = bits;
    
      BI_DB = dbits;
      BI_DM = ((1<<dbits)-1);
      BI_DV = (1<<dbits);
    
      BI_FP = 52;
      BI_FV = Math.pow(2,BI_FP);
      BI_F1 = BI_FP-dbits;
      BI_F2 = 2*dbits-BI_FP;
      
      // Digit conversions
      BI_RC = new Array();
      var rr,vv;
      rr = "0".charCodeAt(0);
      for(vv = 0; vv <= 9; ++vv) BI_RC[rr++] = vv;
      rr = "a".charCodeAt(0);
      for(vv = 10; vv < 36; ++vv) BI_RC[rr++] = vv;
      rr = "A".charCodeAt(0);
      for(vv = 10; vv < 36; ++vv) BI_RC[rr++] = vv;

      // init consts
      BigInteger.ZERO = nbv(0);
      BigInteger.ONE = nbv(1);
    }
      // return bigint initialized to value
    public static function nbv(i) { 
      var r = nbi(); 
      r.fromInt(i); 
      return r; 
    }
    
    // return new, unset BigInteger
    public static function nbi() {
      return new BigInteger(null);
    }

    public function int2char(n) {
      return BI_RM.charAt(n); 
    }
    
    public function intAt(s,i) {
      var c = BI_RC[s.charCodeAt(i)];
      return (c==null)?-1:c;
    }


    
    // (public) return x s.t. r^x < DV
    // bnpChunkSize
    public function chunkSize(r) {
      return Math.floor(Math.LN2*BI_DB/Math.log(r)); 
    }
    
    // (public) convert to radix string
    // bnpToRadix
    public function toRadix(b=undefined) {
      if(b == undefined) b = 10;
      if(this.signum() == 0 || b < 2 || b > 36) return "0";
      var cs = this.chunkSize(b);
      var a = Math.pow(b,cs);
      var d = nbv(a), y = nbi(), z = nbi(), r = "";
      this.divRemTo(d,y,z);
      while(y.signum() > 0) {
        r = (a+z.intValue()).toString(b).substr(1) + r;
        y.divRemTo(d,y,z);
      }
      return z.intValue().toString(b) + r;
    }
    
    // (public) convert from radix string
    // bnpFromRadix
    public function fromRadix(s,b = undefined) {
      this.fromInt(0);
      if(b == undefined) b = 10;
      var cs = this.chunkSize(b);
      var d = Math.pow(b,cs), mi = false, j = 0, w = 0;
      for(var i = 0; i < s.length; ++i) {
        var x = intAt(s,i);
        if(x < 0) {
          if(s.charAt(i) == "-" && this.signum() == 0) mi = true;
          continue;
        }
        w = b*w+x;
        if(++j >= cs) {
          this.dMultiply(d);
          this.dAddOffset(w,0);
          j = 0;
          w = 0;
        }
      }
      if(j > 0) {
        this.dMultiply(Math.pow(b,j));
        this.dAddOffset(w,0);
      }
      if(mi) BigInteger.ZERO.subTo(this,this);
    }
    
    // (public) r = this op a (bitwise)
    // bnpBitwiseTo
    public function bitwiseTo(a,op,r) {
      var this_array = this.array;
      var a_array    = a.array;
      var r_array    = r.array;
      var i, f, m = Math.min(a.t,this.t);
      for(i = 0; i < m; ++i) r_array[i] = op(this_array[i],a_array[i]);
      if(a.t < this.t) {
        f = a.s&BI_DM;
        for(i = m; i < this.t; ++i) r_array[i] = op(this_array[i],f);
        r.t = this.t;
      }
      else {
        f = this.s&BI_DM;
        for(i = m; i < a.t; ++i) r_array[i] = op(f,a_array[i]);
        r.t = a.t;
      }
      r.s = op(this.s,a.s);
      r.clamp();
    }
    
    // (public) this op (1<<n)
    // bnpChangeBit
    public function changeBit(n,op) {
      var r = BigInteger.ONE.shiftLeft(n);
      this.bitwiseTo(r,op,r);
      return r;
    }
    
    // (public) r = this + a
    // bnpAddTo
    public function addTo(a,r) {
      var this_array = this.array;
      var a_array = a.array;
      var r_array = r.array;
      var i = 0, c = 0, m = Math.min(a.t,this.t);
      while(i < m) {
        c += this_array[i]+a_array[i];
        r_array[i++] = c&BI_DM;
        c >>= BI_DB;
      }
      if(a.t < this.t) {
        c += a.s;
        while(i < this.t) {
          c += this_array[i];
          r_array[i++] = c&BI_DM;
          c >>= BI_DB;
        }
        c += this.s;
      }
      else {
        c += this.s;
        while(i < a.t) {
          c += a_array[i];
          r_array[i++] = c&BI_DM;
          c >>= BI_DB;
        }
        c += a.s;
      }
      r.s = (c<0)?-1:0;
      if(c > 0) r_array[i++] = c;
      else if(c < -1) r_array[i++] = BI_DV+c;
      r.t = i;
      r.clamp();
    }
    
    // (public) this *= n, this >= 0, 1 < n < DV
    // bnpDMultiply
    public function dMultiply(n) {
      var this_array = this.array;
      this_array[this.t] = this.am(0,n-1,this,0,0,this.t);
      ++this.t;
      this.clamp();
    }
    
    // (public) this += n << w words, this >= 0
    // bnpDAddOffset
    public function dAddOffset(n,w) {
      var this_array = this.array;
      while(this.t <= w) this_array[this.t++] = 0;
      this_array[w] += n;
      while(this_array[w] >= BI_DV) {
        this_array[w] -= BI_DV;
        if(++w >= this.t) this_array[this.t++] = 0;
        ++this_array[w];
      }
    }
    
    // (public) r = lower n words of "this * a", a.t <= n
    // "this" should be the larger one if appropriate.
    // bnpMultiplyLowerTo
    public function multiplyLowerTo(a,n,r) {
      var r_array = r.array;
      var a_array = a.array;
      var i = Math.min(this.t+a.t,n);
      r.s = 0; // assumes a,this >= 0
      r.t = i;
      while(i > 0) r_array[--i] = 0;
      var j;
      for(j = r.t-this.t; i < j; ++i) r_array[i+this.t] = this.am(0,a_array[i],r,i,0,this.t);
      for(j = Math.min(a.t,n); i < j; ++i) this.am(0,a_array[i],r,i,0,n-i);
      r.clamp();
    }
    
    // (public) r = "this * a" without lower n words, n > 0
    // "this" should be the larger one if appropriate.
    // bnpMultiplyUpperTo
    public function multiplyUpperTo(a,n,r) {
      var r_array = r.array;
      var a_array = a.array;
      --n;
      var i = r.t = this.t+a.t-n;
      r.s = 0; // assumes a,this >= 0
      while(--i >= 0) r_array[i] = 0;
      for(i = Math.max(n-this.t,0); i < a.t; ++i)
        r_array[this.t+i-n] = this.am(n-i,a_array[i],r,0,0,this.t+i-n);
      r.clamp();
      r.drShiftTo(1,r);
    }
    
    // (public) this % n, n < 2^26
    // bnpModInt
    public function modInt(n) {
      var this_array = this.array;
      if(n <= 0) return 0;
      var d = BI_DV%n, r = (this.s<0)?n-1:0;
      if(this.t > 0)
        if(d == 0) r = this_array[0]%n;
        else for(var i = this.t-1; i >= 0; --i) r = (d*r+this_array[i])%n;
      return r;
    }
    
    // (public) true if probably prime (HAC 4.24, Miller-Rabin)
    // bnpMillerRabin
    public function millerRabin(t) {
      var n1 = this.subtract(BigInteger.ONE);
      var k = n1.getLowestSetBit();
      if(k <= 0) return false;
      var r = n1.shiftRight(k);
      t = (t+1)>>1;
      if(t > LOWPRIMES.length) t = LOWPRIMES.length;
      var a = nbi();
      for(var i = 0; i < t; ++i) {
        a.fromInt(LOWPRIMES[i]);
        var y = a.modPow(r,this);
        if(y.compareTo(BigInteger.ONE) != 0 && y.compareTo(n1) != 0) {
          var j = 1;
          while(j++ < k && y.compareTo(n1) != 0) {
            y = y.modPowInt(2,this);
            if(y.compareTo(BigInteger.ONE) == 0) return false;
          }
          if(y.compareTo(n1) != 0) return false;
        }
      }
      return true;
    }
    
    // (public)
    // bnClone
    public function clone() {
      var r = nbi(); 
      this.copyTo(r); 
      return r; 
    }
    
    
    // (public) return value as integer
    // bnIntValue
    public function intValue() {
      var this_array = this.array;
      if(this.s < 0) {
        if(this.t == 1) return this_array[0]-BI_DV;
        else if(this.t == 0) return -1;
      }
      else if(this.t == 1) return this_array[0];
      else if(this.t == 0) return 0;
      // assumes 16 < DB < 32
      return ((this_array[1]&((1<<(32-BI_DB))-1))<<BI_DB)|this_array[0];
    }
    
    // (public) return value as byte
    // bnByteValue
    // TODO: return type?
    public function byteValue() {
      var this_array = this.array;
      return (this.t==0)?this.s:(this_array[0]<<24)>>24;
    }
    
    // (public) return value as short (assumes DB>=16)
    // bnShortValue
    public function shortValue() {
      var this_array = this.array;
      return (this.t==0)?this.s:(this_array[0]<<16)>>16;
    }
    
    // (public) 0 if this == 0, 1 if this > 0
    // bnSigNum
    public function signum() {
      var this_array = this.array;
      if(this.s < 0) return -1;
      else if(this.t <= 0 || (this.t == 1 && this_array[0] <= 0)) return 0;
      else return 1;
    }
    
    // (public) convert to bigendian byte array
    // bnToByteArray
    public function toByteArray() {
      var this_array = this.array;
      var i = this.t, r = new Array();
      r[0] = this.s;
      var p = BI_DB-(i*BI_DB)%8, d, k = 0;
      if(i-- > 0) {
        if(p < BI_DB && (d = this_array[i]>>p) != (this.s&BI_DM)>>p)
          r[k++] = d|(this.s<<(BI_DB-p));
        while(i >= 0) {
          if(p < 8) {
            d = (this_array[i]&((1<<p)-1))<<(8-p);
            d |= this_array[--i]>>(p+=BI_DB-8);
          }
          else {
            d = (this_array[i]>>(p-=8))&0xff;
            if(p <= 0) { p += BI_DB; --i; }
          }
          if((d&0x80) != 0) d |= -256;
          if(k == 0 && (this.s&0x80) != (d&0x80)) ++k;
          if(k > 0 || d != this.s) r[k++] = d;
        }
      }
      return r;
    }
    
    // bnEquals
    public function equals(a) {
      return(this.compareTo(a)==0); 
    }
    
    // bnMin
    public function min(a) {
      return(this.compareTo(a)<0)?this:a;
    }
    
    // bnMax
    public function max(a) {
      return(this.compareTo(a)>0)?this:a; 
    }
    
    
    
    // (public) this & a
    private function op_and(x,y) { return x&y; }
    // bnAnd
    public function and(a) {
      var r = nbi(); 
      this.bitwiseTo(a,op_and,r); 
      return r;
    }
    
    // (public) this | a
    public function op_or(x,y) { return x|y; }
    // bnOr
    public function or(a) { 
      var r = nbi();
      this.bitwiseTo(a,op_or,r);
      return r; 
    }
    
    // (public) this ^ a
    public function op_xor(x,y) { return x^y; }
    //bnXor
    public function xor(a) {
      var r = nbi();
      this.bitwiseTo(a,op_xor,r);
      return r; 
    }
    
    // (public) this & ~a
    public function op_andnot(x,y) { return x&~y; }
    // bnAndNot
    public function andNot(a) { 
      var r = nbi(); 
      this.bitwiseTo(a,op_andnot,r); 
      return r; 
    }
    
    // (public) ~this
    // bnNot
    public function not() {
      var this_array = this.array;
      var r = nbi();
      var r_array = r.array;
    
      for(var i = 0; i < this.t; ++i) r_array[i] = BI_DM&~this_array[i];
      r.t = this.t;
      r.s = ~this.s;
      return r;
    }
    
    // (public) this << n
    // bnShiftLeft
    public function shiftLeft(n) {
      var r = nbi();
      if(n < 0) this.rShiftTo(-n,r); else this.lShiftTo(n,r);
      return r;
    }
    
    // (public) this >> n
    // bnShiftRight
    public function shiftRight(n) {
      var r = nbi();
      if(n < 0) this.lShiftTo(-n,r); else this.rShiftTo(n,r);
      return r;
    }
    
    // return index of lowest 1-bit in x, x < 2^31
    private function lbit(x) {
      if(x == 0) return -1;
      var r = 0;
      if((x&0xffff) == 0) { x >>= 16; r += 16; }
      if((x&0xff) == 0) { x >>= 8; r += 8; }
      if((x&0xf) == 0) { x >>= 4; r += 4; }
      if((x&3) == 0) { x >>= 2; r += 2; }
      if((x&1) == 0) ++r;
      return r;
    }
    
    // (public) returns index of lowest 1-bit (or -1 if none)
    // bnGetLowestSetBit
    public function getLowestSetBit() {
      var this_array = this.array;
      for(var i = 0; i < this.t; ++i)
        if(this_array[i] != 0) return i*BI_DB+lbit(this_array[i]);
      if(this.s < 0) return this.t*BI_DB;
      return -1;
    }
    
    // return number of 1 bits in x
    private function cbit(x) {
      var r = 0;
      while(x != 0) { x &= x-1; ++r; }
      return r;
    }
    
    // (public) return number of set bits
    // bnBitCount
    public function bitCount() {
      var r = 0, x = this.s&BI_DM;
      for(var i = 0; i < this.t; ++i) r += cbit(this_array[i]^x);
      return r;
    }
    
    // (public) true iff nth bit is set
    // bnTestBit
    public function testBit(n) {
      var this_array = this.array;
      var j = Math.floor(n/BI_DB);
      if(j >= this.t) return(this.s!=0);
      return((this_array[j]&(1<<(n%BI_DB)))!=0);
    }
    
    // (public) this | (1<<n)
    // bnSetBit
    public function setBit(n) { return this.changeBit(n,op_or); }
    
    // (public) this & ~(1<<n)
    // bnClearBit
    public function clearBit(n) { return this.changeBit(n,op_andnot); }
    
    // (public) this ^ (1<<n)
    // bnFlipBit
    public function flipBit(n) { return this.changeBit(n,op_xor); }
    
    // (public) this + a
    // bnAdd
    public function add(a) { 
      var r = nbi();
      this.addTo(a,r);
      return r;
    }
    
    // (public) this - a
    // bnSubtract
    public function subtract(a) { 
      var r = nbi();
      this.subTo(a,r);
      return r;
    }
    
    // (public) this * a
    // bnMultiply
    public function multiply(a) { 
      var r = nbi();
      this.multiplyTo(a,r);
      return r;
    }
    
    // (public) this / a
    // bnDivide
    public function divide(a) {
      var r = nbi();
      this.divRemTo(a,r,null);
      return r;
    }
    
    // (public) this % a
    // bnRemainder
    public function remainder(a) {
      var r = nbi();
      this.divRemTo(a,null,r);
      return r;
    }
    
    // (public) [this/a,this%a]
    // bnDivideAndRemainder
    public function divideAndRemainder(a) {
      var q = nbi(), r = nbi();
      this.divRemTo(a,q,r);
      return new Array(q,r);
    }
  
    // (public) copy this to r
    // bnpCopyTo
    public function copyTo(r) {
      var this_array = this.array;
      var r_array    = r.array;
    
      for(var i = this.t-1; i >= 0; --i) r_array[i] = this_array[i];
      r.t = this.t;
      r.s = this.s;
    }
    
    // (public) set from integer value x, -DV <= x < DV
    // bnpFromInt
    public function fromInt(x) {
      var this_array = this.array;
      this.t = 1;
      this.s = (x<0)?-1:0;
      if(x > 0) this_array[0] = x;
      else if(x < -1) this_array[0] = x+DV;
      else this.t = 0;
    }
    
    // (public) set from string and radix
    // bnpFromString
    public function fromString(s,b) {
      var this_array = this.array;
      var k;
      if(b == 16) k = 4;
      else if(b == 8) k = 3;
      else if(b == 256) k = 8; // byte array
      else if(b == 2) k = 1;
      else if(b == 32) k = 5;
      else if(b == 4) k = 2;
      else { this.fromRadix(s,b); return; }
      this.t = 0;
      this.s = 0;
      var i = s.length, mi = false, sh = 0;
      while(--i >= 0) {
        var x = (k==8)?s[i]&0xff:intAt(s,i);
        if(x < 0) {
          if(s.charAt(i) == "-") mi = true;
          continue;
        }
        mi = false;
        if(sh == 0)
          this_array[this.t++] = x;
        else if(sh+k > BI_DB) {
          this_array[this.t-1] |= (x&((1<<(BI_DB-sh))-1))<<sh;
          this_array[this.t++] = (x>>(BI_DB-sh));
        }
        else
          this_array[this.t-1] |= x<<sh;
        sh += k;
        if(sh >= BI_DB) sh -= BI_DB;
      }
      if(k == 8 && (s[0]&0x80) != 0) {
        this.s = -1;
        if(sh > 0) this_array[this.t-1] |= ((1<<(BI_DB-sh))-1)<<sh;
      }
      
      this.clamp();
      if(mi) BigInteger.ZERO.subTo(this,this);
    }
    
    // (public) clamp off excess high words
    // bnpClamp
    public function clamp() {
      var this_array = this.array;
      var c = this.s&BI_DM;
      while(this.t > 0 && this_array[this.t-1] == c) --this.t;
    }
    
    // (public) return string representation in given radix
    // bnToString
    public function toString(b=16) {
      var this_array = this.array;
      if(this.s < 0) return "-"+this.negate().toString(b);
      var k;
      if(b == 16) k = 4;
      else if(b == 8) k = 3;
      else if(b == 2) k = 1;
      else if(b == 32) k = 5;
      else if(b == 4) k = 2;
      else return this.toRadix(b);
      var km = (1<<k)-1, d, m = false, r = "", i = this.t;
      var p = BI_DB-(i*BI_DB)%k;
      if(i-- > 0) {
        if(p < BI_DB && (d = this_array[i]>>p) > 0) { m = true; r = int2char(d); }
        while(i >= 0) {
          if(p < k) {
            d = (this_array[i]&((1<<p)-1))<<(k-p);
            d |= this_array[--i]>>(p+=BI_DB-k);
          }
          else {
            d = (this_array[i]>>(p-=k))&km;
            if(p <= 0) { p += BI_DB; --i; }
          }
          if(d > 0) m = true;
          if(m) r += int2char(d);
        }
      }
      return m?r:"0";
    }
    
    // (public) -this
    // bnNegate
    public function negate() { 
      var r = nbi(); 
      BigInteger.ZERO.subTo(this,r); 
      return r;
    }
    
    // (public) |this|
    // bnAbs
    public function abs() { 
      return (this.s<0)?this.negate():this; 
    }
    
    // (public) return + if this > a, - if this < a, 0 if equal
    // bnCompareTo
    public function compareTo(a) {
      var this_array = this.array;
      var a_array = a.array;
    
      var r = this.s-a.s;
      if(r != 0) return r;
      var i = this.t;
      r = i-a.t;
      if(r != 0) return r;
      while(--i >= 0) if((r=this_array[i]-a_array[i]) != 0) return r;
      return 0;
    }
    
    // returns bit length of the integer x
    private function nbits(x) {
      var r = 1, t;
      if((t=x>>>16) != 0) { x = t; r += 16; }
      if((t=x>>8) != 0) { x = t; r += 8; }
      if((t=x>>4) != 0) { x = t; r += 4; }
      if((t=x>>2) != 0) { x = t; r += 2; }
      if((t=x>>1) != 0) { x = t; r += 1; }
      return r;
    }
    
    // (public) return the number of bits in "this"
    // bnBitLength
    public function bitLength() {
      var this_array = this.array;
      if(this.t <= 0) return 0;
      return BI_DB*(this.t-1)+nbits(this_array[this.t-1]^(this.s&BI_DM));
    }
    
    // (public) r = this << n*DB
    // bnpDLShiftTo
    public function dlShiftTo(n,r) {
      var this_array = this.array;
      var r_array = r.array;
      var i;
      for(i = this.t-1; i >= 0; --i) r_array[i+n] = this_array[i];
      for(i = n-1; i >= 0; --i) r_array[i] = 0;
      r.t = this.t+n;
      r.s = this.s;
    }
    
    // (public) r = this >> n*DB
    // bnpDRShiftTo
    public function drShiftTo(n,r) {
      var this_array = this.array;
      var r_array = r.array;
      for(var i = n; i < this.t; ++i) r_array[i-n] = this_array[i];
      r.t = Math.max(this.t-n,0);
      r.s = this.s;
    }
    
    // (public) r = this << n
    // bnpLShiftTo
    public function lShiftTo(n,r) {
      var this_array = this.array;
      var r_array = r.array;
      var bs = n%BI_DB;
      var cbs = BI_DB-bs;
      var bm = (1<<cbs)-1;
      var ds = Math.floor(n/BI_DB), c = (this.s<<bs)&BI_DM, i;
      for(i = this.t-1; i >= 0; --i) {
        r_array[i+ds+1] = (this_array[i]>>cbs)|c;
        c = (this_array[i]&bm)<<bs;
      }
      for(i = ds-1; i >= 0; --i) r_array[i] = 0;
      r_array[ds] = c;
      r.t = this.t+ds+1;
      r.s = this.s;
      r.clamp();
    }
    
    // (public) r = this >> n
    // bnpRShiftTo
    public function rShiftTo(n,r) {
      var this_array = this.array;
      var r_array = r.array;
      r.s = this.s;
      var ds = Math.floor(n/BI_DB);
      if(ds >= this.t) { r.t = 0; return; }
      var bs = n%BI_DB;
      var cbs = BI_DB-bs;
      var bm = (1<<bs)-1;
      r_array[0] = this_array[ds]>>bs;
      for(var i = ds+1; i < this.t; ++i) {
        r_array[i-ds-1] |= (this_array[i]&bm)<<cbs;
        r_array[i-ds] = this_array[i]>>bs;
      }
      if(bs > 0) r_array[this.t-ds-1] |= (this.s&bm)<<cbs;
      r.t = this.t-ds;
      r.clamp();
    }
    
    // (public) r = this - a
    // bnpSubTo
    public function subTo(a,r) {
      var this_array = this.array;
      var r_array = r.array;
      var a_array = a.array;
      var i = 0, c = 0, m = Math.min(a.t,this.t);
      while(i < m) {
        c += this_array[i]-a_array[i];
        r_array[i++] = c&BI_DM;
        c >>= BI_DB;
      }
      if(a.t < this.t) {
        c -= a.s;
        while(i < this.t) {
          c += this_array[i];
          r_array[i++] = c&BI_DM;
          c >>= BI_DB;
        }
        c += this.s;
      }
      else {
        c += this.s;
        while(i < a.t) {
          c -= a_array[i];
          r_array[i++] = c&BI_DM;
          c >>= BI_DB;
        }
        c -= a.s;
      }
      r.s = (c<0)?-1:0;
      if(c < -1) r_array[i++] = BI_DV+c;
      else if(c > 0) r_array[i++] = c;
      r.t = i;
      r.clamp();
    }
    
    // (public) r = this * a, r != this,a (HAC 14.12)
    // "this" should be the larger one if appropriate.
    // bnpMultiplyTo
    public function multiplyTo(a,r) {
      var this_array = this.array;
      var r_array = r.array;
      var x = this.abs(), y = a.abs();
      var y_array = y.array;
    
      var i = x.t;
      r.t = i+y.t;
      while(--i >= 0) r_array[i] = 0;
      for(i = 0; i < y.t; ++i) r_array[i+x.t] = x.am(0,y_array[i],r,i,0,x.t);
      r.s = 0;
      r.clamp();
      if(this.s != a.s) BigInteger.ZERO.subTo(r,r);
    }
    
    // (public) r = this^2, r != this (HAC 14.16)
    // bnpSquareTo
    public function squareTo(r) {
      var x = this.abs();
      var x_array = x.array;
      var r_array = r.array;
    
      var i = r.t = 2*x.t;
      while(--i >= 0) r_array[i] = 0;
      for(i = 0; i < x.t-1; ++i) {
        var c = x.am(i,x_array[i],r,2*i,0,1);
        if((r_array[i+x.t]+=x.am(i+1,2*x_array[i],r,2*i+1,c,x.t-i-1)) >= BI_DV) {
          r_array[i+x.t] -= BI_DV;
          r_array[i+x.t+1] = 1;
        }
      }
      if(r.t > 0) r_array[r.t-1] += x.am(i,x_array[i],r,2*i,0,1);
      r.s = 0;
      r.clamp();
    }
    
    // (public) divide this by m, quotient and remainder to q, r (HAC 14.20)
    // r != q, this != m.  q or r may be null.
    // bnpDivRemTo
    public function divRemTo(m,q,r) {
      var pm = m.abs();
      if(pm.t <= 0) return;
      var pt = this.abs();
      if(pt.t < pm.t) {
        if(q != null) q.fromInt(0);
        if(r != null) this.copyTo(r);
        return;
      }
      if(r == null) r = nbi();
      var y= nbi(), ts = this.s, ms = m.s;
      var pm_array = pm.array;
      var nsh = BI_DB-nbits(pm_array[pm.t-1]);	// normalize modulus
      if(nsh > 0) { pm.lShiftTo(nsh,y); pt.lShiftTo(nsh,r); }
      else { pm.copyTo(y); pt.copyTo(r); }
      var ys = y.t;
    
      var y_array = y.array;
      var y0 = y_array[ys-1];
      if(y0 == 0) return;
      var yt= y0*(1<<BI_F1)+((ys>1)?y_array[ys-2]>>BI_F2:0);
      var d1 = BI_FV/yt, d2 = (1<<BI_F1)/yt, e = 1<<BI_F2;
      var i = r.t, j = i-ys, t = (q==null)?nbi():q;
      y.dlShiftTo(j,t);
    
      var r_array = r.array;
      if(r.compareTo(t) >= 0) {
        r_array[r.t++] = 1;
        r.subTo(t,r);
      }
      BigInteger.ONE.dlShiftTo(ys,t);
      t.subTo(y,y);	// "negative" y so we can replace sub with am later
      while(y.t < ys) y_array[y.t++] = 0;
      while(--j >= 0) {
        // Estimate quotient digit
        var qd = (r_array[--i]==y0)?BI_DM:Math.floor(r_array[i]*d1+(r_array[i-1]+e)*d2);
        if((r_array[i]+=y.am(0,qd,r,j,0,ys)) < qd) {	// Try it out
          y.dlShiftTo(j,t);
          r.subTo(t,r);
          while(r_array[i] < --qd) r.subTo(t,r);
        }
      }
      if(q != null) {
        r.drShiftTo(ys,q);
        if(ts != ms) BigInteger.ZERO.subTo(q,q);
      }
      r.t = ys;
      r.clamp();
      if(nsh > 0) r.rShiftTo(nsh,r);	// Denormalize remainder
      if(ts < 0) BigInteger.ZERO.subTo(r,r);
    }
    
    // (public) this mod a
    // bnMod
    public function mod(a) {
      var r = nbi();
      this.abs().divRemTo(a,null,r);
      if(this.s < 0 && r.compareTo(BigInteger.ZERO) > 0) a.subTo(r,r);
      return r;
    }
    
    // (public) return "-1/this % 2^DB"; useful for Mont. reduction
    // justification:
    //         xy == 1 (mod m)
    //         xy =  1+km
    //   xy(2-xy) = (1+km)(1-km)
    // x[y(2-xy)] = 1-k^2m^2
    // x[y(2-xy)] == 1 (mod m^2)
    // if y is 1/x mod m, then y(2-xy) is 1/x mod m^2
    // should reduce x and y(2-xy) by m^2 at each step to keep size bounded.
    // JS multiply "overflows" differently from C/C++, so care is needed here.
    // bnpInvDigit
    public function invDigit() {
      var this_array = this.array;
      if(this.t < 1) return 0;
      var x = this_array[0];
      if((x&1) == 0) return 0;
      var y = x&3;		// y == 1/x mod 2^2
      y = (y*(2-(x&0xf)*y))&0xf;	// y == 1/x mod 2^4
      y = (y*(2-(x&0xff)*y))&0xff;	// y == 1/x mod 2^8
      y = (y*(2-(((x&0xffff)*y)&0xffff)))&0xffff;	// y == 1/x mod 2^16
      // last step - calculate inverse mod DV directly;
      // assumes 16 < DB <= 32 and assumes ability to handle 48-bit ints
      y = (y*(2-x*y%BI_DV))%BI_DV;		// y == 1/x mod 2^dbits
      // we really want the negative inverse, and -DV < y < DV
      return (y>0)?BI_DV-y:-y;
    }
    
    // (public) true iff this is even
    // bnpIsEven
    public function isEven() {
      var this_array = this.array;
      return ((this.t>0)?(this_array[0]&1):this.s) == 0;
    }
    
    // (public) this^e, e < 2^32, doing sqr and mul with "r" (HAC 14.79)
    // bnpExp
    public function exp(e,z) {
      if(e > 0xffffffff || e < 1) return BigInteger.ONE;
      var r = nbi(), r2  = nbi(), g  = z.convert(this), i = nbits(e)-1;
      g.copyTo(r);
      while(--i >= 0) {
        z.sqrTo(r,r2);
        if((e&(1<<i)) > 0) z.mulTo(r2,g,r);
        else { var t = r; r = r2; r2 = t; }
      }
      return z.revert(r);
    }
    
    // (public) this^e % m, 0 <= e < 2^32
    // bnModPowInt
    public function modPowInt(e,m) {
      var z;
      if(e < 256 || m.isEven()) z = new Classic(m); else z = new Montgomery(m);
      return this.exp(e,z);
    }
  
    // (public) this^e
    // bnPow
    public function pow(e) { 
      return this.exp(e,new NullExp()); 
    }
    
    // (public) this^e % m (HAC 14.85)
    // bnModPow
    public function modPow(e,m) {
      var e_array = e.array;
      var i = e.bitLength(), k, r = nbv(1), z;
      if(i <= 0) return r;
      else if(i < 18) k = 1;
      else if(i < 48) k = 3;
      else if(i < 144) k = 4;
      else if(i < 768) k = 5;
      else k = 6;
      if(i < 8)
        z = new Classic(m);
      else if(m.isEven())
        z = new Barrett(m);
      else
        z = new Montgomery(m);
    
      // precomputation
      var g = new Array(), n = 3, k1 = k-1, km = (1<<k)-1;
      g[1] = z.convert(this);
      if(k > 1) {
        var g2 = nbi();
        z.sqrTo(g[1],g2);
        while(n <= km) {
          g[n] = nbi();
          z.mulTo(g2,g[n-2],g[n]);
          n += 2;
        }
      }
    
      var j = e.t-1, w, is1 = true, r2 = nbi(), t;
      i = nbits(e_array[j])-1;
      while(j >= 0) {
        if(i >= k1) w = (e_array[j]>>(i-k1))&km;
        else {
          w = (e_array[j]&((1<<(i+1))-1))<<(k1-i);
          if(j > 0) w |= e_array[j-1]>>(BI_DB+i-k1);
        }
    
        n = k;
        while((w&1) == 0) { w >>= 1; --n; }
        if((i -= n) < 0) { i += BI_DB; --j; }
        if(is1) {	// ret == 1, don't bother squaring or multiplying it
          g[w].copyTo(r);
          is1 = false;
        }
        else {
          while(n > 1) { z.sqrTo(r,r2); z.sqrTo(r2,r); n -= 2; }
          if(n > 0) z.sqrTo(r,r2); else { t = r; r = r2; r2 = t; }
          z.mulTo(r2,g[w],r);
        }
    
        while(j >= 0 && (e_array[j]&(1<<i)) == 0) {
          z.sqrTo(r,r2); t = r; r = r2; r2 = t;
          if(--i < 0) { i = BI_DB-1; --j; }
        }
      }
      return z.revert(r);
    }
    
    // (public) gcd(this,a) (HAC 14.54)
    // bnGCD
    public function gcd(a) {
      var x = (this.s<0)?this.negate():this.clone();
      var y = (a.s<0)?a.negate():a.clone();
      if(x.compareTo(y) < 0) { var t = x; x = y; y = t; }
      var i = x.getLowestSetBit(), g = y.getLowestSetBit();
      if(g < 0) return x;
      if(i < g) g = i;
      if(g > 0) {
        x.rShiftTo(g,x);
        y.rShiftTo(g,y);
      }
      while(x.signum() > 0) {
        if((i = x.getLowestSetBit()) > 0) x.rShiftTo(i,x);
        if((i = y.getLowestSetBit()) > 0) y.rShiftTo(i,y);
        if(x.compareTo(y) >= 0) {
          x.subTo(y,x);
          x.rShiftTo(1,x);
        }
        else {
          y.subTo(x,y);
          y.rShiftTo(1,y);
        }
      }
      if(g > 0) y.lShiftTo(g,y);
      return y;
    }
    
    
    
    // (public) 1/this % m (HAC 14.61)
    // bnModInverse
    public function modInverse(m) {
      var ac = m.isEven();
      if((this.isEven() && ac) || m.signum() == 0) return BigInteger.ZERO;
      var u = m.clone(), v = this.clone();
      var a = nbv(1), b = nbv(0), c = nbv(0), d = nbv(1);
      while(u.signum() != 0) {
        while(u.isEven()) {
          u.rShiftTo(1,u);
          if(ac) {
            if(!a.isEven() || !b.isEven()) { a.addTo(this,a); b.subTo(m,b); }
            a.rShiftTo(1,a);
          }
          else if(!b.isEven()) b.subTo(m,b);
          b.rShiftTo(1,b);
        }
        while(v.isEven()) {
          v.rShiftTo(1,v);
          if(ac) {
            if(!c.isEven() || !d.isEven()) { c.addTo(this,c); d.subTo(m,d); }
            c.rShiftTo(1,c);
          }
          else if(!d.isEven()) d.subTo(m,d);
          d.rShiftTo(1,d);
        }
        if(u.compareTo(v) >= 0) {
          u.subTo(v,u);
          if(ac) a.subTo(c,a);
          b.subTo(d,b);
        }
        else {
          v.subTo(u,v);
          if(ac) c.subTo(a,c);
          d.subTo(b,d);
        }
      }
      if(v.compareTo(BigInteger.ONE) != 0) return BigInteger.ZERO;
      if(d.compareTo(m) >= 0) return d.subtract(m);
      if(d.signum() < 0) d.addTo(m,d); else return d;
      if(d.signum() < 0) return d.add(m); else return d;
    }
    
    // (public) test primality with certainty >= 1-.5^t
    // bnIsProbablePrime
    public function isProbablePrime(t) {
      var lplim = (1<<26)/LOWPRIMES[LOWPRIMES.length-1];
      var i, x = this.abs();
      var x_array = x.array;
      if(x.t == 1 && x_array[0] <= LOWPRIMES[LOWPRIMES.length-1]) {
        for(i = 0; i < LOWPRIMES.length; ++i)
          if(x_array[0] == LOWPRIMES[i]) return true;
        return false;
      }
      if(x.isEven()) return false;
      i = 1;
      while(i < LOWPRIMES.length) {
        var m = LOWPRIMES[i], j = i+1;
        while(j < LOWPRIMES.length && m < lplim) m *= LOWPRIMES[j++];
        m = x.modInt(m);
        while(i < j) if(m%LOWPRIMES[i++] == 0) return false;
      }
      return x.millerRabin(t);
    }
    
  } // class BigInteger

  public class NullExp implements ModularReduction {
    // A "null" reducer
    public function NullExp() {}
    // nNop
    public function convert(x) { 
      return x;
    }
    public function revert(x) {
      return x;
    }
    
    public function reduce(x) {}
    // nMulTo
    public function mulTo(x,y,r) {
      x.multiplyTo(y,r);
    }
    // nSqrTo
    public function sqrTo(x,r) {
      x.squareTo(r);
    }
  } // class NullExp
  
  public interface ModularReduction {
    function convert(x) ;
    function revert(x) ;
    function reduce(x) ;
    function mulTo(x,y,r) ;
    function sqrTo(x,r) ;
  } // interface ModularReduction

  public class Classic implements ModularReduction {
    private var m;
    public function Classic(m) {
      this.m = m;
    }
    // cConvert
    public function convert(x) {
      if(x.s < 0 || x.compareTo(this.m) >= 0) return x.mod(this.m);
      else return x;
    }
    // cRevert
    public function revert(x) { return x; }
    // cReduce
    public function reduce(x) { x.divRemTo(this.m,null,x); }
    // cMulTo
    public function mulTo(x,y,r) {
      x.multiplyTo(y,r); 
      this.reduce(r);
    }
    // cSqrTo
    public function sqrTo(x,r) {
      x.squareTo(r); 
      this.reduce(r); 
    }
  } // class Classic
  
  public class Montgomery implements ModularReduction {
    private var m ;
    private var mt2 ;
    private var mp ;
    private var mpl ;
    private var mph ;
    private var um ;
    public function Montgomery(m) {
      this.m = m;
      this.mp = m.invDigit();
      this.mpl = this.mp&0x7fff;
      this.mph = this.mp>>15;
      this.um = (1<<(BigInteger.BI_DB-15))-1;
      this.mt2 = 2*m.t;
    }
    // xR mod m
    public function convert (x) {
      var r = BigInteger.nbi();
      x.abs().dlShiftTo(this.m.t,r);
      r.divRemTo(this.m,null,r);
      if(x.s < 0 && r.compareTo(BigInteger.ZERO) > 0) this.m.subTo(r,r);
      return r;
    }
    
    // x/R mod m
    public function revert(x) {
      var r = BigInteger.nbi();
      x.copyTo(r);
      this.reduce(r);
      return r;
    }
    
    // x = x/R mod m (HAC 14.32)
    public function reduce(x) {
      var x_array = x.array;
      while(x.t <= this.mt2)  // pad x so am has enough room later
        x_array[x.t++] = 0;
      for(var i = 0; i < this.m.t; ++i) {
        // faster way of calculating u0 = x[i]*mp mod DV
        var j = x_array[i]&0x7fff;
        var u0 = (j*this.mpl+(((j*this.mph+(x_array[i]>>15)*this.mpl)&this.um)<<15))&BigInteger.BI_DM;
        // use am to combine the multiply-shift-add into one call
        j = i+this.m.t;
        x_array[j] += this.m.am(0,u0,x,i,0,this.m.t);
        // propagate carry
        while(x_array[j] >= BigInteger.BI_DV) { x_array[j] -= BigInteger.BI_DV; x_array[++j]++; }
      }
      x.clamp();
      x.drShiftTo(this.m.t,x);
      if(x.compareTo(this.m) >= 0) x.subTo(this.m,x);
    }
    
    // r = "x^2/R mod m"; x != r
    public function sqrTo(x,r) { x.squareTo(r); this.reduce(r); }
    
    // r = "xy/R mod m"; x,y != r
    public function mulTo(x,y,r) {
      x.multiplyTo(y,r); 
      this.reduce(r); 
    }

  } // class Montgomery
  
  // Barrett modular reduction
  public class Barrett implements ModularReduction {
    var m ;
    var mu ;
    var r2 ;
    var q3 ;
    
    public function Barrett(m) {
      // setup Barrett
      this.r2 = BigInteger.nbi();
      this.q3 = BigInteger.nbi();
      BigInteger.ONE.dlShiftTo(2*m.t,this.r2);
      this.mu = this.r2.divide(m);
      this.m = m;
    }
    
    public function convert(x) {
      if(x.s < 0 || x.t > 2*this.m.t) return x.mod(this.m);
      else if(x.compareTo(this.m) < 0) return x;
      else { var r = BigInteger.nbi(); x.copyTo(r); this.reduce(r); return r; }
    }
    
    public function revert(x) { return x; }
    
    // x = x mod m (HAC 14.42)
    public function reduce(x) {
      x.drShiftTo(this.m.t-1,this.r2);
      if(x.t > this.m.t+1) { x.t = this.m.t+1; x.clamp(); }
      this.mu.multiplyUpperTo(this.r2,this.m.t+1,this.q3);
      this.m.multiplyLowerTo(this.q3,this.m.t+1,this.r2);
      while(x.compareTo(this.r2) < 0) x.dAddOffset(1,this.m.t+1);
      x.subTo(this.r2,x);
      while(x.compareTo(this.m) >= 0) x.subTo(this.m,x);
    }
    
    // r = x^2 mod m; x != r
    public function sqrTo(x,r) { x.squareTo(r); this.reduce(r); }
    
    // r = x*y mod m; x,y != r
    public function mulTo(x,y,r) {
      x.multiplyTo(y,r);
      this.reduce(r);
    }

  } // class Barrett
  
  public class ArcFour {
    var i ;
    var j ;
    var S ;
    
    public function ArcFour() {
      i = 0;
      j = 0;
      S = new Array();
    }
    
    /**
    Initialize arcfour context from key, an array of ints,
    each from [0..255]. An array of bytes the size of the
    pool must be passed to init()
    **/
    public function init(key )  {
      var i, j, t;
      for(i = 0; i < 256; ++i)
        this.S[i] = i;
      j = 0;
      for(i = 0; i < 256; ++i) {
        j = (j + this.S[i] + key[i % key.length]) & 255;
        t = this.S[i];
        this.S[i] = this.S[j];
        this.S[j] = t;
      }
      this.i = 0;
      this.j = 0;
    }
    
    /**
    Returns the next byte.
    **/
    public function next()  {
      var t;
      this.i = (this.i + 1) & 255;
      this.j = (this.j + this.S[this.i]) & 255;
      t = this.S[this.i];
      this.S[this.i] = this.S[this.j];
      this.S[this.j] = t;
      return this.S[(t + this.S[this.i]) & 255];
    }
    
    
    public function toString()  {
      return "ArcFour";
    }
}





public class SecureRandom {
  private static var rng_state;
  private static var rng_pool;
  private static var rng_pptr;
  // Pool size must be a multiple of 4 and greater than 32.
  // An array of bytes the size of the pool will be passed to init()
  private static var rng_psize = 256;
  private static var initialized=false;
  
  public function SecureRandom() {
    // Initialize the pool with junk if needed.
    if (!initialized) {
      if(rng_pool == null) {
        rng_pool = new Array();
        rng_pptr = 0;
        var t;
        while(rng_pptr < rng_psize) {  // extract some randomness from Math.random()
          t = Math.floor(65536 * Math.random());
          rng_pool[rng_pptr++] = t >>> 8;
          rng_pool[rng_pptr++] = t & 255;
        }
        rng_pptr = 0;
        rng_seed_time();
        //rng_seed_int(window.screenX);
        //rng_seed_int(window.screenY);
        initialized = true;
      }
    }
  }
  
  // rng_get_bytes
  public function nextBytes(ba) {
    var i;
    for(i = 0; i < ba.length; ++i) 
      ba[i] = rng_get_byte();
      
  }
  // Plug in your RNG constructor here
  private function prng_newstate() {
    return new ArcFour();
  }

  // Mix in a 32-bit integer into the pool
  private function rng_seed_int(x) {
    rng_pool[rng_pptr++] ^= x & 255;
    rng_pool[rng_pptr++] ^= (x >> 8) & 255;
    rng_pool[rng_pptr++] ^= (x >> 16) & 255;
    rng_pool[rng_pptr++] ^= (x >> 24) & 255;
    if(rng_pptr >= rng_psize) rng_pptr -= rng_psize;
  }
  
  // Mix in the current time (w/milliseconds) into the pool
  private function rng_seed_time() {
    rng_seed_int(new Date().getTime());
  }
  
  private function rng_get_byte() {
    if(rng_state == null) {
      rng_seed_time();
      rng_state = prng_newstate();
      rng_state.init(rng_pool);
      for(rng_pptr = 0; rng_pptr < rng_pool.length; ++rng_pptr)
        rng_pool[rng_pptr] = 0;
      rng_pptr = 0;
      //rng_pool = null;
    }
    // TODO: allow reseeding after first request
    return rng_state.next();
  }
  
} // class SecureRandom


// Depends on jsbn.js and rng.js



//cpeyer - not used
/*
function linebrk(s,n) {
  var ret = "";
  var i = 0;
  while(i + n < s.length) {
    ret += s.substring(i,i+n) + "\n";
    i += n;
  }
  return ret + s.substring(i,s.length);
}
*/

//cpeyer - not used
/*
function byte2Hex(b) {
  if(b < 0x10)
    return "0" + b.toString(16);
  else
    return b.toString(16);
}
*/





public class RSAKey {
  var n ;
  var e ;
  var d ;
  var p ;
  var q ;
  var dmp1 ;
  var dmq1 ;
  var coeff ;
  
  // "empty" RSA key constructor
  public function RSAKey() {
    this.n = null;
    this.e = 0;
    this.d = null;
    this.p = null;
    this.q = null;
    this.dmp1 = null;
    this.dmq1 = null;
    this.coeff = null;
  }
  
  // Set the public key fields N and e from hex strings
  // RSASetPublic
  public function setPublic(N,E) {
    if(N != null && E != null && N.length > 0 && E.length > 0) {
      this.n = parseBigInt(N,16);
      this.e = parseInt(E,16);
    }
    else
      //alert("Invalid RSA public key");
      trace("Invalid RSA public key");
  }
  
  // PKCS#1 (type 2, random) pad input string s to n bytes, and return a bigint
  function pkcs1pad2(s,n) {
    if(n < s.length + 11) {
      alert("Message too long for RSA");
      return null;
    }
    var ba = new Array();
    var i = s.length - 1;
    while(i >= 0 && n > 0) ba[--n] = s.charCodeAt(i--);
    ba[--n] = 0;
    var rng = new SecureRandom();
    var x = new Array();
    while(n > 2) { // random non-zero pad
      x[0] = 0;
      while(x[0] == 0) rng.nextBytes(x);
      ba[--n] = x[0];
    }
    ba[--n] = 2;
    ba[--n] = 0;
    return new BigInteger(ba);
  }

  
  // convert a (hex) string to a bignum object
  private function parseBigInt(str,r) {
    return new BigInteger(str,r);
  }
  
  // Perform raw public operation on "x": return x^e (mod n)
  // RSADoPublic
  public function doPublic(x) {
    return x.modPowInt(this.e, this.n);
  }
  
  // Return the PKCS#1 RSA encryption of "text" as an even-length hex string
  // RSAEncrypt
  public function encrypt(text) {
    var m = pkcs1pad2(text,(this.n.bitLength()+7)>>3);
    if(m == null) return null;
    var c = this.doPublic(m);
    if(c == null) return null;
    var h = c.toString(16);
    if((h.length & 1) == 0) return h; else return "0" + h;
  }
  
  // Return the PKCS#1 RSA encryption of "text" as a Base64-encoded string
  //function RSAEncryptB64(text) {
  //  var h = this.encrypt(text);
  //  if(h) return hex2b64(h); else return null;
  //}
  //RSAKey.prototype.encrypt_b64 = RSAEncryptB64;
  // Depends on rsa.js and jsbn2.js
  
  // Undo PKCS#1 (type 2, random) padding and, if valid, return the plaintext
  private function pkcs1unpad2(d,n) {
    var b = d.toByteArray();
    var i = 0;
    while(i < b.length && b[i] == 0) ++i;
    if(b.length-i != n-1 || b[i] != 2)
      return null;
    ++i;
    while(b[i] != 0)
      if(++i >= b.length) return null;
    var ret = "";
    while(++i < b.length)
      ret += String.fromCharCode(b[i]);
    return ret;
  }
  
  // Set the private key fields N, e, and d from hex strings
  // RSASetPrivate
  public function setPrivate(N,E,D) {
    if(N != null && E != null && N.length > 0 && E.length > 0) {
      this.n = parseBigInt(N,16);
      this.e = parseInt(E,16);
      this.d = parseBigInt(D,16);
    }
    else
      trace("Invalid RSA private key");
  }
  
  // Set the private key fields N, e, d and CRT params from hex strings
  // RSASetPrivateEx
  public function setPrivateEx(N,E,D,P,Q,DP,DQ,C) {
    if(N != null && E != null && N.length > 0 && E.length > 0) {
      this.n = parseBigInt(N,16);
      this.e = parseInt(E,16);
      this.d = parseBigInt(D,16);
      this.p = parseBigInt(P,16);
      this.q = parseBigInt(Q,16);
      this.dmp1 = parseBigInt(DP,16);
      this.dmq1 = parseBigInt(DQ,16);
      this.coeff = parseBigInt(C,16);
    }
    else
      trace("Invalid RSA private key");
  }
  
  // Generate a new random private key B bits long, using public expt E
  // RSAGenerate
  public function generate(B,E) {
    var rng = new SecureRandom();
    var qs = B>>1;
    this.e = parseInt(E,16);
    var ee = new BigInteger(E,16);
    for(;;) {
      for(;;) {
        this.p = new BigInteger(B-qs,1,rng);
        if(this.p.subtract(BigInteger.ONE).gcd(ee).compareTo(BigInteger.ONE) == 0 && this.p.isProbablePrime(10)) break;
      }
      for(;;) {
        this.q = new BigInteger(qs,1,rng);
        if(this.q.subtract(BigInteger.ONE).gcd(ee).compareTo(BigInteger.ONE) == 0 && this.q.isProbablePrime(10)) break;
      }
      if(this.p.compareTo(this.q) <= 0) {
        var t = this.p;
        this.p = this.q;
        this.q = t;
      }
      var p1 = this.p.subtract(BigInteger.ONE);
      var q1 = this.q.subtract(BigInteger.ONE);
      var phi = p1.multiply(q1);
      if(phi.gcd(ee).compareTo(BigInteger.ONE) == 0) {
        this.n = this.p.multiply(this.q);
        this.d = ee.modInverse(phi);
        this.dmp1 = this.d.mod(p1);
        this.dmq1 = this.d.mod(q1);
        this.coeff = this.q.modInverse(this.p);
        break;
      }
    }
  }
  
  // Perform raw private operation on "x": return x^d (mod n)
  // RSADoPrivate
  public function doPrivate(x) {
    if(this.p == null || this.q == null)
      return x.modPow(this.d, this.n);
  
    // TODO: re-calculate any missing CRT params
    var xp = x.mod(this.p).modPow(this.dmp1, this.p);
    var xq = x.mod(this.q).modPow(this.dmq1, this.q);
  
    while(xp.compareTo(xq) < 0)
      xp = xp.add(this.p);
    return xp.subtract(xq).multiply(this.coeff).mod(this.p).multiply(this.q).add(xq);
  }
  
  // Return the PKCS#1 RSA decryption of "ctext".
  // "ctext" is an even-length hex string and the output is a plain string.
  // RSADecrypt
  function decrypt(ctext) {
    var c = parseBigInt(ctext, 16);
    var m = this.doPrivate(c);
    if(m == null) return null;
    return pkcs1unpad2(m, (this.n.bitLength()+7)>>3);
  }
  
} // class RSAKey 


var nValue="a5261939975948bb7a58dffe5ff54e65f0498f9175f5a09288810b8975871e99af3b5dd94057b0fc07535f5f97444504fa35169d461d0d30cf0192e307727c065168c788771c561a9400fb49175e9e6aa4e23fe11af69e9412dd23b0cb6684c4c2429bce139e848ab26d0829073351f4acd36074eafd036a5eb83359d2a698d3";
var eValue="10001";
var dValue="8e9912f6d3645894e8d38cb58c0db81ff516cf4c7e5a14c7f1eddb1459d2cded4d8d293fc97aee6aefb861859c8b6a3d1dfe710463e1f9ddc72048c09751971c4a580aa51eb523357a3cc48d31cfad1d4a165066ed92d4748fb6571211da5cb14bc11b6e2df7c1a559e6d5ac1cd5c94703a22891464fba23d0d965086277a161";
var pValue="d090ce58a92c75233a6486cb0a9209bf3583b64f540c76f5294bb97d285eed33aec220bde14b2417951178ac152ceab6da7090905b478195498b352048f15e7d";
var qValue="cab575dc652bb66df15a0359609d51d1db184750c00c6698b90ef3465c99655103edbf0d54c56aec0ce3c4d22592338092a126a0cc49f65a4a30d222b411e58f";
var dmp1Value="1a24bca8e273df2f0e47c199bbf678604e7df7215480c77c8db39f49b000ce2cf7500038acfff5433b7d582a01f1826e6f4d42e1c57f5e1fef7b12aabc59fd25";
var dmq1Value="3d06982efbbe47339e1f6d36b1216b8a741d410b0c662f54f7118b27b9a4ec9d914337eb39841d8666f3034408cf94f5b62f11c402fc994fe15a05493150d9fd";
var coeffValue="3a3e731acd8960b7ff9eb81a7ff93bd1cfa74cbd56987db58b4594fb09c09084db1734c8143f98b602b981aaa9243ca28deb69b5b280ee8dcee0fd2625e53250";


var startTime = new Date();
// am3/28 is best for SM, Rhino, but am4/26 is best for v8.
// Kestrel (Opera 9.5) gets its best result with am4/26.
// IE7 does 9% better with am3/28 than with am4/26.
// Firefox (SM) gets 10% faster with am3/28 than with am4/26.
BigInteger.setupEngine(3, 28);
var x = new BigInteger();
var RSA = new RSAKey();
var TEXT = "The quick brown fox jumped over the extremely lazy frogs!";

RSA.setPublic(nValue, eValue);
RSA.setPrivateEx(nValue, eValue, dValue, pValue, qValue, dmp1Value, dmq1Value, coeffValue);

// The code has been adapted for use as a benchmark by Google.

// verify that test has correct results before running
testTxt = "This is a test of the emergency broadcast system";
if (RSA.decrypt(RSA.encrypt(testTxt)) == testTxt) {
  print("Correct");
} else {
  trace('Error with RSA encryption.  Test Failed.');
}

var totalTime = new Date() - startTime;
print("Total time: " + totalTime);

function encrypt() {
  return RSA.encrypt(TEXT);
}

function decrypt() {
  return RSA.decrypt(TEXT);
}

}// package
