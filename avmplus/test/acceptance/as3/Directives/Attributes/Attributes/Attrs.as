/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package Attrs {

 public class Attrs
 {
    /*=======================================================================*/
    /* Variables                                                             */
    /*=======================================================================*/
                    var emptyVar    : String;
    public          var pubVar      : String;
    private         var privVar     : String;
    static          var statVar     : String;
    public  static  var pubStatVar  : String;
    private static  var privStatVar : String;
    static  public  var statPubVar  : String;
    static  private var statPrivVar : String;

    function Attrs()
    {
        emptyVar    = "var, empty         ";
        pubVar      = "var, public        ";
        privVar     = "var, private       ";
        statVar     = "var, static        ";
        pubStatVar  = "var, public static ";
        privStatVar = "var, private static";
        statPubVar  = "var, static public ";
        statPrivVar = "var, static private";
    };

    function getStatVar()     { return statVar;     };
    function getPubStatVar()  { return pubStatVar;  };
    function getPrivStatVar() { return privStatVar; };
    function getStatPubVar()  { return statPubVar;  };
    function getStatPrivVar() { return statPrivVar; };

    /*=======================================================================*/
    /* Functions                                                             */
    /*=======================================================================*/

                    function emptyFunc()    { return "func, empty         "; };
    public          function pubFunc()      { return "func, public        "; };
    private         function privFunc()     { return "func, private       "; };
    static          function statFunc()     { return "func, static        "; };
    public  static  function pubStatFunc()  { return "func, public static "; };
    private static  function privStatFunc() { return "func, private static"; };
    static  public  function statPubFunc()  { return "func, static public "; };
    static  private function statPrivFunc() { return "func, static private"; };

    function getStatFunc()     { return statFunc();     };
    function getPubStatFunc()  { return pubStatFunc();  };
    function getPrivStatFunc() { return privStatFunc(); };
    function getStatPubFunc()  { return statPubFunc();  };
    function getStatPrivFunc() { return statPrivFunc(); };
 }
}
