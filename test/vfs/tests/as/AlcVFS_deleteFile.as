package tests{
	import flash.display.Sprite;
	import flash.utils.ByteArray;
	import com.adobe.flascc.vfs.*;
 	import com.adobe.flascc.*;
	import tests.TestInfo;
 
	public class AlcVFS_deleteFile extends Sprite{

		/**
		* Constructor
		**/
		public function AlcVFS_deleteFile():void
		{}

		/**
		* Go.
		**/
		public function runTests():void{
			Util.runTest( new TestInfo( deleteFile_file, "deleteFile_file" ) );
			Util.runTest( new TestInfo( deleteFile_emptyDir, "deleteFile_emptyDir" ) );
			Util.runTest( new TestInfo( deleteFile_populatedDir, "deleteFile_populatedDir" ) );
			Util.runTest( new TestInfo( deleteFile_noFile, "deleteFile_noFile" ) );
		}
		
		/**
		 * Delete a file.
		 **/
		public function deleteFile_file( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";
			var fileEntry:FileHandle = null;

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/foo.txt", data );
			fileEntry = CModule.vfs.getFileHandleFromPath( rootDir + "/foo.txt" );
			if( fileEntry != null ){
				CModule.vfs.deleteFile( rootDir + "/foo.txt" );
				fileEntry = CModule.vfs.getFileHandleFromPath( rootDir + "/foo.txt" );
				if( fileEntry == null ){
					info.result = "PASS";
				}else{
					info.message += "File was found; it should have been deleted.";
				}				
			}else{
				info.message += "addFile() failed, so deleteFile() could not be tested.";
			}

			return info;
		}

		/**
		 * Delete an empty directory.
		 **/
		public function deleteFile_emptyDir( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var fileEntry:FileHandle = null;

			fileEntry = CModule.vfs.getFileHandleFromPath( rootDir );
			if( fileEntry != null ){
				CModule.vfs.deleteFile( rootDir );
				fileEntry = CModule.vfs.getFileHandleFromPath( rootDir );
				if( fileEntry == null ){
					info.result = "PASS";
				}else{
					info.message += "Directory was found; it should have been deleted.";
				}				
			}else{
				info.message += "addDirectory() failed, so deleteFile() could not be tested.";
			}

			return info;
		}

		/**
		 * Try to delete a populated directory.
		 **/
		public function deleteFile_populatedDir( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";
			var fileEntry:FileHandle = null;
			var rteMessage:String = null;

			//info.message += "Fails due to ALC-348. (fixed)";

			data.writeUTFBytes( dataString );
			CModule.vfs.addDirectory( rootDir + "/dir1" );
			CModule.vfs.addFile( rootDir + "/dir1/foo.txt", data );
			fileEntry = CModule.vfs.getFileHandleFromPath( rootDir + "/dir1/foo.txt" );

			if( fileEntry != null ){
				try{
					CModule.vfs.deleteFile( rootDir + "/dir1" );
				}catch( e:Error ){
					rteMessage = e.message;
				}

				if( rteMessage == "cannot delete \'/store18/dir1\', directory not empty" ){
					fileEntry = CModule.vfs.getFileHandleFromPath( rootDir + "/dir1" );
					if( fileEntry != null ){
						fileEntry = CModule.vfs.getFileHandleFromPath( rootDir + "/dir1/foo.txt" );
						if( fileEntry != null ){
							info.result = "PASS";
						}else{
							info.message += "File was deleted when it should not have been.";
						}
					}else{
						info.message += "Directory was deleted when it should not have been.";
					}
				}else{
					info.message += "Did not get expected RTE.";
				}
			}else{
				info.message += "addFile() failed, so deleteFile() could not be tested.";
			}

			return info;
		}

		/**
		 * Call when a file does not exist.
		 **/
		public function deleteFile_noFile( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var err:String = null;
			
			try{		
				CModule.vfs.deleteFile( rootDir + "/thisDoesNotExist.txt" );
			}catch( e:Error ){
				if( e.message == "cannot delete \'" + rootDir + "/thisDoesNotExist.txt\', file does not exist" ){
					info.result = "PASS";
				}
			}

			return info;
		}
	}
}


