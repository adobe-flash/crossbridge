package tests{
	import flash.display.Sprite;
	import flash.utils.ByteArray;
	import com.adobe.flascc.*;
	import com.adobe.flascc.vfs.*;
	import tests.TestInfo;
 
	public class AlcVFS_getFileHandleFromPath extends Sprite{

		/**
		* Constructor
		**/
		public function AlcVFS_getFileHandleFromPath():void
		{}

		/**
		* Go.
		**/
		public function runTests():void{
			// getFileHandleFromPath() is called throughout the test suite.
			Util.runTest( new TestInfo( getFileHandleFromPath_file, "getFileHandleFromPath_file" ) );
			Util.runTest( new TestInfo( getFileHandleFromPath_dir, "getFileHandleFromPath_dir" ) );
			Util.runTest( new TestInfo( getFileHandleFromPath_invalidPath, "getFileHandleFromPath_invalidPath" ) );
			Util.runTest( new TestInfo( getFileHandleFromPath_deletedFile, "getFileHandleFromPath_deletedFile" ) );
		}

		/**
		 * Pass in a path to a file.
		 **/
		public function getFileHandleFromPath_file( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";
			var fileEntry:FileHandle = null;

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/foo.txt", data );
			fileEntry = CModule.vfs.getFileHandleFromPath( rootDir + "/foo.txt" );
			if( fileEntry != null ){
				info = Util.verifyTableEntry( fileEntry, dataString, rootDir + "/foo.txt", false, info );
			}else{
				info.message += "getFileHandleFromPath() returned null.";
			}

			return info;		
		}
		
		/**
		 * Pass in a path to a directory.
		 **/
		public function getFileHandleFromPath_dir( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var fileEntry:FileHandle = CModule.vfs.getFileHandleFromPath( rootDir );

			if( fileEntry != null ){
				info = Util.verifyTableEntry( fileEntry, null, rootDir, true, info );
			}else{
				info.message += "getFileHandleFromPath() returned null.";
			}

			return info;
		}
		
		/**
		 * Pass in an invalid path.
		 **/
		public function getFileHandleFromPath_invalidPath( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var fileEntry:FileHandle = CModule.vfs.getFileHandleFromPath( rootDir + "/ThisFileDoesNotExist.txt" );

			if( fileEntry == null ){
				info.result = "PASS";
			}else{
				info.message += "Expected a null fileEntry.";
			}

			return info;
		}
		
		/**
		 * Pass in the path to a file which was deleted.
		 **/
		public function getFileHandleFromPath_deletedFile( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";
			var fileEntry:FileHandle = null;

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/foo.txt", data );
			CModule.vfs.deleteFile( rootDir + "/foo.txt" );
			fileEntry = CModule.vfs.getFileHandleFromPath( rootDir + "/foo.txt" );
			
			if( fileEntry == null ){
				info.result = "PASS";
			}else{
				info.message += "File was found; it should have been deleted.";
			}				

			return info;
		}
	}
}


