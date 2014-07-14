package tests{
	import flash.utils.ByteArray;
	import com.adobe.flascc.*;
	import com.adobe.flascc.vfs.*;

	public class TestInfo{
		public var name:String = null;
		public var result:String = "FAIL";
		public var message:String = "";
		public var func:Function = null;

		public function TestInfo( f:Function, testName:String ):void{
			func = f;
			name = testName;
		}
	}
}


