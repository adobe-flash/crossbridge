/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 
 package {

    import flash.concurrent.Mutex;
    import flash.concurrent.Condition;    
import com.adobe.test.Assert;

//     var SECTION = "Workers";
//     var VERSION = "as3";
//     var TITLE   = "Condition and Mutex API";

    var mutex=null;
    mutex=new Mutex();
    Assert.expectEq("create Mutex object, assert is not null",true,mutex!=null);
    var exception:String="";
    try {
        mutex.lock();
        mutex.unlock();
        mutex.lock();
        mutex.unlock();
        mutex.lock();
        mutex.lock();
        mutex.unlock();
        mutex.unlock();
    } catch(e) {
        exception=e.toString();
    }
    Assert.expectEq("locking and unlocking Mutex did not throw any exceptions","",exception);
    var trylock:Boolean=mutex.tryLock();
    Assert.expectEq("tryLock succeeded",trylock,true);
    mutex.unlock();

    exception="";
    var i:uint=0;
    try {
        for (i=0;i<1025;i++) {
            mutex.lock();
        }
    } catch (e) {
        exception=e.toString();
    }
    Assert.expectEq("Mutex lock throws does not throw exception when 1025 mutex locks happen","",exception);

    mutex=new Mutex();
    mutex.lock();
    var condition:Condition;

    exception="";
    try {
        condition=new Condition(null);
    } catch (e) {
        exception=e.toString();
    }
    Assert.expectEq("Condition with no mutex throws ArgumentError","ArgumentError: Error #2007",exception.substring(0,"ArgumentError: Error #2007".length));

    mutex=new Mutex();
    condition=new Condition(mutex);
    Assert.expectEq("Condition has getter mutex",mutex,condition.mutex);
    condition.mutex.lock();
    var notifyRes=condition.notify();
    var notifyAllRes=condition.notifyAll();
    Assert.expectEq("Condition notify and notifyAll returns immediately on same worker",true,notifyRes==null && notifyAllRes==null);
    var waitRes:Boolean;
    waitRes=condition.wait(1);
    Assert.expectEq("Condition wait times out when notify and notifyAll were called previously",true,waitRes);

    mutex=new Mutex();
    condition=new Condition(mutex);
    mutex.lock();
    waitRes=condition.wait(1);
    Assert.expectEq("Condition wait times out with no notify",true,waitRes);

    exception="";
    mutex=new Mutex();
    condition=new Condition(mutex);

    try {
        condition.notify();
    } catch (e) {
        exception=e.toString();
    }
    Assert.expectEq("Condition.notify() without owning the lock to the mutex throws an exception","Error: Error #1516",exception.substring(0,"Error: Error #1516".length));

    try {
        condition.notifyAll();
    } catch (e) {
        exception=e.toString();
    }
    Assert.expectEq("Condition.notifyAll() without owning the lock to the mutex throws an exception","Error: Error #1517",exception.substring(0,"Error: Error #1517".length));

    try {
        condition.wait();
    } catch (e) {
        exception=e.toString();
    }
    Assert.expectEq("Condition.wait() without owning the lock to the mutex throws an exception","Error: Error #1518",exception.substring(0,"Error: Error #1518".length));

 }
