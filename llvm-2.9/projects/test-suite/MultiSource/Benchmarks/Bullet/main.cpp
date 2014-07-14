/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2007 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "BenchmarkDemo.h"
#include "btBulletDynamicsCommon.h"
#include "LinearMath/btHashMap.h"
#include <stdio.h>

#ifdef USE_GLUT_DEMO_APPLICATION
	#include "GlutStuff.h"
	#include "GLDebugDrawer.h"
	GLDebugDrawer	gDebugDrawer;
#endif //USE_GLUT_DEMO_APPLICATION


#define NUM_DEMOS 7
#define NUM_TESTS 100


extern bool gDisableDeactivation;

int main(int argc,char** argv)
{
	gDisableDeactivation = true;

	BenchmarkDemo1 benchmarkDemo1;
	BenchmarkDemo2 benchmarkDemo2;
	BenchmarkDemo3 benchmarkDemo3;
	BenchmarkDemo4 benchmarkDemo4;
	BenchmarkDemo5 benchmarkDemo5;
	BenchmarkDemo6 benchmarkDemo6;
	BenchmarkDemo7 benchmarkDemo7;

	BenchmarkDemo* demoArray[NUM_DEMOS] = {&benchmarkDemo1,&benchmarkDemo2,&benchmarkDemo3,&benchmarkDemo4,&benchmarkDemo5,&benchmarkDemo6,&benchmarkDemo7};
	const char* demoNames[NUM_DEMOS] = {"3000 fall", "1000 stack", "136 ragdolls","1000 convex", "prim-trimesh", "convex-trimesh","raytests"};
	float totalTime[NUM_DEMOS] = {0.f,0.f,0.f,0.f,0.f,0.f,0.f};

#ifdef USE_GLUT_DEMO_APPLICATION
	benchmarkDemo.getDynamicsWorld()->setDebugDrawer(&gDebugDrawer);
	return glutmain(argc, argv,640,480,"Bullet Physics Demo. http://bulletphysics.com",&benchmarkDemo);
#else
	int d;

	for (d=0;d<NUM_DEMOS;d++)
	{
		demoArray[d]->initPhysics();
		

		for (int i=0;i<NUM_TESTS;i++)
		{
			demoArray[d]->clientMoveAndDisplay();
			float frameTime = CProfileManager::Get_Time_Since_Reset();
			if ((i % 25)==0)
			{
			#ifdef NO_TIME
			        printf("BenchmarkDemo: %s, Frame: %d\n", demoNames[d], i);
			#else
				printf("BenchmarkDemo: %s, Frame %d, Duration (ms): %f\n",demoNames[d],i, frameTime);
			#endif
			}
			totalTime[d] += frameTime;
			#ifndef NO_TIME
			if (i==NUM_TESTS-1)
				CProfileManager::dumpAll();
			#endif
			
		}
	}

	#ifndef NO_TIME
	for (d=0;d<NUM_DEMOS;d++)
	{
		printf("\nResults for %s: %f",demoNames[d],totalTime[d]*(1.f/NUM_TESTS));
	}
	#endif

#endif
	return 0;
}
