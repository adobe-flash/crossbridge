package tests{
	import flash.display.Sprite;
	import flash.utils.ByteArray;
	import com.adobe.flascc.*;
	import com.adobe.flascc.vfs.*;
	import tests.TestInfo;
 
	public class AlcVFS_getDirEntries extends Sprite{

		/**
		* Constructor
		**/
		public function AlcVFS_getDirEntries():void
		{}

		/**
		* Go.
		**/
		public function runTests():void{
			Util.runTest( new TestInfo( getDirectoryEntries_dirMissing, "getDirectoryEntries_dirMissing" ) );
			Util.runTest( new TestInfo( getDirectoryEntries_passInFile, "getDirectoryEntries_passInFile" ) );
			Util.runTest( new TestInfo( getDirectoryEntries_emptyDir, "getDirectoryEntries_emptyDir" ) );
			Util.runTest( new TestInfo( getDirectoryEntries_oneFile, "getDirectoryEntries_oneFile" ) );
			Util.runTest( new TestInfo( getDirectoryEntries_oneDir, "getDirectoryEntries_oneDir" ) );
			Util.runTest( new TestInfo( getDirectoryEntries_oneDirWithFiles, "getDirectoryEntries_oneDirWithFiles" ) );
			Util.runTest( new TestInfo( getDirectoryEntries_fileDeleted, "getDirectoryEntries_fileDeleted" ) );
			Util.runTest( new TestInfo( getDirectoryEntries_dirsAndFiles, "getDirectoryEntries_dirsAndFiles" ) );
			Util.runTest( new TestInfo( getDirectoryEntries_relativePath, "getDirectoryEntries_relativePath" ) );
			Util.runTest( new TestInfo( getDirectoryEntries_slashes, "getDirectoryEntries_slashes" ) );
		}

		/**
		 * Pass in a directory which is not there.
		 **/
		public function getDirectoryEntries_dirMissing( info:TestInfo ):TestInfo{
			var arr:Vector.<FileHandle> = CModule.vfs.getDirectoryEntries( "/getDirectoryEntriesInvalidPath" );
			if( arr == null ){
				info.result = "PASS";
			}else{
				info.message += "Null should have been returned.";
			}
			
			return info;
		}

		/**
		 * Pass in a file instead of a directory.
		 **/
		public function getDirectoryEntries_passInFile( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";
			var fileEntry:FileHandle = null;
			var arr:Vector.<FileHandle> = null;

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/foo.txt", data );
			fileEntry = CModule.vfs.getFileHandleFromPath( rootDir + "/foo.txt" );
			if( fileEntry != null ){
				arr = CModule.vfs.getDirectoryEntries( rootDir + "/foo.txt" );
				if( arr == null ){
					info.result = "PASS";
				}else{
					info.message += "Null should have been returned.";
				}
			}else{
				info.message += "Unable to test getDirectoryEntries because addFile is not working.";
			}

			return info;
		}

		/**
		 * Pass in an empty directory.
		 **/
		public function getDirectoryEntries_emptyDir( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var arr:Vector.<FileHandle> = CModule.vfs.getDirectoryEntries( rootDir );

			if( arr != null ){
				if( arr.length == 0 ){
					info.result = "PASS";
				}else{
					info.message += "Expected an empty array.";
				}
			}else{
				info.message += "Null array returned.";
			}

			return info;
		}

		/**
		 * Pass in a directory with one file.
		 **/
		public function getDirectoryEntries_oneFile( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";
			var arr:Vector.<FileHandle> = null;

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/foo.txt", data );
			arr = CModule.vfs.getDirectoryEntries( rootDir );
							
			if( arr != null ){
				if( arr.length == 1 ){
					info = Util.verifyTableEntry( arr[ 0 ], dataString, rootDir + "/foo.txt", false, info );
				}else{
					info.message += "Expected 1 item in the returned array.";
				}
			}else{
				info.message += "Null array was returned.";
			}

			return info;
			
		}

		/**
		 * Pass in directory with one directory.
		 **/
		public function getDirectoryEntries_oneDir( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var arr:Vector.<FileHandle> = null;

			CModule.vfs.addDirectory( rootDir + "/dir1" );
			arr = CModule.vfs.getDirectoryEntries( rootDir );
			
			if( arr != null ){
				if( arr.length == 1 ){
					info = Util.verifyTableEntry( arr[ 0 ], null, rootDir + "/dir1", true, info );
				}else{
					info.message += "Expected 1 item in the returned array.";
				}
			}else{
				info.message += "Null array was returned.";
			}

			return info;
		}

		/**
		 * Pass in a directory with one directory which contains a file.
		 **/
		public function getDirectoryEntries_oneDirWithFiles( info:TestInfo ):TestInfo{
			var rootDir = Util.getCleanMountPoint();
			var arr:Vector.<FileHandle> = null;
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";
			
			data.writeUTFBytes( dataString );
			CModule.vfs.addDirectory( rootDir + "/dir1" );
			CModule.vfs.addFile( rootDir + "/dir1/foo.txt", data );
			arr = CModule.vfs.getDirectoryEntries( rootDir );
			
			if( arr != null ){
				if( arr.length == 1 ){
					info = Util.verifyTableEntry( arr[ 0 ], null, rootDir + "/dir1", true, info );
				}else{
					info.message += "Expected 1 item in the returned array.";
				}
			}else{
				info.message += "Null array was returned.";
			}

			return info;
		}

		/**
		 * Pass in a directory which has had a file deleted.
		 **/
		public function getDirectoryEntries_fileDeleted( info:TestInfo ):TestInfo{
			var rootDir = Util.getCleanMountPoint();
			var arr:Vector.<FileHandle> = null;
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";
			
			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/foo.txt", data );
			CModule.vfs.deleteFile( rootDir + "/foo.txt" );			
			arr = CModule.vfs.getDirectoryEntries( rootDir );
			
			if( arr != null ){
				if( arr.length == 0 ){
					info.result = "PASS";
				}else{
					info.message += "Expected 0 items in the returned array.";
				}
			}else{
				info.message += "Null array was returned.";
			}

			return info;
	
		}
		
		/**
		 * Try a mix of directories and files.  Should be no surprises.
		 **/
		public function getDirectoryEntries_dirsAndFiles( info:TestInfo ):TestInfo{
			var rootDir = Util.getCleanMountPoint();
			var arr:Vector.<FileHandle> = null;
			var data:ByteArray = new ByteArray();
			var dataString:String = "Who would have thought that hamsterdance.com would still be around?";
			var i:int = 0;
			var found:int = 0;

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/foo1.txt", data );
			CModule.vfs.addFile( rootDir + "/foo2.txt", data );
			CModule.vfs.addFile( rootDir + "/foo3.txt", data );

			CModule.vfs.addDirectory( rootDir + "/dir1" );
			CModule.vfs.addFile( rootDir + "/dir1/foo1.txt", data );
			CModule.vfs.addFile( rootDir + "/dir1/foo2.txt", data );
			CModule.vfs.addFile( rootDir + "/dir1/foo3.txt", data );

			CModule.vfs.addDirectory( rootDir + "/dir2" );
			CModule.vfs.addFile( rootDir + "/dir2/foo1.txt", data );
			CModule.vfs.addFile( rootDir + "/dir2/foo2.txt", data );
			CModule.vfs.addFile( rootDir + "/dir2/foo3.txt", data );

			CModule.vfs.addDirectory( rootDir + "/dir2/dir3" );
			CModule.vfs.addFile( rootDir + "/dir2/dir3/foo1.txt", data );
			CModule.vfs.addFile( rootDir + "/dir2/dir3/foo2.txt", data );
			CModule.vfs.addFile( rootDir + "/dir2/dir3/foo3.txt", data );

			arr = CModule.vfs.getDirectoryEntries( rootDir );
			if( arr != null ){
				if( arr.length == 5 ){
					found = 0;
					for( i = 0; i < 5; ++i ){
						if( arr[ i ].path == rootDir + "/foo1.txt" ){
							info = Util.verifyTableEntry( arr[ i ], data, rootDir + "/foo1.txt", false, info );
							++found;
						}else if( arr[ i ].path == rootDir + "/foo2.txt" ){
							info = Util.verifyTableEntry( arr[ i ], data, rootDir + "/foo2.txt", false, info );
							++found;
						}else if( arr[ i ].path == rootDir + "/foo3.txt" ){
							info = Util.verifyTableEntry( arr[ i ], data, rootDir + "/foo3.txt", false, info );
							++found;
						}else if( arr[ i ].path == rootDir + "/dir1" ){
							info = Util.verifyTableEntry( arr[ i ], null, rootDir + "/dir1", true, info );
							++found;
						}else if( arr[ i ].path == rootDir + "/dir2" ){
							info = Util.verifyTableEntry( arr[ i ], null, rootDir + "/dir2", true, info );
							++found;
						}else{
							info.message += "Bailing.  Don't know what to do with " + arr[ i ].path;
							info.result = "FAIL";
							return info;
						}

						if( info.result != "PASS" ){
							return info;
						}
					}

					if( found != 5 ){
						info.result = "FAIL";
						info.message += "Something was missing.";
					}

				}else{
					info.message += "Expected 5 items in the returned array.";
				}
			}else{
				info.message += "Null array was returned.";
			}

			arr = CModule.vfs.getDirectoryEntries( rootDir + "/dir1" );
			if( arr != null ){
				if( arr.length == 3 ){
					found = 0;
					for( i = 0; i < 3; ++i ){
						if( arr[ i ].path == rootDir + "/dir1/foo1.txt" ){
							info = Util.verifyTableEntry( arr[ i ], data, rootDir + "/dir1/foo1.txt", false, info );
							++found;
						}else if( arr[ i ].path == rootDir + "/dir1/foo2.txt" ){
							info = Util.verifyTableEntry( arr[ i ], data, rootDir + "/dir1/foo2.txt", false, info );
							++found;
						}else if( arr[ i ].path == rootDir + "/dir1/foo3.txt" ){
							info = Util.verifyTableEntry( arr[ i ], data, rootDir + "/dir1/foo3.txt", false, info );
							++found;
						}else{
							info.message += "Bailing.  Don't know what to do with " + arr[ i ].path;
							info.result = "FAIL";
							return info;
						}

						if( info.result != "PASS" ){
							return info;
						}
					}

					if( found != 3 ){
						info.result = "FAIL";
						info.message += "Something was missing.";
					}

				}else{
					info.message += "Expected 3 items in the returned array.";
				}
			}else{
				info.message += "Null array was returned.";
			}

			arr = CModule.vfs.getDirectoryEntries( rootDir + "/dir2" );
			if( arr != null ){
				if( arr.length == 4 ){
					found = 0;
					for( i = 0; i < 4; ++i ){
						if( arr[ i ].path == rootDir + "/dir2/foo1.txt" ){
							info = Util.verifyTableEntry( arr[ i ], data, rootDir + "/dir2/foo1.txt", false, info );
							++found;
						}else if( arr[ i ].path == rootDir + "/dir2/foo2.txt" ){
							info = Util.verifyTableEntry( arr[ i ], data, rootDir + "/dir2/foo2.txt", false, info );
							++found;
						}else if( arr[ i ].path == rootDir + "/dir2/foo3.txt" ){
							info = Util.verifyTableEntry( arr[ i ], data, rootDir + "/dir2/foo3.txt", false, info );
							++found;
						}else if( arr[ i ].path == rootDir + "/dir2/dir3" ){
							info = Util.verifyTableEntry( arr[ i ], null, rootDir + "/dir2/dir3", true, info );
							++found;
						}else{
							info.message += "Bailing.  Don't know what to do with " + arr[ i ].path;
							info.result = "FAIL";
							return info;
						}

						if( info.result != "PASS" ){
							return info;
						}
					}

					if( found != 4 ){
						info.result = "FAIL";
						info.message += "Something was missing.";
					}

				}else{
					info.message += "Expected 4 items in the returned array.";
				}
			}else{
				info.message += "Null array was returned.";
			}


			arr = CModule.vfs.getDirectoryEntries( rootDir + "/dir2/dir3" );
			if( arr != null ){
				if( arr.length == 3 ){
					found = 0;
					for( i = 0; i < 3; ++i ){
						if( arr[ i ].path == rootDir + "/dir2/dir3/foo1.txt" ){
							info = Util.verifyTableEntry( arr[ i ], data, rootDir + "/dir2/dir3/foo1.txt", false, info );
							++found;
						}else if( arr[ i ].path == rootDir + "/dir2/dir3/foo2.txt" ){
							info = Util.verifyTableEntry( arr[ i ], data, rootDir + "/dir2/dir3/foo2.txt", false, info );
							++found;
						}else if( arr[ i ].path == rootDir + "/dir2/dir3/foo3.txt" ){
							info = Util.verifyTableEntry( arr[ i ], data, rootDir + "/dir2/dir3/foo3.txt", false, info );
							++found;
						}else{
							info.message += "Bailing.  Don't know what to do with " + arr[ i ].path + ". I most certainly did not add it.";
							info.result = "FAIL";
							return info;
						}

						if( info.result != "PASS" ){
							return info;
						}
					}

					if( found != 3 ){
						info.result = "FAIL";
						info.message += "Something was missing.";
					}

				}else{
					info.message += "Expected 3 items in the returned array.";
				}
			}else{
				info.message += "Null array was returned.";
			}

			return info;
		}

		/**
		 * Pass in a relative path.
		 **/
		public function getDirectoryEntries_relativePath( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "If Survivor were not so popular, would reality TV have taken off as it did?";
			var arr:Vector.<FileHandle> = null;
			var i:int = 0;

			data.writeUTFBytes( dataString );
			CModule.vfs.addDirectory( rootDir + "/dir1" );
			CModule.vfs.addDirectory( rootDir + "/dir1/dir2" );
			CModule.vfs.addDirectory( rootDir + "/dir1/dir2/dir3" );
			CModule.vfs.addFile( rootDir + "/dir1/dir2/foo.txt", data );
			arr = CModule.vfs.getDirectoryEntries( rootDir + "/dir1/./../dir1/dir2/dir3/.."  );
							
			if( arr != null ){
				if( arr.length == 2 ){
					for( i = 0; i < 2; ++i ){
						if( arr[ i ].path.indexOf( ".txt" ) > 0 ){
							info = Util.verifyTableEntry( arr[ i ], dataString, rootDir + "/dir1/dir2/foo.txt", false, info );
						}else{
							info = Util.verifyTableEntry( arr[ i ], null, rootDir + "/dir1/dir2/dir3", true, info );
						}
					}
				}else{
					info.message += "Expected 2 items in the returned array.";
				}
			}else{
				info.message += "Null array was returned.";
			}

			return info;
			
		}

		/**
		 * Pass in multiple slashes and a trailing slash.
		 **/
		public function getDirectoryEntries_slashes( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "Why not simply sell replacement bristles for a toothbrush instead of replacing the entire brush?";
			var arr:Vector.<FileHandle> = null;

			data.writeUTFBytes( dataString );
			CModule.vfs.addDirectory( rootDir + "/dir1" );
			CModule.vfs.addFile( rootDir + "/dir1/foo.txt", data );
			arr = CModule.vfs.getDirectoryEntries( "/" + rootDir + "///dir1/"  );
							
			if( arr != null ){
				if( arr.length == 1 ){
					info = Util.verifyTableEntry( arr[ 0 ], dataString, rootDir + "/dir1/foo.txt", false, info );
				}else{
					info.message += "Expected 1 item in the returned array.";
				}
			}else{
				info.message += "Null array was returned.";
			}

			return info;		
		}
	}
}


