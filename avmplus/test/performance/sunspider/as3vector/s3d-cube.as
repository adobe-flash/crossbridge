/*
 Copyright (C) 2007 Apple Inc.  All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
// 3D Cube Rotation
// http://www.speich.net/computer/moztesting/3d.htm
// Created by Simon Speich

package {
  public class DisplArea {
    public static var Width:int = 300;
    public static var Height:int = 300;
  }
  
  public class Origin {
    public static var V:Vector.<Number> = new Vector.<Number>(4);
  }
  
  public class Testing {
    public static var LoopCount:int = 0;
    public static var LoopMax:int = 50;
    public static var TimeMax:int = 0;
    public static var TimeAvg:Number = 0;
    public static var TimeMin:int = 0;
    public static var TimeTemp:Number = 0;
    public static var TimeTotal:Number = 0;
    public static var Init:Boolean = false;
    }

  public class Point {
    public var V:Vector.<Number>;
    public function Point(X:Number,Y:Number,Z:Number):void {
      this.V=Vector.<Number>([X,Y,Z,1]);
    }
  }

  public class Qt {
    public var elements:Vector.<Point>;
    public var LastPx:Number;
    public var NumPx:Number;
    public var Normal:Vector.<Vector.<Number>>;
    public var Line:Vector.<Boolean>;
    public var Edge:Vector.<Vector.<Number>>;
    public function Qt() {
      elements=new Vector.<Point>(4);
    }
  }

  public class Cube {
    public var Q:Qt = new Qt();
    var MTrans:Vector.<Vector.<Number>>;  // transformation matrix
    var MQube:Vector.<Vector.<Number>>;  // position information of qube
    var I:Vector.<Vector.<Number>>;      // entity matrix
    var LoopTimer:Number;

  

  public function DrawLine(From:Point, To:Point):void {
    //print("DrawLine "+new Date());
    var x1:Number = From.V[0];
    var x2:Number = To.V[0];
    var y1:Number = From.V[1];
    var y2:Number = To.V[1];
    var dx:Number = Math.abs(x2 - x1);
    var dy:Number = Math.abs(y2 - y1);
    var x:Number = x1;
    var y:Number = y1;
    var IncX1:Number, IncY1:Number;
    var IncX2:Number, IncY2:Number;
    var Den:Number;
    var Num:Number;
    var NumAdd:Number;
    var NumPix:Number;

    if (x2 >= x1) {  IncX1 = 1; IncX2 = 1;  }
    else { IncX1 = -1; IncX2 = -1; }
    if (y2 >= y1)  {  IncY1 = 1; IncY2 = 1; }
    else { IncY1 = -1; IncY2 = -1; }
    if (dx >= dy) {
      IncX1 = 0;
      IncY2 = 0;
      Den = dx;
      Num = dx / 2;
      NumAdd = dy;
      NumPix = dx;
    }
    else {
      IncX2 = 0;
      IncY1 = 0;
      Den = dy;
      Num = dy / 2;
      NumAdd = dx;
      NumPix = dy;
    }

    NumPix = Math.round(Q.LastPx + NumPix);

    var i:int = Q.LastPx;
    for (; i < NumPix; i++) {
      Num += NumAdd;
      if (Num >= Den) {
        Num -= Den;
        x += IncX1;
        y += IncY1;
      }
      x += IncX2;
      y += IncY2;
    }
    Q.LastPx = NumPix;
  }

  public function CalcCross(V0:Vector.<Number>, V1:Vector.<Number>):Vector.<Number> {
    //print("CalcCross "+new Date());
    var Cross = new Vector.<Number>(4);
    Cross[0] = V0[1]*V1[2] - V0[2]*V1[1];
    Cross[1] = V0[2]*V1[0] - V0[0]*V1[2];
    Cross[2] = V0[0]*V1[1] - V0[1]*V1[0];
    return Cross;
  }

  public function CalcNormal(V0:Vector.<Number>, V1:Vector.<Number>, V2:Vector.<Number>):Vector.<Number> {
    //print("CalcNormal "+new Date());
    var A:Vector.<Number> = new Vector.<Number>(4);   var B:Vector.<Number> = new Vector.<Number>(4);
    var i:int;
    for (i = 0; i < 3; i++) {
      A[i] = V0[i] - V1[i];
      B[i] = V2[i] - V1[i];
    }
    A = CalcCross(A, B);
    var Length:Number = Math.sqrt(A[0]*A[0] + A[1]*A[1] + A[2]*A[2]);
    for (i = 0; i < 3; i++) A[i] = A[i] / Length;
    A[3] = 1;
    return A;
  }

  // multiplies two matrices
  public function MMulti(M1:Vector.<Vector.<Number>>, M2:Vector.<Vector.<Number>>):Vector.<Vector.<Number>> {
    //print("MMulti "+new Date());
    var M:Vector.<Vector.<Number>> = new Vector.<Vector.<Number>>(4);
    M[0]=Vector.<Number>([0,0,0,0]);
    M[1]=Vector.<Number>([0,0,0,0]);
    M[2]=Vector.<Number>([0,0,0,0]);
    M[3]=Vector.<Number>([0,0,0,0]);
    var i:int = 0;
    var j:int = 0;
    for (; i < 4; i++) {
      j = 0;
      for (; j < 4; j++) {
        M[i][j] = M1[i][0] * M2[0][j] + M1[i][1] * M2[1][j] + M1[i][2] * M2[2][j] + M1[i][3] * M2[3][j];
      }
    }
    return M;
  }

  //multiplies matrix with vector
  public function VMulti(M:Vector.<Vector.<Number>>, V:Vector.<Number>):Vector.<Number> {
    //print("VMulti "+new Date());
    var Vect:Vector.<Number> = new Vector.<Number>(4);
    var i:int = 0;
    for (;i < 4; i++) Vect[i] = M[i][0] * V[0] + M[i][1] * V[1] + M[i][2] * V[2] + M[i][3] * V[3];
    return Vect;
  }

  public function VMulti2(M:Vector.<Vector.<Number>>, V:Vector.<Number>):Vector.<Number> {
    //print("VMulti2 "+new Date());
    var Vect:Vector.<Number> = new Vector.<Number>(3);
    var i:int = 0;
    for (;i < 3; i++) Vect[i] = M[i][0] * V[0] + M[i][1] * V[1] + M[i][2] * V[2];
    return Vect;
  }

  // add to matrices
  public function MAdd(M1:Vector.<Vector.<Number>>, M2:Vector.<Vector.<Number>>):Vector.<Vector.<Number>> {
    //print("MAdd "+new Date());
    var M:Vector.<Vector.<Number>>=new Vector.<Vector.<Number>>(4);
    M[0]=Vector.<Number>;
    M[1]=Vector.<Number>;
    M[2]=Vector.<Number>;
    M[3]=Vector.<Number>;
    var i:int = 0;
    var j:int = 0;
    for (; i < 4; i++) {
      j = 0;
      for (; j < 4; j++) M[i][j] = M1[i][j] + M2[i][j];
    }
    return M;
  }

  public function Translate(M:Vector.<Vector.<Number>>, Dx:Number, Dy:Number, Dz:Number):Vector.<Vector.<Number>> {
    //print("Translate "+new Date());
    var T:Vector.<Vector.<Number>>=new Vector.<Vector.<Number>>(4);
    T[0]=Vector.<Number>([1,0,0,Dx]);
    T[1]=Vector.<Number>([0,1,0,Dy]);
    T[2]=Vector.<Number>([0,0,1,Dz]);
    T[3]=Vector.<Number>([0,0,0,1]);
    return MMulti(T, M);
  }

  public function RotateX(M:Vector.<Vector.<Number>>, Phi:Number):Vector.<Vector.<Number>> {
    //print("RotateX "+new Date());
    var a:Number = Phi;
    a *= Math.PI / 180;
    var Cos:Number = Math.cos(a);
    var Sin:Number = Math.sin(a);
    var R:Vector.<Vector.<Number>> = new Vector.<Vector.<Number>>(4);
    R[0]=Vector.<Number>([1,0,0,0]);
    R[1]=Vector.<Number>([0,Cos,-Sin,0]);
    R[2]=Vector.<Number>([0,Sin,Cos,0]);
    R[3]=Vector.<Number>([0,0,0,1]);
    return MMulti(R, M);
  }

  public function RotateY(M:Vector.<Vector.<Number>>, Phi:Number):Vector.<Vector.<Number>> {
    //print("RotateY "+new Date());
    var a:Number = Phi;
    a *= Math.PI / 180;
    var Cos:Number = Math.cos(a);
    var Sin:Number = Math.sin(a);
    var R:Vector.<Vector.<Number>> = new Vector.<Vector.<Number>>(4);
    R[0]=Vector.<Number>([Cos,0,Sin,0]);
    R[1]=Vector.<Number>([0,1,0,0]);
    R[2]=Vector.<Number>([-Sin,0,Cos,0]);
    R[3]=Vector.<Number>([0,0,0,1]);
    return MMulti(R, M);
  }

  public function RotateZ(M:Vector.<Vector.<Number>>, Phi:Number):Vector.<Vector.<Number>> {
    //print("RotateZ "+new Date());
    var a:Number = Phi;
    a *= Math.PI / 180;
    var Cos:Number = Math.cos(a);
    var Sin:Number = Math.sin(a);
    var R:Vector.<Vector.<Number>> = new Vector.<Vector.<Number>>(4);
    R[0]=Vector.<Number>([Cos,-Sin,0,0]);
    R[1]=Vector.<Number>([Sin,Cos,0,0]);
    R[2]=Vector.<Number>([0,0,1,0]);
    R[3]=Vector.<Number>([0,0,0,1]);
    return MMulti(R, M);
  }

  public function DrawQube():void {
    //print("DrawCube "+new Date());
    // calc current normals
    var CurN:Vector.<Vector.<Number>> = new Vector.<Vector.<Number>>(5);
    var i:int = 5;
    Q.LastPx = 0;
    for (; i > -1; i--) CurN[i] = VMulti2(MQube, Q.Normal[i]);
    if (CurN[0][2] < 0) {
      if (!Q.Line[0]) { DrawLine(Q.elements[0], Q.elements[1]); Q.Line[0] = true; };
      if (!Q.Line[1]) { DrawLine(Q.elements[1], Q.elements[2]); Q.Line[1] = true; };
      if (!Q.Line[2]) { DrawLine(Q.elements[2], Q.elements[3]); Q.Line[2] = true; };
      if (!Q.Line[3]) { DrawLine(Q.elements[3], Q.elements[0]); Q.Line[3] = true; };
    }
    if (CurN[1][2] < 0) {
      if (!Q.Line[2]) { DrawLine(Q.elements[3], Q.elements[2]); Q.Line[2] = true; };
      if (!Q.Line[9]) { DrawLine(Q.elements[2], Q.elements[6]); Q.Line[9] = true; };
      if (!Q.Line[6]) { DrawLine(Q.elements[6], Q.elements[7]); Q.Line[6] = true; };
      if (!Q.Line[10]) { DrawLine(Q.elements[7], Q.elements[3]); Q.Line[10] = true; };
    }
    if (CurN[2][2] < 0) {
      if (!Q.Line[4]) { DrawLine(Q.elements[4], Q.elements[5]); Q.Line[4] = true; };
      if (!Q.Line[5]) { DrawLine(Q.elements[5], Q.elements[6]); Q.Line[5] = true; };
      if (!Q.Line[6]) { DrawLine(Q.elements[6], Q.elements[7]); Q.Line[6] = true; };
      if (!Q.Line[7]) { DrawLine(Q.elements[7], Q.elements[4]); Q.Line[7] = true; };
    }
    if (CurN[3][2] < 0) {
      if (!Q.Line[4]) { DrawLine(Q.elements[4], Q.elements[5]); Q.Line[4] = true; };
      if (!Q.Line[8]) { DrawLine(Q.elements[5], Q.elements[1]); Q.Line[8] = true; };
      if (!Q.Line[0]) { DrawLine(Q.elements[1], Q.elements[0]); Q.Line[0] = true; };
      if (!Q.Line[11]) { DrawLine(Q.elements[0], Q.elements[4]); Q.Line[11] = true; };
    }
    if (CurN[4][2] < 0) {
      if (!Q.Line[11]) { DrawLine(Q.elements[4], Q.elements[0]); Q.Line[11] = true; };
      if (!Q.Line[3]) { DrawLine(Q.elements[0], Q.elements[3]); Q.Line[3] = true; };
      if (!Q.Line[10]) { DrawLine(Q.elements[3], Q.elements[7]); Q.Line[10] = true; };
      if (!Q.Line[7]) { DrawLine(Q.elements[7], Q.elements[4]); Q.Line[7] = true; };
    }
    if (CurN[5][2] < 0) {
      if (!Q.Line[8]) { DrawLine(Q.elements[1], Q.elements[5]); Q.Line[8] = true; };
      if (!Q.Line[5]) { DrawLine(Q.elements[5], Q.elements[6]); Q.Line[5] = true; };
      if (!Q.Line[9]) { DrawLine(Q.elements[6], Q.elements[2]); Q.Line[9] = true; };
      if (!Q.Line[1]) { DrawLine(Q.elements[2], Q.elements[1]); Q.Line[1] = true; };
    }
    Q.Line = Vector.<Boolean>([false,false,false,false,false,false,false,false,false,false,false,false]);
    Q.LastPx = 0;
  }

  public function Loop():void {
    if (Testing.LoopCount > Testing.LoopMax) return;
    var TestingStr:String = String(Testing.LoopCount);
    while (TestingStr.length < 3) TestingStr = "0" + TestingStr;
    MTrans = Translate(I, -Q.elements[8].V[0], -Q.elements[8].V[1], -Q.elements[8].V[2]);
    MTrans = RotateX(MTrans, 1);
    MTrans = RotateY(MTrans, 3);
    MTrans = RotateZ(MTrans, 5);
    MTrans = Translate(MTrans, Q.elements[8].V[0], Q.elements[8].V[1], Q.elements[8].V[2]);
    MQube = MMulti(MTrans, MQube);
    var i:int = 8;
    for (; i > -1; i--) {
      Q.elements[i].V = VMulti(MTrans, Q.elements[i].V);
    }
    DrawQube();
    Testing.LoopCount++;
    Loop();
  }

  public function Init(CubeSize:Number):void {
    //print("Init "+new Date());
    // init/reset vars
    Origin.V = Vector.<Number>([150,150,20,1]);
    Testing.LoopCount = 0;
    Testing.LoopMax = 50;
    Testing.TimeMax = 0;
    Testing.TimeAvg = 0;
    Testing.TimeMin = 0;
    Testing.TimeTemp = 0;
    Testing.TimeTotal = 0;
    Testing.Init = false;

    // transformation matrix
    MTrans = new Vector.<Vector.<Number>>(4);
    MTrans[0]=Vector.<Number>([1,0,0,0]);
    MTrans[1]=Vector.<Number>([0,1,0,0]);
    MTrans[2]=Vector.<Number>([0,0,1,0]);
    MTrans[3]=Vector.<Number>([0,0,0,1]);
    
    // position information of qube
    MQube = new Vector.<Vector.<Number>>(4);
    MQube[0]=Vector.<Number>([1,0,0,0]);
    MQube[1]=Vector.<Number>([0,1,0,0]);
    MQube[2]=Vector.<Number>([0,0,1,0]);
    MQube[3]=Vector.<Number>([0,0,0,1]);
    
    // entity matrix
    I = new Vector.<Vector.<Number>>(4);
    I[0]=Vector.<Number>([1,0,0,0]);
    I[1]=Vector.<Number>([0,1,0,0]);
    I[2]=Vector.<Number>([0,0,1,0]);
    I[3]=Vector.<Number>([0,0,0,1]);
    
    // create qube
    Q.elements[0] = new Point(-CubeSize,-CubeSize, CubeSize);
    Q.elements[1] = new Point(-CubeSize, CubeSize, CubeSize);
    Q.elements[2] = new Point( CubeSize, CubeSize, CubeSize);
    Q.elements[3] = new Point( CubeSize,-CubeSize, CubeSize);
    Q.elements[4] = new Point(-CubeSize,-CubeSize,-CubeSize);
    Q.elements[5] = new Point(-CubeSize, CubeSize,-CubeSize);
    Q.elements[6] = new Point( CubeSize, CubeSize,-CubeSize);
    Q.elements[7] = new Point( CubeSize,-CubeSize,-CubeSize);
    // center of gravity
    Q.elements[8] = new Point(0, 0, 0);
    
    // anti-clockwise edge check
    Q.Edge = new Vector.<Vector.<Number>>(6);
    Q.Edge[0]=Vector.<Number>([0,1,2]);
    Q.Edge[1]=Vector.<Number>([3,2,6]);
    Q.Edge[2]=Vector.<Number>([7,6,5]);
    Q.Edge[3]=Vector.<Number>([4,5,1]);
    Q.Edge[4]=Vector.<Number>([4,0,3]);
    Q.Edge[5]=Vector.<Number>([1,5,6]);
    
    // calculate squad normals
    Q.Normal = new Vector.<Vector.<Number>>(6);
    var i:int;
   
    for (i = 0; i < Q.Edge.length; i++) {
       Q.Normal[i] = CalcNormal(Q.elements[Q.Edge[i][0]].V, Q.elements[Q.Edge[i][1]].V, Q.elements[Q.Edge[i][2]].V);
    }
    
    // line drawn ?
    Q.Line = Vector.<Boolean>([false,false,false,false,false,false,false,false,false,false,false,false]);
    
    // create line pixels
    Q.NumPx = 9 * 2 * CubeSize;
    for (i = 0; i < Q.NumPx; i++) new Point(0,0,0);
    
    MTrans = Translate(MTrans, Origin.V[0], Origin.V[1], Origin.V[2]);
    MQube = MMulti(MTrans, MQube);

    var i:int = 0;
    for (; i < 9; i++) {
      Q.elements[i].V = VMulti(MTrans, Q.elements[i].V);
    }
    DrawQube();
    Testing.Init = true;
    Loop();
  }
  }
}
function run3dCube():int {
  if (CONFIG::desktop)
      var _sunSpiderStartDate:Number = new Date();
  else // mobile
      var _sunSpiderStartDate:int = getTimer();

  var cube:Cube=new Cube();
  var i:int;
  for (i = 20; i <= 160; i *= 2 ) {
    cube.Init(i);
  }

  if (CONFIG::desktop)
      var _sunSpiderInterval:Number = new Date() - _sunSpiderStartDate;
  else // mobile
      var _sunSpiderInterval:int = getTimer() - _sunSpiderStartDate;
  
  
  // verify test results
  var expectedResults = [250.49814997925202, 308.02382919560387, -184.27577256519325];
  for (i=0; i<3; i++) {
    if (Math.abs(cube.Q.elements[5].V[i] - expectedResults[i])>0.00001) {
        print("Test validation failed.  Q[5].V["+i+"]: Expected: "+expectedResults[i]+" Got: "+cube.Q.elements[5].V[i]);
    }
  }
  
  print("metric time "+_sunSpiderInterval);
}

run3dCube();


