package tests{
	import flash.display.Sprite;
	import flash.utils.ByteArray;
	import com.adobe.flascc.*;
	import com.adobe.flascc.vfs.*;
	import tests.TestInfo;
 
	public class AlcVFS_addDirectory extends Sprite{

		/**
		* Constructor
		**/
		public function AlcVFS_addDirectory():void
		{}

		/**
		* Go.
		* addDirectory takes the same code path as addFile, except that the data is null.
		**/
		public function runTests():void{
			Util.runTest( new TestInfo( addDirectory_normal, "addDirectory_normal" ) );
		}
	
		/**
		 * Supply a normal directory.
		 **/
		public function addDirectory_normal( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var fileEntry:FileHandle = CModule.vfs.getFileHandleFromPath( rootDir );

			if( fileEntry != null ){
				if( fileEntry.bytes == null ){
					if( fileEntry.isDirectory == true ) {
						info.result = "PASS";
					}else{
						info.message += "\tGot a file, expected a directory.\n";
					}
				}else{
					info.message += "\tData was not correct.\n";
				}
			}else{
				info.message += "\tFileHandle was null.\n";
			}

			return info;	
		}
	}
}


