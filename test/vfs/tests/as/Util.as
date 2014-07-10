package tests{
	import flash.utils.ByteArray;
	import com.adobe.flascc.vfs.*;
	import com.adobe.flascc.*;
	import tests.TestInfo;

	public class Util{
		public static var TOTAL_AS_PASSES = 0;
		public static var TOTAL_AS_FAILURES = 0;
	
		/**
		* Trace the totals.
		**/
		public static function traceTotals(){
			trace( "TOTAL_AS_PASSES=" + TOTAL_AS_PASSES + " TOTAL_AS_FAILURES=" + TOTAL_AS_FAILURES );
		}

		/**
		* Can we get a function's name?
		**/
		public static function runTest( info:TestInfo ){
			
			reportResult( info.func( info ) );

			//reportResult( f.call, fName );
			//for( var o:Object in this ){
			//	trace("o: " + o);
			//}
			//reportResult( f.call(), fName );
			//trace("this: " + this);
			//var foo:Function = new Function( fName );
			//foo();
		}

		/**
		* Eventually, remove and remount origMountPoint. (ALC-320)
		* For now, just return a new one.
		**/
		public static function getCleanMountPoint( origMountPoint:String = null ):String{
			var base:String = "/store";
			var num:int = 0;
			var retDir = null;
			var exists:Boolean = true;
			
			while( exists ){
				retDir = base + num.toString();
				if( CModule.vfs.getFileHandleFromPath( retDir ) != null ){
					exists = true;
					num++;
				}else{
					exists = false;
				}
			}

			// Now we have an unused string.
			CModule.vfs.addDirectory( retDir );
			return retDir;
		}

		/**
		* Given a file or path (as a String), return the BackingStore.
		**/
		public static function getBackingStore( fileOrDir:String ):IBackingStore{
			return CModule.vfs.getFileHandleFromPath( fileOrDir ).backingStore;
		}

		/**
		* Standard way to report results.
		* Maybe tests should return a test info. structure instead, since there
		* are a few pieces of data and that global testInfo.
		**/
		public static function reportResult( info:TestInfo ){
			if( info != null ){
				var output:String = "RESULT=" + info.result;
				output += " TEST=" + info.name;
				
				if( info.result == "PASS" ){
					Util.TOTAL_AS_PASSES++;
				}else if( info.result == "FAIL" ){
					Util.TOTAL_AS_FAILURES++;
				}

				if( info.result != "PASS" ){
					output += " MSG=" + info.message;
				}

				trace( output );
			}else{
				trace( "reportResult: Received null info." );
			}
		}

		/**
		* Compares an Object containing strings (returned by readFiles()) to an array of
		* objects (submitted to createFiles()).  Returns the provided TestInfo object, with
		* the result toggled to PASS if necessary.
		**/
		public static function compareActualToSubmitted( allFilesFound:Object, filesExpected:Array, rootDir:String, info:TestInfo ):TestInfo{
			var found:Boolean = false;
			var dataCorrect:Boolean = false;
			var missing:Boolean = false;
			var foundTooMany:Boolean = false;	
			var fileEntry:FileHandle = null;

			// Make sure everything we find in that directory on the file system is part of what we submitted.

			for( var fileFound:String in allFilesFound ){
				if( fileFound.indexOf( rootDir ) > -1 && fileFound != rootDir ){
					found = false;
					for( var i:int = 0; i < filesExpected.length; ++i ){
						if( fileFound == filesExpected[ i ].path ){
							found = true;
						}
					}
					if( !found ){
						info.message += fileFound + " was found in the file system, but we did not add it.\n";
						missing = true;
					}
				}
			}

			// Make sure everything we submitted was added, added just once, and has correct data.
			for( var i:int = 0; i < filesExpected.length; ++i ){
				found = false;
				for( var fileFound:String in allFilesFound ){
					if( fileFound == filesExpected[ i ].path ){
						if( found ){
							info.message += filesExpected[ i ].path + " was added more than once.\n";
							foundTooMany = true;
						}
						
						found = true;
				
						if( found ){
							// Now check the data.
							fileEntry = CModule.vfs.getFileHandleFromPath( filesExpected[ i ].path );
							if( ( filesExpected[ i ].data == null ) == fileEntry.isDirectory ){
								if( filesExpected[ i ].data != null ){
									if( filesExpected[ i ].data == fileEntry.bytes.toString() ){
										dataCorrect = true;
									}else{
										info.message += "Found data \"" + fileEntry.bytes.toString() + "\", but expected \"" + filesExpected[ i ].data + "\".";
									}
								}else if( fileEntry.bytes == null ){
									dataCorrect = true;
								}else{
									info.message += "Expected no data, but found some.";
								}
							}else{
								info.message += "Expected a file or directory, but got a directory or file instead.";
							}
						}						
					}
				}
				if( !found ){
					info.message += filesExpected[i].path + " was submitted for creation, but was not created.\n";
					missing = true;
				}
			}
	
			if( !foundTooMany && !missing && dataCorrect ){
				info.result = "PASS";
			}

			return info;
		}

		/**
		* Receives a path like "/store0" and creates the necessary subdirs.  We are just
		* creating objects in an array to pass to createFiles; we are not actually creating them.
		* Example, given rootDir = "/store0":
		*	createSubdirs( rootDir, 0, 2, 3, arr )
		* will create an array of objects which each have paths of:
		* 	/store0/0-0
		* 	/store0/0-0/1-0
		* 	/store0/0-0/1-1
		* 	/store0/0-0/1-2
		* 	/store0/0-1
		* 	/store0/0-1/1-0
		* 	/store0/0-1/1-1
		* 	/store0/0-1/1-2
		* 	/store0/0-2
		* 	/store0/0-2/1-0
		* 	/store0/0-2/1-1
		* 	/store0/0-2/1-2 
		**/
		public static function createSubdirs( rootDir:String, curDepth:int, maxDepth:int, maxEntries:int, allDirs:Array = null ):Array{
			if( allDirs == null ){
				allDirs = new Array();
			}

			for( var i:int = 0; i < maxEntries; ++i ){
				// Create an entry (a subdirectory).
				var newDir:Object = new Object();
				newDir.path = rootDir + "/" + curDepth.toString() + "-" + i.toString();
				allDirs.push( newDir );
				
				// Give the new subdir some subdirs if we need to go deeper.
				if( curDepth < maxDepth - 1 ){
					createSubdirs( newDir.path, curDepth + 1, maxDepth, maxEntries, allDirs );	
				}
			}
			return allDirs;
		}

		/**
		* Utility function to create/return and object that can be pushed
		* into the array that is given to createFiles.
		**/
		public static function createFSObject( thePath:String, theData:String = null ):Object{
			var obj:Object = new Object();
			obj.path = thePath;
			if( theData != null ){
				obj.data = new ByteArray();
				obj.data.writeUTFBytes( theData );
			}
			return obj;
		}

		/**
		* Verify a file.  Return the info. object.
		**/	
		public static function verifyFile( path:String, expectedData:String, info:TestInfo ):TestInfo{
			var fileEntry:FileHandle = CModule.vfs.getFileHandleFromPath( path );
			if( fileEntry != null ){
				if( fileEntry.bytes.toString() == expectedData ){
					if( fileEntry.isDirectory == false ) {
						info.result = "PASS";
					}else{
						info.message += " Got a directory, expected a file.";
					}
				}else{
					info.message += " Data was not correct.";
				}
			}else{
				info.message += " FileHandle was null.";
			}

			return info;	
		}

		/**
		* Verify a file, given an FileHandle.  Return the info. object.
		**/	
		public static function verifyTableEntry( entry:FileHandle, expectedData:String, expectedPath:String, isDir:Boolean, info:TestInfo ):TestInfo{
			//trace( "verifyTableEntry:" );
			//trace( "\tentry.path=" + entry.path );
			//trace( "\texpectedPath=" + expectedPath );
			//trace( "\tentry.isDirectory=" + entry.isDirectory );
			//trace( "\tisDir=" + isDir );
			
			var result:String = "FAIL";


			if( entry != null ){
				if( entry.isDirectory == isDir ){
					if( entry.path == expectedPath ){
						if( expectedData == null && entry.bytes == null ){
							result = "PASS";
						}else{
							if( entry.bytes != null ){
								if( entry.bytes.toString() == expectedData ){
									result = "PASS";
								}else{
									info.message += " Data was not correct.";
								}
							}else{
								info.message += " entry.bytes was null.";
							}
						}
					}else{
						info.message += " Path was " + entry.path + ", expected " + expectedPath + ".";
					}
				}else{
					info.message += " isDirectory was " + entry.isDirectory + " for " + entry.path + ".";
				}
			}else{
				info.message += " FileHandle was null.";
			}

			info.result = result;

			return info;	
		}
	}
}



