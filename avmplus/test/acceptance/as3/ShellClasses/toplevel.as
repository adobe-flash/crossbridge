/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

    import avmplus.File

//     var SECTION = "toplevel";
//     var VERSION = "as3";
//     var TITLE   = "test toplevel classes";

    import avmplus.System
    import flash.system.Capabilities

import com.adobe.test.Assert;


    Assert.expectEq("queue a gc collection",
      void 0,
      System.queueCollection());

    Assert.expectEq("playerType is AVMPlus",
      "AVMPlus",
      Capabilities.playerType);

    Assert.expectEq("isDebugger is true/false",
      true,
      Capabilities.isDebugger==true || Capabilities.isDebugger==false
    );

    Assert.expectEq("System getAvmplusVersion",
      true,
      System.getAvmplusVersion().length>0);

    Assert.expectEq("System argv",
      0,
      System.argv.length);

    Assert.expectEq("System.isDebugger",
      true,
      System.isDebugger()==true || System.isDebugger()==false);

    Assert.expectEq("System.freeMemory >=0",
      true,
      System.freeMemory>=0);

    Assert.expectEq("System.freeMemory <= System.totalMemory",
      true,
      System.freeMemory<=System.totalMemory);

    Assert.expectEq("System.totalMemory >0",
      true,
      System.totalMemory>0);

    Assert.expectEq("System.privateMemory >0",
      true,
      System.privateMemory>0);

    print("metric System.totalMemory "+System.totalMemory);
    print("metric System.freeMemory "+System.freeMemory);
    print("metric System.privateMemory "+System.privateMemory);


    var err="";
    try {
        System.exec();
    } catch (e) {
        err=e.toString();
    }
    Assert.expectEq("System.exec no params throws exception",
      "ArgumentError: Error #1063",
      err.substring(0,26));

    var ct=0;
    while (getTimer()==0) {
      ct++;
      if (ct==1000000) break;
    }
    var time=getTimer();
    Assert.expectEq("System getTimer>0",
      true,
      time>0);
    print("metric System.getTimer "+getTimer());
    
    
    // Bug 407894: getTimer() returns values that are a power of 2
    // https://bug407894.bugzilla.mozilla.org/attachment.cgi?id=293543
    function notPower2():Boolean {
        // it is possible that there could be a timer returned that is a power
        // of 2, so check multiple times in a row that the timer is not a power of 2.
        var count:int = 0;
        var sleeper:int = 2;
        for(var i:int = 0; i < 5; i++){
            var t1:int = getTimer();
            var t2:int;
            while ( (t2 = getTimer()) == t1 ) {
                // Do some work in order to take up CPU time, keep doing this until the
                // a call to getTimer() != what we have in t1
                for(var j:int = 0; j < sleeper; j++){ var x:Array = [1,2,3,4,5,6,7,8,9,10]; }
                if (sleeper == 2)
                    sleeper <<= 2;
            }
            // determine if the t1 or t2 are a power of 2
            if ( (t1 & (t1 -1)) == 0 )
                count++;
            if ( (t2 & (t2 -1)) == 0 )
                count++;
        }
        if (count == 10)
            return false;
        else
            return true;
    }
    
    Assert.expectEq("System getTimer bug 407894",
      true,
      notPower2());



    
    System.trace(["trace message ln cr \n\r"]);
    System.trace(["trace message ln \n","trace message cr \r"]);
    System.write("write message to stdout");
    System.exit(0);
    
    
