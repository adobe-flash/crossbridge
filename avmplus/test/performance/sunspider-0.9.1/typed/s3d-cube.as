// 3D Cube Rotation
// http://www.speich.net/computer/moztesting/3d.htm
// Created by Simon Speich
package {

var Q:Array = new Array();
var MTrans:Array = new Array();  // transformation matrix
var MQube:Array = new Array();  // position information of qube
var I:Array = new Array();      // entity matrix
var Origin:Object = new Object();
var Testing:Object = new Object();
var LoopTimer:uint;

var DisplArea:Object = new Object();
DisplArea.Width = 300;
DisplArea.Height = 300;

function DrawLine(From:Object, To:Object):void {
  var x1:int = From.V[0];
  var x2:int = To.V[0];
  var y1:int = From.V[1];
  var y2:int = To.V[1];
  var dx:int = Math.abs(x2 - x1);
  var dy:int = Math.abs(y2 - y1);
  var x:int = x1;
  var y:int = y1;
  var IncX1:int, IncY1:int;
  var IncX2:int, IncY2:int;
  var Den:int;
  var Num:int;
  var NumAdd:int;
  var NumPix:int;

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

function CalcCross(V0:Array, V1:Array):Array {
  var Cross:Array = new Array();
  Cross[0] = V0[1]*V1[2] - V0[2]*V1[1];
  Cross[1] = V0[2]*V1[0] - V0[0]*V1[2];
  Cross[2] = V0[0]*V1[1] - V0[1]*V1[0];
  return Cross;
}

function CalcNormal(V0:Array, V1:Array, V2:Array):Array {
  var A:Array = new Array();   var B:Array = new Array();
  for (var i:uint = 0; i < 3; i++) {
    A[i] = V0[i] - V1[i];
    B[i] = V2[i] - V1[i];
  }
  A = CalcCross(A, B);
  var Length = Math.sqrt(A[0]*A[0] + A[1]*A[1] + A[2]*A[2]);
  for (i = 0; i < 3; i++) A[i] = A[i] / Length;
  A[3] = 1;
  return A;
}

public class CreateP {
  var V:Array;
public function CreateP(X:int,Y:int,Z:int):void {
  this.V = [X,Y,Z,1];
}
}

// multiplies two matrices
function MMulti(M1:Array, M2:Array):Array {
  var M:Array = [[],[],[],[]];
  var i:uint = 0;
  var j:uint = 0;
  for (; i < 4; i++) {
    j = 0;
    for (; j < 4; j++) M[i][j] = M1[i][0] * M2[0][j] + M1[i][1] * M2[1][j] + M1[i][2] * M2[2][j] + M1[i][3] * M2[3][j];
  }
  return M;
}

//multiplies matrix with vector
function VMulti(M:Array, V:Array):Array {
  var Vect:Array = new Array();
  var i:uint = 0;
  for (;i < 4; i++) Vect[i] = M[i][0] * V[0] + M[i][1] * V[1] + M[i][2] * V[2] + M[i][3] * V[3];
  return Vect;
}

function VMulti2(M:Array, V:Array):Array {
  var Vect:Array = new Array();
  var i:uint = 0;
  for (;i < 3; i++) Vect[i] = M[i][0] * V[0] + M[i][1] * V[1] + M[i][2] * V[2];
  return Vect;
}

// add to matrices
function MAdd(M1:Array, M2:Array):Array {
  var M:Array = [[],[],[],[]];
  var i:uint = 0;
  var j:uint = 0;
  for (; i < 4; i++) {
    j = 0;
    for (; j < 4; j++) M[i][j] = M1[i][j] + M2[i][j];
  }
  return M;
}

function Translate(M:Array, Dx:int, Dy:int, Dz:int):Array {
  var T:Array = [
  [1,0,0,Dx],
  [0,1,0,Dy],
  [0,0,1,Dz],
  [0,0,0,1]
  ];
  return MMulti(T, M);
}

function RotateX(M:Array, Phi:Number):Array {
  var a:Number = Phi;
  a *= Math.PI / 180;
  var Cos:Number = Math.cos(a);
  var Sin:Number = Math.sin(a);
  var R:Array = [
  [1,0,0,0],
  [0,Cos,-Sin,0],
  [0,Sin,Cos,0],
  [0,0,0,1]
  ];
  return MMulti(R, M);
}

function RotateY(M:Array, Phi:Number):Array {
  var a:Number = Phi;
  a *= Math.PI / 180;
  var Cos:Number = Math.cos(a);
  var Sin:Number = Math.sin(a);
  var R:Array = [
  [Cos,0,Sin,0],
  [0,1,0,0],
  [-Sin,0,Cos,0],
  [0,0,0,1]
  ];
  return MMulti(R, M);
}

function RotateZ(M:Array, Phi:Number):Array {
  var a:Number = Phi;
  a *= Math.PI / 180;
  var Cos:Number = Math.cos(a);
  var Sin:Number = Math.sin(a);
  var R:Array = [
  [Cos,-Sin,0,0],
  [Sin,Cos,0,0],
  [0,0,1,0],
  [0,0,0,1]
  ];
  return MMulti(R, M);
}

function DrawQube():void {
  // calc current normals
  var CurN:Array = new Array();
  var i:int = 5;
  Q.LastPx = 0;
  for (; i > -1; i--) CurN[i] = VMulti2(MQube, Q.Normal[i]);
  if (CurN[0][2] < 0) {
    if (!Q.Line[0]) { DrawLine(Q[0], Q[1]); Q.Line[0] = true; };
    if (!Q.Line[1]) { DrawLine(Q[1], Q[2]); Q.Line[1] = true; };
    if (!Q.Line[2]) { DrawLine(Q[2], Q[3]); Q.Line[2] = true; };
    if (!Q.Line[3]) { DrawLine(Q[3], Q[0]); Q.Line[3] = true; };
  }
  if (CurN[1][2] < 0) {
    if (!Q.Line[2]) { DrawLine(Q[3], Q[2]); Q.Line[2] = true; };
    if (!Q.Line[9]) { DrawLine(Q[2], Q[6]); Q.Line[9] = true; };
    if (!Q.Line[6]) { DrawLine(Q[6], Q[7]); Q.Line[6] = true; };
    if (!Q.Line[10]) { DrawLine(Q[7], Q[3]); Q.Line[10] = true; };
  }
  if (CurN[2][2] < 0) {
    if (!Q.Line[4]) { DrawLine(Q[4], Q[5]); Q.Line[4] = true; };
    if (!Q.Line[5]) { DrawLine(Q[5], Q[6]); Q.Line[5] = true; };
    if (!Q.Line[6]) { DrawLine(Q[6], Q[7]); Q.Line[6] = true; };
    if (!Q.Line[7]) { DrawLine(Q[7], Q[4]); Q.Line[7] = true; };
  }
  if (CurN[3][2] < 0) {
    if (!Q.Line[4]) { DrawLine(Q[4], Q[5]); Q.Line[4] = true; };
    if (!Q.Line[8]) { DrawLine(Q[5], Q[1]); Q.Line[8] = true; };
    if (!Q.Line[0]) { DrawLine(Q[1], Q[0]); Q.Line[0] = true; };
    if (!Q.Line[11]) { DrawLine(Q[0], Q[4]); Q.Line[11] = true; };
  }
  if (CurN[4][2] < 0) {
    if (!Q.Line[11]) { DrawLine(Q[4], Q[0]); Q.Line[11] = true; };
    if (!Q.Line[3]) { DrawLine(Q[0], Q[3]); Q.Line[3] = true; };
    if (!Q.Line[10]) { DrawLine(Q[3], Q[7]); Q.Line[10] = true; };
    if (!Q.Line[7]) { DrawLine(Q[7], Q[4]); Q.Line[7] = true; };
  }
  if (CurN[5][2] < 0) {
    if (!Q.Line[8]) { DrawLine(Q[1], Q[5]); Q.Line[8] = true; };
    if (!Q.Line[5]) { DrawLine(Q[5], Q[6]); Q.Line[5] = true; };
    if (!Q.Line[9]) { DrawLine(Q[6], Q[2]); Q.Line[9] = true; };
    if (!Q.Line[1]) { DrawLine(Q[2], Q[1]); Q.Line[1] = true; };
  }
  Q.Line = [false,false,false,false,false,false,false,false,false,false,false,false];
  Q.LastPx = 0;
}

function Loop():void {
  if (Testing.LoopCount > Testing.LoopMax) return;
  var TestingStr:String = String(Testing.LoopCount);
  while (TestingStr.length < 3) TestingStr = "0" + TestingStr;
  MTrans = Translate(I, -Q[8].V[0], -Q[8].V[1], -Q[8].V[2]);
  MTrans = RotateX(MTrans, 1);
  MTrans = RotateY(MTrans, 3);
  MTrans = RotateZ(MTrans, 5);
  MTrans = Translate(MTrans, Q[8].V[0], Q[8].V[1], Q[8].V[2]);
  MQube = MMulti(MTrans, MQube);
  var i:int = 8;
  for (; i > -1; i--) {
    Q[i].V = VMulti(MTrans, Q[i].V);
  }
  DrawQube();
  Testing.LoopCount++;
  Loop();
}

function Init(CubeSize:int):void {
  // init/reset vars
  Origin.V = [150,150,20,1];
  Testing.LoopCount = 0;
  Testing.LoopMax = 50;
  Testing.TimeMax = 0;
  Testing.TimeAvg = 0;
  Testing.TimeMin = 0;
  Testing.TimeTemp = 0;
  Testing.TimeTotal = 0;
  Testing.Init = false;

  // transformation matrix
  MTrans = [
  [1,0,0,0],
  [0,1,0,0],
  [0,0,1,0],
  [0,0,0,1]
  ];
  
  // position information of qube
  MQube = [
  [1,0,0,0],
  [0,1,0,0],
  [0,0,1,0],
  [0,0,0,1]
  ];
  
  // entity matrix
  I = [
  [1,0,0,0],
  [0,1,0,0],
  [0,0,1,0],
  [0,0,0,1]
  ];
  
  // create qube
  Q[0] = new CreateP(-CubeSize,-CubeSize, CubeSize);
  Q[1] = new CreateP(-CubeSize, CubeSize, CubeSize);
  Q[2] = new CreateP( CubeSize, CubeSize, CubeSize);
  Q[3] = new CreateP( CubeSize,-CubeSize, CubeSize);
  Q[4] = new CreateP(-CubeSize,-CubeSize,-CubeSize);
  Q[5] = new CreateP(-CubeSize, CubeSize,-CubeSize);
  Q[6] = new CreateP( CubeSize, CubeSize,-CubeSize);
  Q[7] = new CreateP( CubeSize,-CubeSize,-CubeSize);
  
  // center of gravity
  Q[8] = new CreateP(0, 0, 0);
  
  // anti-clockwise edge check
  Q.Edge = [[0,1,2],[3,2,6],[7,6,5],[4,5,1],[4,0,3],[1,5,6]];
  
  // calculate squad normals
  Q.Normal = new Array();
  for (var i:uint = 0; i < Q.Edge.length; i++) Q.Normal[i] = CalcNormal(Q[Q.Edge[i][0]].V, Q[Q.Edge[i][1]].V, Q[Q.Edge[i][2]].V);
  
  // line drawn ?
  Q.Line = [false,false,false,false,false,false,false,false,false,false,false,false];
  
  // create line pixels
  Q.NumPx = 9 * 2 * CubeSize;
  for (var i:uint = 0; i < Q.NumPx; i++) new CreateP(0,0,0);
  
  MTrans = Translate(MTrans, Origin.V[0], Origin.V[1], Origin.V[2]);
  MQube = MMulti(MTrans, MQube);

  var i:uint = 0;
  for (; i < 9; i++) {
    Q[i].V = VMulti(MTrans, Q[i].V);
  }
  DrawQube();
  Testing.Init = true;
  Loop();
}

// main entry point for running testcase
function runTest():void{
for ( var i:uint = 20; i <= 160; i *= 2 ) {
  Init(i);
}

Q = new Array();
MTrans = new Array();  // transformation matrix
MQube = new Array();  // position information of qube
I = new Array();      // entity matrix
Origin = new Object();
Testing = new Object();
DisplArea = new Object();
DisplArea.Width = 300;
DisplArea.Height = 300;
}

// warm up run of testcase
runTest();
var startTime:uint = new Date().getTime();
runTest();
var time:uint = new Date().getTime() - startTime;
print("metric time " + time);

} // package