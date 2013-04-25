// Copyright 2006-2008 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


// This is a JavaScript implementation of the Richards
// benchmark from:
//
//    http://www.cl.cam.ac.uk/~mr10/Bench.html
//
// The benchmark was originally implemented in BCPL by
// Martin Richards.

package richards {
    //import avmplus.System;
    const COUNT:int = 1000;
    
    /**
     * These two constants specify how many times a packet is queued and
     * how many times a task is put on hold in a correct run of richards.
     * They don't have any meaning a such but are characteristic of a
     * correct run so if the actual queue or hold count is different from
     * the expected there must be a bug in the implementation.
     **/
    const EXPECTED_QUEUE_COUNT:int = 2322;
    const EXPECTED_HOLD_COUNT:int = 928;
    
    const ID_IDLE:int       = 0;
    const ID_WORKER:int     = 1;
    const ID_HANDLER_A:int  = 2;
    const ID_HANDLER_B:int  = 3;
    const ID_DEVICE_A:int   = 4;
    const ID_DEVICE_B:int   = 5;
    
    
    const KIND_DEVICE:int   = 0;
    const KIND_WORK:int     = 1;

    var r=new RichardsClass();
    
    public class RichardsClass {

    
        public function RichardsClass() {
            var Richards:BenchmarkSuite = new BenchmarkSuite('Richards', 34886, [
              new Benchmark("Richards", runRichards)
            ]);
        }
    
        /**
         * The Richards benchmark simulates the task dispatcher of an
         * operating system.
         **/
        public function runRichards():void {
          var scheduler:Scheduler = new Scheduler();
          scheduler.addIdleTask(ID_IDLE, 0, null, COUNT);
        
          var queue:Packet = new Packet(null, ID_WORKER, KIND_WORK);
          queue = new Packet(queue,  ID_WORKER, KIND_WORK);
          scheduler.addWorkerTask(ID_WORKER, 1000, queue);
        
          queue = new Packet(null, ID_DEVICE_A, KIND_DEVICE);
          queue = new Packet(queue,  ID_DEVICE_A, KIND_DEVICE);
          queue = new Packet(queue,  ID_DEVICE_A, KIND_DEVICE);
          scheduler.addHandlerTask(ID_HANDLER_A, 2000, queue);
        
          queue = new Packet(null, ID_DEVICE_B, KIND_DEVICE);
          queue = new Packet(queue,  ID_DEVICE_B, KIND_DEVICE);
          queue = new Packet(queue,  ID_DEVICE_B, KIND_DEVICE);
          scheduler.addHandlerTask(ID_HANDLER_B, 3000, queue);
        
          scheduler.addDeviceTask(ID_DEVICE_A, 4000, null);
        
          scheduler.addDeviceTask(ID_DEVICE_B, 5000, null);
        
          scheduler.schedule();
        
          if (scheduler.queueCount != EXPECTED_QUEUE_COUNT ||
              scheduler.holdCount != EXPECTED_HOLD_COUNT) {
            var msg =
                "Error during execution: queueCount = " + scheduler.queueCount +
                ", holdCount = " + scheduler.holdCount + ".  Test Failed.";
            print(msg);
//            System.exit(1);
          }
        }
    } // class RichardsClass
    
    
    
    
    public class Scheduler {
        /**
         * A scheduler can be used to schedule a set of tasks based on their relative
         * priorities.  Scheduling is done by maintaining a list of task control blocks
         * which holds tasks and the data queue they are processing.
         * @constructor
         */
        static const NUMBER_OF_IDS:int = 6;
        
        public var queueCount:int;
        public var holdCount:int;
        public var blocks:Vector.<TaskControlBlock>;
        public var list:TaskControlBlock;
        public var currentTcb:TaskControlBlock;
        public var currentId:int;
          
        public function Scheduler() {
          this.queueCount = 0;
          this.holdCount = 0;
          this.blocks = new Vector.<TaskControlBlock>(NUMBER_OF_IDS);
          this.list = null;
          this.currentTcb = null;
          this.currentId = null;
        }
        
        
        
        
        /**
         * Add an idle task to this scheduler.
         * @param {int} id the identity of the task
         * @param {int} priority the task's priority
         * @param {Packet} queue the queue of work to be processed by the task
         * @param {int} count the number of times to schedule the task
         */
        public function addIdleTask(id:int, priority:int, queue:Packet, count:int):void {
          this.addRunningTask(id, priority, queue, new IdleTask(this, 1, count));
        };
        
        /**
         * Add a work task to this scheduler.
         * @param {int} id the identity of the task
         * @param {int} priority the task's priority
         * @param {Packet} queue the queue of work to be processed by the task
         */
        public function addWorkerTask(id:int, priority:int, queue:Packet):void {
          this.addTask(id, priority, queue, new WorkerTask(this, ID_HANDLER_A, 0));
        };
        
        /**
         * Add a handler task to this scheduler.
         * @param {int} id the identity of the task
         * @param {int} priority the task's priority
         * @param {Packet} queue the queue of work to be processed by the task
         */
        public function addHandlerTask(id:int, priority:int, queue:Packet):void {
          this.addTask(id, priority, queue, new HandlerTask(this));
        };
        
        /**
         * Add a handler task to this scheduler.
         * @param {int} id the identity of the task
         * @param {int} priority the task's priority
         * @param {Packet} queue the queue of work to be processed by the task
         */
        public function addDeviceTask(id:int, priority:int, queue:Packet):void {
          this.addTask(id, priority, queue, new DeviceTask(this))
        };
        
        /**
         * Add the specified task and mark it as running.
         * @param {int} id the identity of the task
         * @param {int} priority the task's priority
         * @param {Packet} queue the queue of work to be processed by the task
         * @param {Task} task the task to add
         */
        public function addRunningTask(id:int, priority:int, queue:Packet, task:Task):void {
          this.addTask(id, priority, queue, task);
          this.currentTcb.setRunning();
        };
        
        /**
         * Add the specified task to this scheduler.
         * @param {int} id the identity of the task
         * @param {int} priority the task's priority
         * @param {Packet} queue the queue of work to be processed by the task
         * @param {Task} task the task to add
         */
        public function addTask(id:int, priority:int, queue:Packet, task:Task):void {
          this.currentTcb = new TaskControlBlock(this.list, id, priority, queue, task);
          this.list = this.currentTcb;
          this.blocks[id] = this.currentTcb;
        };
        
        /**
         * Execute the tasks managed by this scheduler.
         */
        public function schedule():void {
          this.currentTcb = this.list;
          while (this.currentTcb != null) {
            if (this.currentTcb.isHeldOrSuspended()) {
              this.currentTcb = this.currentTcb.link;
            } else {
              this.currentId = this.currentTcb.id;
              this.currentTcb = this.currentTcb.run();
            }
          }
        };
        
        /**
         * Release a task that is currently blocked and return the next block to run.
         * @param {int} id the id of the task to suspend
         */
        public function release(id:int):TaskControlBlock {
          var tcb = this.blocks[id];
          if (tcb == null) return tcb;
          tcb.markAsNotHeld();
          if (tcb.priority > this.currentTcb.priority) {
            return tcb;
          } else {
            return this.currentTcb;
          }
        };
        
        /**
         * Block the currently executing task and return the next task control block
         * to run.  The blocked task will not be made runnable until it is explicitly
         * released, even if new work is added to it.
         */
        public function holdCurrent():TaskControlBlock {
          this.holdCount++;
          this.currentTcb.markAsHeld();
          return this.currentTcb.link;
        };
        
        /**
         * Suspend the currently executing task and return the next task control block
         * to run.  If new work is added to the suspended task it will be made runnable.
         */
        public function suspendCurrent():TaskControlBlock {
          this.currentTcb.markAsSuspended();
          return this.currentTcb;
        };
        
        /**
         * Add the specified packet to the end of the worklist used by the task
         * associated with the packet and make the task runnable if it is currently
         * suspended.
         * @param {Packet} packet the packet to add
         */
        public function queue(packet:Packet):TaskControlBlock { // TODO: return type?
          var t = this.blocks[packet.id];
          if (t == null) return t;
          this.queueCount++;
          packet.link = null;
          packet.id = this.currentId;
          return t.checkPriorityAdd(this.currentTcb, packet);
        };
    } // class Scheduler
        
    
    public class TaskControlBlock {
        /**
         * A task control block manages a task and the queue of work packages associated
         * with it.
         * @param {TaskControlBlock} link the preceding block in the linked block list
         * @param {int} id the id of this block
         * @param {int} priority the priority of this block
         * @param {Packet} queue the queue of packages to be processed by the task
         * @param {Task} task the task
         * @constructor
         */
         
        public var link:TaskControlBlock;
        public var id:int;
        public var priority:int;
        public var queue:Packet;
        public var task:Task;
        public var state:int;
        
        /**
         * The task is running and is currently scheduled.
         */
        static const STATE_RUNNING = 0;
        
        /**
         * The task has packets left to process.
         */
        static const STATE_RUNNABLE = 1;
        
        /**
         * The task is not currently running.  The task is not blocked as such and may
        * be started by the scheduler.
         */
        static const STATE_SUSPENDED = 2;
        
        /**
         * The task is blocked and cannot be run until it is explicitly released.
         */
        static const STATE_HELD = 4;
        
        public var STATE_SUSPENDED_RUNNABLE = STATE_SUSPENDED | STATE_RUNNABLE;
        public var STATE_NOT_HELD = ~STATE_HELD;
        
        public function TaskControlBlock(link:TaskControlBlock, id:int, priority:int, queue:Packet, task:Task) {
          this.link = link;
          this.id = id;
          this.priority = priority;
          this.queue = queue;
          this.task = task;
          if (queue == null) {
            this.state = STATE_SUSPENDED;
          } else {
            this.state = STATE_SUSPENDED_RUNNABLE;
          }
        }
        
        public function setRunning ():void {
          this.state = STATE_RUNNING;
        };
        
        public function markAsNotHeld ():void {
          this.state = this.state & STATE_NOT_HELD;
        };
        
        public function markAsHeld ():void {
          this.state = this.state | STATE_HELD;
        };
        
        public function isHeldOrSuspended ():Boolean {
          return (this.state & STATE_HELD) != 0 || (this.state == STATE_SUSPENDED);
        };
        
        public function markAsSuspended ():void {
          this.state = this.state | STATE_SUSPENDED;
        };
        
        public function markAsRunnable ():void {
          this.state = this.state | STATE_RUNNABLE;
        };
        
        /**
         * Runs this task, if it is ready to be run, and returns the next task to run.
         */
        public function run():TaskControlBlock {
          var packet;
          if (this.state == STATE_SUSPENDED_RUNNABLE) {
            packet = this.queue;
            this.queue = packet.link;
            if (this.queue == null) {
              this.state = STATE_RUNNING;
            } else {
              this.state = STATE_RUNNABLE;
            }
          } else {
            packet = null;
          }
          return this.task.run(packet);
        };
        
        /**
         * Adds a packet to the worklist of this block's task, marks this as runnable if
         * necessary, and returns the next runnable object to run (the one
         * with the highest priority).
         */
        public function checkPriorityAdd (task:TaskControlBlock, packet:Packet):TaskControlBlock {
          if (this.queue == null) {
            this.queue = packet;
            this.markAsRunnable();
            if (this.priority > task.priority) return this;
          } else {
            this.queue = packet.addTo(this.queue);
          }
          return task;
        };
        
        public function toString ():String {
          return "tcb { " + this.task + "@" + this.state + " }";
        };
    } // class TaskControlBlock
    
    // Base interface for all task types
    public interface Task {
        function run(packet:Packet):TaskControlBlock;
    } // interface Task
    
    
    public class IdleTask implements Task {
        /**
         * An idle task doesn't do any work itself but cycles control between the two
         * device tasks.
         * @param {Scheduler} scheduler the scheduler that manages this task
         * @param {int} v1 a seed value that controls how the device tasks are scheduled
         * @param {int} count the number of times this task should be scheduled
         * @constructor
         */
        private var count:int;
        private var scheduler:Scheduler;
        private var v1:*;
        private var v2:*;
        
        public function IdleTask(scheduler:Scheduler, v1:int, count:int):void {
          this.scheduler = scheduler;
          this.v1 = v1;
          this.count = count;
        }
        
        public function run(packet:Packet):TaskControlBlock {
          this.count--;
          if (this.count == 0) return this.scheduler.holdCurrent();
          if ((this.v1 & 1) == 0) {
            this.v1 = this.v1 >> 1;
            return this.scheduler.release(ID_DEVICE_A);
          } else {
            this.v1 = (this.v1 >> 1) ^ 0xD008;
            return this.scheduler.release(ID_DEVICE_B);
          }
        };
        
        public function toString():String {
          return "IdleTask"
        };
    } // class IdleTask
    
    
    public class DeviceTask implements Task {
        /**
         * A task that suspends itself after each time it has been run to simulate
         * waiting for data from an external device.
         * @param {Scheduler} scheduler the scheduler that manages this task
         * @constructor
         */
        private var scheduler:Scheduler;
        private var v1:*;
        private var v2:*;
        
        public function DeviceTask(scheduler:Scheduler):void {
          this.scheduler = scheduler;
          this.v1 = null;
        }
        
        public function run(packet:Packet):TaskControlBlock {
          if (packet == null) {
            if (this.v1 == null) return this.scheduler.suspendCurrent();
            var v = this.v1;
            this.v1 = null;
            return this.scheduler.queue(v);
          } else {
            this.v1 = packet;
            return this.scheduler.holdCurrent();
          }
        };
        
        public function toString():String {
          return "DeviceTask"
        };
    } // class DeviceTask
    
    public class WorkerTask implements Task {
        /**
         * A task that manipulates work packets.
         * @param {Scheduler} scheduler the scheduler that manages this task
         * @param {int} v1 a seed used to specify how work packets are manipulated
         * @param {int} v2 another seed used to specify how work packets are manipulated
         * @constructor
         */
        private var scheduler:Scheduler;
        private var v1:*;
        private var v2:*;
        
        public function WorkerTask(scheduler:Scheduler, v1:int, v2:int):void {
          this.scheduler = scheduler;
          this.v1 = v1;
          this.v2 = v2;
        }
        
        public function run(packet:Packet):TaskControlBlock {
          if (packet == null) {
            return this.scheduler.suspendCurrent();
          } else {
            if (this.v1 == ID_HANDLER_A) {
              this.v1 = ID_HANDLER_B;
            } else {
              this.v1 = ID_HANDLER_A;
            }
            packet.id = this.v1;
            packet.a1 = 0;
            for (var i = 0; i < DATA_SIZE; i++) {
              this.v2++;
              if (this.v2 > 26) this.v2 = 1;
              packet.a2[i] = this.v2;
            }
            return this.scheduler.queue(packet);
          }
        };
        
        public function toString():String {
          return "WorkerTask"
        };
    } // class WorkerTask
    
    public class HandlerTask implements Task {
        
        /**
         * A task that manipulates work packets and then suspends itself.
         * @param {Scheduler} scheduler the scheduler that manages this task
         * @constructor
         */
        private var scheduler:Scheduler;
        private var v1:*;
        private var v2:*;
        
        public function HandlerTask(scheduler:Scheduler) {
          this.scheduler = scheduler;
          this.v1 = null;
          this.v2 = null;
        }
        
        public function run(packet:Packet):TaskControlBlock {
          if (packet != null) {
            if (packet.kind == KIND_WORK) {
              this.v1 = packet.addTo(this.v1);
            } else {
              this.v2 = packet.addTo(this.v2);
            }
          }
          if (this.v1 != null) {
            var count = this.v1.a1;
            var v;
            if (count < DATA_SIZE) {
              if (this.v2 != null) {
                v = this.v2;
                this.v2 = this.v2.link;
                v.a1 = this.v1.a2[count];
                this.v1.a1 = count + 1;
                return this.scheduler.queue(v);
              }
            } else {
              v = this.v1;
              this.v1 = this.v1.link;
              return this.scheduler.queue(v);
            }
          }
          return this.scheduler.suspendCurrent();
        };
        
        public function toString():String {
          return "HandlerTask"
        };
    } // class HandlerTask
    
    /* --- *
     * P a c k e t
     * --- */
    
    var DATA_SIZE = 4;
    
    /**
     * A simple package of data that is manipulated by the tasks.  The exact layout
     * of the payload data carried by a packet is not importaint, and neither is the
     * nature of the work performed on packets by the tasks.
     *
     * Besides carrying data, packets form linked lists and are hence used both as
     * data and worklists.
     * @param {Packet} link the tail of the linked list of packets
     * @param {int} id an ID for this packet
     * @param {int} kind the type of this packet
     * @constructor
     */
    class Packet {
        public var link:Packet;
        public var id:int;
        public var kind:int;
        public var a1:int;
        public var a2:Vector.<int>;
        
        public function Packet(link:Packet, id:int, kind:int):void {
            this.link = link;
            this.id = id;
            this.kind = kind;
            this.a1 = 0;
            this.a2 = new Vector.<int>(DATA_SIZE);
        }
    
        /**
         * Add this packet to the end of a worklist, and return the worklist.
         * @param {Packet} queue the worklist to add this packet to
         */
        public function addTo(queue:Packet):Packet {
          this.link = null;
          if (queue == null) return this;
          var peek, next = queue;
          while ((peek = next.link) != null)
            next = peek;
          next.link = this;
          return queue;
        }
        
        public function toString():String {
          return "Packet";
        }
    } // class Packet
    
    // if running individually, start the Benchmark suite.
    try {
      runSuite
    } catch (e:ReferenceError) {
      BenchmarkSuite.RunSuites({ NotifyResult: PrintResult,
                               NotifyScore: PrintScore });
    }
} // package