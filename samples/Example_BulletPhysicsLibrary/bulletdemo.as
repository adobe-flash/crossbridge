// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

package
{
  import flash.display.Sprite;
  import flash.events.Event;
  import flash.text.TextField;

  import org.bulletphysics.*;

  public class bulletdemo extends Sprite
  {
    public function bulletdemo()
    {
      addEventListener(Event.ADDED_TO_STAGE, initCode);
    }
 
    public function initCode(e:Event):void
    {
      CModule.startAsync(this)
      
      var tf:TextField = new TextField
      tf.multiline = true
      tf.width = stage.stageWidth
      tf.height = stage.stageHeight
      addChild(tf)

      var broadphase:btDbvtBroadphase = btDbvtBroadphase.create(0)
      var defCollisionInfo:btDefaultCollisionConstructionInfo =
        btDefaultCollisionConstructionInfo.create()
      var collisionConfig:btDefaultCollisionConfiguration =
        btDefaultCollisionConfiguration.create(defCollisionInfo.swigCPtr)
      var dispatcher:btCollisionDispatcher =
        btCollisionDispatcher.create(collisionConfig.swigCPtr)
      
      var solver:btSequentialImpulseConstraintSolver =
        btSequentialImpulseConstraintSolver.create()

      var world:btDiscreteDynamicsWorld =
        btDiscreteDynamicsWorld.create(dispatcher.swigCPtr,
          broadphase.swigCPtr, solver.swigCPtr, collisionConfig.swigCPtr)

      world.setGravity(vector(0, -10, 0))

      var groundShape:btStaticPlaneShape = 
        btStaticPlaneShape.create(vector(0, 1, 0), 1)
      var fallShape:btSphereShape = btSphereShape.create(1)

      var trans:btTransform = btTransform.create()
      trans.setOrigin(vector(0, -1, 0))
      trans.setRotation(quat(0, 0, 0, 1))

      var groundMotionState:btDefaultMotionState =
        btDefaultMotionState.create(trans.swigCPtr, btTransform.getIdentity())
      var groundRigidBodyCI:btRigidBodyConstructionInfo =
        btRigidBodyConstructionInfo.create(0, groundMotionState.swigCPtr,
          groundShape.swigCPtr, vector(0, 0, 0))
      var groundRigidBody:btRigidBody =
        btRigidBody.create(groundRigidBodyCI.swigCPtr)
      world.addRigidBody(groundRigidBody.swigCPtr)

      trans = btTransform.create()
      trans.setOrigin(vector(0, 50, 0))
      trans.setRotation(quat(0, 0, 0, 1))
      
      var fallMotionState:btDefaultMotionState =
        btDefaultMotionState.create(trans.swigCPtr, btTransform.getIdentity())
      fallShape.calculateLocalInertia(1, vector(0, 0, 0))
      var fallRigidBodyCI:btRigidBodyConstructionInfo =
        btRigidBodyConstructionInfo.create(1, fallMotionState.swigCPtr,
          fallShape.swigCPtr, vector(0, 0, 0))
      var fallRigidBody:btRigidBody =
        btRigidBody.create(fallRigidBodyCI.swigCPtr)
      world.addRigidBody(fallRigidBody.swigCPtr)
      
      for (var i:int = 0; i < 5; i++) {
        world.stepSimulation(1/60., 10, 1/60.)
        var motionStatePtr:int = fallRigidBody.getMotionState()
        var motionState:btDefaultMotionState = new btDefaultMotionState()
        motionState.swigCPtr = motionStatePtr
        trans = btTransform.create()
        motionState.getWorldTransform(trans.swigCPtr)

        var pos:btVector3 = new btVector3()
        pos.swigCPtr = trans.getOrigin()
        var yPtr:int = pos.getY()
        var s:String = "sphere height: " + CModule.readFloat(yPtr) + "\n" 
        tf.appendText( s )
        trace( s )
      }
      
      world.removeRigidBody(fallRigidBody.swigCPtr)
      world.removeRigidBody(groundRigidBody.swigCPtr)
      fallRigidBody.destroy()
      groundRigidBody.destroy()
      world.destroy()
      solver.destroy()
      dispatcher.destroy()
      collisionConfig.destroy()
      broadphase.destroy()
    }

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
  }
}
