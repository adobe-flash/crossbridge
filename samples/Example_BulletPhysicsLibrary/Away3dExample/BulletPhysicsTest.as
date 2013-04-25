/*

Based on code from the SkyBox example in Away3d by Rob Bateman (rob@infiniteturtles.co.uk)

This code is distributed under the MIT License

Copyright (c)  

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

package
{
	import away3d.cameras.lenses.*;
	import away3d.containers.*;
	import away3d.core.math.*;
	import away3d.entities.*;
	import away3d.lights.*;
	import away3d.materials.*;
	import away3d.materials.lightpickers.StaticLightPicker;
	import away3d.materials.methods.*;
	import away3d.primitives.*;
	import away3d.textures.*;
	import away3d.utils.*;
	
	import flash.display.*;
	import flash.events.*;
	import flash.geom.Vector3D;

	import org.bulletphysics.*;
	import org.bulletphysics.vfs.ISpecialFile;

	[SWF(backgroundColor="#000000", frameRate="60", width="1024", height="768")]
	
	public class BulletPhysicsTest extends Sprite implements ISpecialFile
	{
		// Environment map.
		[Embed(source="skybox/px.jpg")]
		private var EnvPosX:Class;
		[Embed(source="skybox/py.jpg")]
		private var EnvPosY:Class;
		[Embed(source="skybox/pz.jpg")]
		private var EnvPosZ:Class;
		[Embed(source="skybox/nx.jpg")]
		private var EnvNegX:Class;
		[Embed(source="skybox/ny.jpg")]
		private var EnvNegY:Class;
		[Embed(source="skybox/nz.jpg")]
		private var EnvNegZ:Class;

		//plane textures
		[Embed(source="stone.png")]
		public static var FloorDiffuse:Class;
		
		//engine variables
		private var _view:View3D;
		
		//scene objects
		private var _skyBox:SkyBox; 
		private var meshes:Vector.<Mesh>;
		private var skymat:ColorMaterial;
		private var planemat:TextureMaterial;

		//light objects
		private var light1:DirectionalLight;
		private var light2:DirectionalLight;
		private var lightPicker:StaticLightPicker;

		/**
		 * Initialise the lights
		 */
		private function initLights():void
		{
			light1 = new DirectionalLight();
			light1.direction = new Vector3D(-1,-1,-1);
			light1.castsShadows = true;
			light1.ambient = 0.3;
			light1.diffuse = 0.7;
			light1.shadowMapper.depthMapSize = 1024;
			
			_view.scene.addChild(light1);
			
			lightPicker = new StaticLightPicker([light1]);
		}
		
		/**
		 * Constructor
		 */
		public function BulletPhysicsTest()
		{
			trace("BulletPhysicsTest: " + CModule.ram.length);

			CModule.rootSprite = this

			// Handle the case where we are a pthread
			if(CModule.runningAsWorker()) {
				return;
			}
			
			// PlayerKernel will delegate read/write requests to the "/dev/tty"
        	// file to the object specified with this API.
        	CModule.vfs.console = this

			// Initialize Bullet
			trace("Start the world!");
			CModule.startAsync(this)
			trace("Started the world: " + CModule.ram.length);

			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;
			
			//setup the view
			_view = new View3D();
			addChild(_view);
			
			//setup the camera
			_view.camera.z = -60;
			_view.camera.y = 0;
			_view.camera.lookAt(new Vector3D());
			_view.camera.lens = new PerspectiveLens(90);
			_view.camera.lens.near = 1;
			_view.camera.lens.far = 200;

			initLights();
			
			//setup the cube texture
			var cubeTexture:BitmapCubeTexture = new BitmapCubeTexture(Cast.bitmapData(EnvPosX), Cast.bitmapData(EnvNegX), Cast.bitmapData(EnvPosY), Cast.bitmapData(EnvNegY), Cast.bitmapData(EnvPosZ), Cast.bitmapData(EnvNegZ));
			
			//setup the environment map material
			skymat = new ColorMaterial(0xFFFFFF, 1);
			skymat.specular = 0.5;
			skymat.ambient = 0.25;
			skymat.ambientColor = 0x111199;
			skymat.ambient = 1;
			skymat.mipmap = true;
			//skymat.lightPicker = lightPicker;
			skymat.addMethod(new EnvMapMethod(cubeTexture, 1));

			planemat = new TextureMaterial(Cast.bitmapTexture(FloorDiffuse));
			planemat.lightPicker = lightPicker;
			planemat.shadowMethod = new FilteredShadowMapMethod(light1);
			planemat.repeat = true;
			planemat.mipmap = true;
			
			//setup the scene
			meshes = new Vector.<Mesh>();
			
			_skyBox = new SkyBox(cubeTexture);
			_view.scene.addChild(_skyBox);

			// Setup the physics simulation
			createWorld()
			
			//setup the render loop
			addEventListener(Event.ENTER_FRAME, _onEnterFrame);
			stage.addEventListener(Event.RESIZE, onResize);
			onResize();
		}

		private var broadphase:btDbvtBroadphase
		private var defCollisionInfo:btDefaultCollisionConstructionInfo
		private var collisionConfig:btDefaultCollisionConfiguration
		private var dispatcher:btCollisionDispatcher
		private var solver:btSequentialImpulseConstraintSolver
		private var world:btDiscreteDynamicsWorld
		private var trans:btTransform

		private var bods:Vector.<btRigidBody>;

		private static function vector(x:Number, y:Number, z:Number):int {
	      var vec:btVector3 = btVector3.create()
	      vec.setX(x)
	      vec.setY(y)
	      vec.setZ(z)
	      return vec.swigCPtr
	    }

	    private static function quat(x:Number, y:Number, z:Number, w:Number):int {
	      var q:btQuaternion = btQuaternion.create()
	      q.setX(x)
	      q.setY(y)
	      q.setZ(z)
	      q.setW(w)
	      return q.swigCPtr
	    }

	    private function spawnRigidBody(shape:*, geom:*, mass:Number, x:Number, y:Number, z:Number):btRigidBody
	    {
			var inertia:btVector3 = btVector3.create()
			if(mass != 0)
				shape.calculateLocalInertia(mass, inertia.swigCPtr);
			trans = btTransform.create()
			trans.setIdentity()
			trans.setOrigin(vector(x, y, z))
			var ms:btDefaultMotionState = btDefaultMotionState.create(trans.swigCPtr, btTransform.getIdentity())

			var rbci:btRigidBodyConstructionInfo = btRigidBodyConstructionInfo.create(mass, ms.swigCPtr, shape.swigCPtr, inertia.swigCPtr)
			rbci.m_restitution = 0.1;
			rbci.m_friction = 1.0;

			var rb:btRigidBody = btRigidBody.create(rbci.swigCPtr)
			world.addRigidBody(rb.swigCPtr)

			var mesh:Mesh = new Mesh(geom, planemat)
			mesh.x = x
			mesh.y = y
			mesh.z = z;
			meshes.push(mesh)
			_view.scene.addChild(mesh)

			bods.push(rb)

			return rb
	    }

	    private function spawnSphere(r:Number, x:Number, y:Number, z:Number):btRigidBody
	    {
	    	return spawnRigidBody(
	    		btSphereShape.create(r),
	    		new SphereGeometry(r),
	    		10.0,
	    		x, y, z
			);
	    }

	    private function spawnCube(x:Number, y:Number, z:Number, mass:Number, w:Number, h:Number, d:Number):btRigidBody
	    {
	    	return spawnRigidBody(
	    		btBoxShape.create(vector(w/2,h/2,d/2)),
	    		new CubeGeometry(w,h,d,w,h,d),
	    		mass,
	    		x, y, z
			);
	    }

		private function createWorld():void
		{
			bods = new Vector.<btRigidBody>()

			var maxNumOutstandingTasks:int = 2

			defCollisionInfo = btDefaultCollisionConstructionInfo.create()
			defCollisionInfo.m_defaultMaxPersistentManifoldPoolSize = 32768;
			collisionConfig = btDefaultCollisionConfiguration.create(defCollisionInfo.swigCPtr)

			if(true) {
				dispatcher = btCollisionDispatcher.create(collisionConfig.swigCPtr)
				solver = btSequentialImpulseConstraintSolver.create()
			} else {
				dispatcher = new btCollisionDispatcher();
				dispatcher.swigCPtr = Bullet.createThreadedDispatcher(collisionConfig.swigCPtr, maxNumOutstandingTasks);
				
				solver = new btSequentialImpulseConstraintSolver();
				solver.swigCPtr = Bullet.createThreadedSolver(maxNumOutstandingTasks);
				
				//this solver requires the contacts to be in a contiguous pool, so avoid dynamic allocation
				dispatcher.setDispatcherFlags(btCollisionDispatcher.CD_DISABLE_CONTACTPOOL_DYNAMIC_ALLOCATION);
			}

			broadphase = btDbvtBroadphase.create(0)
			world = btDiscreteDynamicsWorld.create(dispatcher.swigCPtr, broadphase.swigCPtr, solver.swigCPtr, collisionConfig.swigCPtr)
			world.setGravity(vector(0, -20, 0))

			//world.getDispatchInfo().m_enableSPU = true;

			// Create some massless (static) cubes
			spawnCube(-50, 0, 0, 0, 5, 5, 100)
			spawnCube(50, 0, 0, 0, 5, 5, 100)
			spawnCube(0, 0, 50, 0, 100, 5, 5)
			spawnCube(0, 0, -50, 0, 100, 5, 5)
			spawnCube(0, -1, 0, 0, 100, 0.1, 100)

			var numCols:int = 6;
			var w:Number = 2.0;
			var s:Number = 4.0;
			
			for(var i:int=0; i<256; i++) {
				if(i%7 == 0)
					spawnSphere(w, ((i%numCols) ) * 10 - 30, 10.0 + ((i/numCols) * s), 0)
				else
					spawnCube(((i%numCols)) * 10  - 30, 10.0 + ((i/numCols) * s), 0, 10, w*2, w*2, w*2)
			}
		}
		
		/**
	    * The PlayerKernel implementation uses this function to handle
	    * C IO write requests to the file "/dev/tty" (for example, output from
	    * printf will pass through this function). See the ISpecialFile
	    * documentation for more information about the arguments and return value.
	    */
	    public function write(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int
	    {
	      var str:String = CModule.readString(bufPtr, nbyte)
	      trace(str)
	      return nbyte
	    }

	    /**
	    * The PlayerKernel implementation uses this function to handle
	    * C IO read requests to the file "/dev/tty" (for example, reads from stdin
	    * will expect this function to provide the data). See the ISpecialFile
	    * documentation for more information about the arguments and return value.
	    */
	    public function read(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int
	    {
	      return 0
	    }

	    /**
	    * The PlayerKernel implementation uses this function to handle
	    * C fcntl requests to the file "/dev/tty." 
	    * See the ISpecialFile documentation for more information about the
	    * arguments and return value.
	    */
	    public function fcntl(fd:int, com:int, data:int, errnoPtr:int):int
	    {
	      return 0
	    }

	    /**
	    * The PlayerKernel implementation uses this function to handle
	    * C ioctl requests to the file "/dev/tty." 
	    * See the ISpecialFile documentation for more information about the
	    * arguments and return value.
	    */
	    public function ioctl(fd:int, com:int, data:int, errnoPtr:int):int
	    {
	      return 0;
	    }
		
		/**
		 * render loop
		 */
		private function _onEnterFrame(e:Event):void
		{
			CModule.serviceUIRequests();		
			_view.camera.position = new Vector3D();
			_view.camera.rotationY += 0.5*(stage.mouseX-stage.stageWidth/2)/800;
			_view.camera.moveBackward(80);
			_view.camera.moveUp(25);
			var i:int

			for(i=0; i<1; i++)
				world.stepSimulation(1/60.0, 0, 0)

	        for(i=0; i<meshes.length; i++) {
	        	positionAndRotateMesh(meshes[i], bods[i])
	        }
			
			_view.render();
		}
		
		/**
		 * stage listener for resize events
		 */
		private function onResize(event:Event = null):void
		{
			_view.width = stage.stageWidth;
			_view.height = stage.stageHeight;
		}
	}
}
