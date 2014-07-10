package tests{
	import flash.display.Sprite;
	import flash.utils.ByteArray;
	import com.adobe.flascc.*;
	import com.adobe.flascc.vfs.*;
	import tests.TestInfo;
 
	/**
	 * This class also tests isValidFileDescriptor() and getFileHandleFromPath().  The code paths are the same
	 * but I'm going to go through the motions (just copy/paste mostly) for completeness.
	 * It might catch something if we make changes later.
	 **/
	public class AlcVFS_openFile extends Sprite{

		/**
		* Constructor
		**/
		public function AlcVFS_openFile():void
		{}

		/**
		* Go.
		**/
		public function runTests():void{
			Util.runTest( new TestInfo( openFile_invalidEntry, "openFile_invalidEntry" ) );
			Util.runTest( new TestInfo( openFile_file, "openFile_file" ) );
			Util.runTest( new TestInfo( openFile_directory, "openFile_directory" ) );
			Util.runTest( new TestInfo( openFile_openOpenedFile, "openFile_openOpenedFile" ) );
			Util.runTest( new TestInfo( openFile_openOpenedFile2, "openFile_openOpenedFile2" ) );
			Util.runTest( new TestInfo( openFile_openClosedFile, "openFile_openClosedFile" ) );
		}

		/**
		 * Pass in an invalid FileHandle.  What should happen?  From the code,
		 * this just puts the FileHandle into an array. We don't verify that
		 * the file is valid via this method.
		 **/
		public function openFile_invalidEntry( info:TestInfo ):TestInfo{
			var fileEntry1:FileHandle = new FileHandle();
			var fileEntry2:FileHandle = null;
			var fd:int = CModule.vfs.openFile( fileEntry1 );

			if( CModule.vfs.isValidFileDescriptor( fd ) ){
				fileEntry2 = CModule.vfs.getFileHandleFromFileDescriptor( fd );
				if( fileEntry1 === fileEntry2 ){
					info.result = "PASS";
				}
			}else{
				info.message += "isValidFileDescriptor() returned false.";
			}

			return info;
		}
		
		/**
		 * Open a file.  This should work exactly like the invalid file one.
		 **/
		public function openFile_file( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";
			var fileEntry1:FileHandle = null;
			var fileEntry2:FileHandle = null;
			var fd:int = -1;

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/foo.txt", data );
			fileEntry1 = CModule.vfs.getFileHandleFromPath( rootDir + "/foo.txt" );
			fd = CModule.vfs.openFile( fileEntry1 );
			
			if( CModule.vfs.isValidFileDescriptor( fd ) ){
				fileEntry2 = CModule.vfs.getFileHandleFromFileDescriptor( fd );
				if( fileEntry1 === fileEntry2 ){
					info.result = "PASS";
				}
			}else{
				info.message += "isValidFileDescriptor() returned false.";
			}

			return info;
		}
			
		/**
		 * Open a directory.  This should work just like the above.
		 **/	
		public function openFile_directory( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var fileEntry1:FileHandle = null;
			var fileEntry2:FileHandle = null;
			var fd:int = -1;

			fileEntry1 = CModule.vfs.getFileHandleFromPath( rootDir );
			fd = CModule.vfs.openFile( fileEntry1 );
			
			if( CModule.vfs.isValidFileDescriptor( fd ) ){
				fileEntry2 = CModule.vfs.getFileHandleFromFileDescriptor( fd );
				if( fileEntry1 === fileEntry2 ){
					info.result = "PASS";
				}
			}else{
				info.message += "isValidFileDescriptor() returned false.";
			}

			return info;
		}
		
		/**
		 * Open a file which has already been opened, using a different fileEntry.
		 * This will just make two entries in _fdTable.  Make sure one is still 
		 * open after the other is closed.
		 **/
		public function openFile_openOpenedFile( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";
			var fileEntry1:FileHandle = null;
			var fileEntry2:FileHandle = null;
			var fd1:int = -1;
			var fd2:int = -1;

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/foo.txt", data );			
			fileEntry1 = CModule.vfs.getFileHandleFromPath( rootDir + "/foo.txt" );
			fd1 = CModule.vfs.openFile( fileEntry1 );
			fileEntry2 = CModule.vfs.getFileHandleFromPath( rootDir + "/foo.txt" );
			fd2 = CModule.vfs.openFile( fileEntry2 );

			if( CModule.vfs.isValidFileDescriptor( fd1 ) ){
				if( CModule.vfs.isValidFileDescriptor( fd2 ) ){
					if( fd1 != fd2 ){
						CModule.vfs.closeFile( fd1 );
						if( (!CModule.vfs.isValidFileDescriptor( fd1 )) && CModule.vfs.isValidFileDescriptor( fd2 ) ){
							info.result = "PASS";
						}else{	
							info.message += "Problems with closing fd1.";
						}
					}else{
						info.message += "Did not expect identical file descriptors.";
					}	
				}else{
					info.message += "isValidFileDescriptor( fd2 ) returned false.";
				}
			}else{
				info.message += "isValidFileDescriptor( fd1 ) returned false.";
			}

			return info;			
		}

		/**
		 * Open a file which has already been opened, using THE SAME fileEntry.
		 * This will just make two entries in _fdTable.  Make sure one is still 
		 * open after the other is closed.
		 * This is the same code path as above, but again, in case we change later
		 * or if someone implements their own...
		 **/
		public function openFile_openOpenedFile2( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";
			var fileEntry1:FileHandle = null;
			var fd1:int = -1;
			var fd2:int = -1;

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/foo.txt", data );			
			fileEntry1 = CModule.vfs.getFileHandleFromPath( rootDir + "/foo.txt" );
			fd1 = CModule.vfs.openFile( fileEntry1 );
			fd2 = CModule.vfs.openFile( fileEntry1 );

			if( CModule.vfs.isValidFileDescriptor( fd1 ) ){
				if( CModule.vfs.isValidFileDescriptor( fd2 ) ){
					if( fd1 != fd2 ){
						CModule.vfs.closeFile( fd1 );
						if( (!CModule.vfs.isValidFileDescriptor( fd1 )) && CModule.vfs.isValidFileDescriptor( fd2 ) ){
							info.result = "PASS";
						}else{	
							info.message += "Problems with closing fd1.";
						}
					}else{
						info.message += "Did not expect identical file descriptors.";
					}	
				}else{
					info.message += "isValidFileDescriptor( fd2 ) returned false.";
				}
			}else{
				info.message += "isValidFileDescriptor( fd1 ) returned false.";
			}

			return info;			
		}
		
		/**
		 * Open a closed file.
		 **/
		public function openFile_openClosedFile( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";
			var fileEntry1:FileHandle = null;
			var fileEntry2:FileHandle = null;
			var fd1:int = -1;

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/foo.txt", data );
			fileEntry1 = CModule.vfs.getFileHandleFromPath( rootDir + "/foo.txt" );
			fd1 = CModule.vfs.openFile( fileEntry1 );

			if( CModule.vfs.isValidFileDescriptor( fd1 ) ){
				CModule.vfs.closeFile( fd1 );
				fd1 = CModule.vfs.openFile( fileEntry1 );
				if( CModule.vfs.isValidFileDescriptor( fd1 ) ){
					fileEntry2 = CModule.vfs.getFileHandleFromFileDescriptor( fd1 );
					if( fileEntry2 != null ){
						info.result = "PASS";
					}else{
						info.message += "Could not open closed file (bad FileHandle).";
					}
				}else{
					info.message += "Could not open closed file (bad file descriptor).";
				}
			}else{
				info.message += "isValidFileDescriptor( fd1 ) returned false.";
			}

			return info;
		}
	}
}


