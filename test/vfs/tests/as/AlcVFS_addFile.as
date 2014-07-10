package tests{
	import flash.display.Sprite;
	import flash.utils.ByteArray;
	import com.adobe.flascc.*;
	import com.adobe.flascc.vfs.*;
	import tests.TestInfo;
 
	public class AlcVFS_addFile extends Sprite{

		/**
		* Constructor
		**/
		public function AlcVFS_addFile():void
		{}

		/**
		* Go.
		**/
		public function runTests():void{
			Util.runTest( new TestInfo( addFile_normal, "addFile_normal" ) );
			Util.runTest( new TestInfo( addFile_noData, "addFile_noData" ) );
			Util.runTest( new TestInfo( addFile_emptyData, "addFile_emptyData" ) );
			Util.runTest( new TestInfo( addFile_invalidPath, "addFile_invalidPath" ) );
			Util.runTest( new TestInfo( addFile_dot, "addFile_dot" ) );
			Util.runTest( new TestInfo( addFile_startWithDot, "addFile_startWithDot" ) );
			Util.runTest( new TestInfo( addFile_manyDots, "addFile_manyDots" ) );
			Util.runTest( new TestInfo( addFile_doubleDot, "addFile_doubleDot" ) );
			Util.runTest( new TestInfo( addFile_startWithDoubleDot, "addFile_startWithDoubleDot" ) );
			Util.runTest( new TestInfo( addFile_manyDoubleDots, "addFile_manyDoubleDots" ) );
			Util.runTest( new TestInfo( addFile_mixedDots, "addFile_mixedDots" ) );
			Util.runTest( new TestInfo( addFile_multipleSlashes, "addFile_multipleSlashes" ) );
			Util.runTest( new TestInfo( addFile_trailingSlash, "addFile_trailingSlash" ) );
			Util.runTest( new TestInfo( addFile_spacesInName, "addFile_spacesInName" ) );
			Util.runTest( new TestInfo( addFile_allSpaces, "addFile_allSpaces" ) );
			Util.runTest( new TestInfo( addFile_alreadyExists, "addFile_alreadyExists" ) );
		}
	
		/**
		 * Supply a normal file.
		 **/
		public function addFile_normal( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/foo.txt", data );
			return Util.verifyFile( rootDir + "/foo.txt", dataString, info );
		}

		/**
		 * Create a file with no data.  This is not allowed.
		 **/
		public function addFile_noData( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var dataString:String = null;
			var errorMessage:String = null;

			try{
				CModule.vfs.addFile( rootDir + "/foo.txt", null );
			}catch( e:Error ){
				errorMessage = e.message;
				if( errorMessage == "data may not be null" ){
					info.result = "PASS";
				}
			}

			return info;
		}

		/**
		 * Create a file with an empty ByteArray. This should be OK.
		 **/
		public function addFile_emptyData( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var dataString:String = "";
			CModule.vfs.addFile( rootDir + "/foo.txt", new ByteArray() );
			return Util.verifyFile( rootDir + "/foo.txt", dataString, info );
		}

		/**
		 * Create a file with an invalid path.
		 */
		public function addFile_invalidPath( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/this/makes/rootDir/invalid/foo.txt", data );
			return Util.verifyFile(  rootDir + "/this/makes/rootDir/invalid/foo.txt", dataString, info );
		}

		/**
		 * Create a file with a dot in a relative path.
		 **/
		public function addFile_dot( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/./foo.txt", data )
			return Util.verifyFile( rootDir + "/foo.txt", dataString, info );
		}

		/**
		 * Start a path with a dot.
		 **/
		public function addFile_startWithDot( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( "." + rootDir + "/foo.txt", data )
			return Util.verifyFile( rootDir + "/foo.txt", dataString, info );
		}

		/**
		 * Create a file with multiple dots in a relative path.
		 **/
		public function addFile_manyDots( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/./././././././foo.txt", data )
			return Util.verifyFile( rootDir + "/foo.txt", dataString, info );	
		}


		/**
		 * Create a file with .. in a relative path.
		 **/
		public function addFile_doubleDot( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";

			data.writeUTFBytes( dataString );
			CModule.vfs.addDirectory( rootDir + "/dir1" );
			CModule.vfs.addDirectory( rootDir + "/dir2" );
			CModule.vfs.addFile( rootDir + "/dir1/dir2/../foo.txt", data )
			return Util.verifyFile( rootDir + "/dir1/foo.txt", dataString, info );
		}

		/**
		 * Start with ..
		 **/
		public function addFile_startWithDoubleDot( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";

			data.writeUTFBytes( dataString );
			CModule.vfs.addDirectory( rootDir + "/dir1" );
			CModule.vfs.addDirectory( rootDir + "/dir2" );
			CModule.vfs.addFile( ".." + rootDir + "/foo.txt", data );
			return Util.verifyFile( rootDir + "/foo.txt", dataString, info );			
		}

		/**
		 * Create a file with multiple .. in a relative path.
		 **/
		public function addFile_manyDoubleDots( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";
			var foundRoot:Boolean = false;
			var foundMid:Boolean = false;
			var foundDeep:Boolean = false;

			data.writeUTFBytes( dataString );
			CModule.vfs.addDirectory( rootDir + "/dir1" );
			CModule.vfs.addDirectory( rootDir + "/dir2" );
			CModule.vfs.addDirectory( rootDir + "/dir3" );
			CModule.vfs.addFile( rootDir + "/dir1/dir2/dir3/../../../foo_root.txt", data );
			CModule.vfs.addFile( rootDir + "/dir1/dir2/dir3/../../../dir1/../dir1/dir2/../../dir1/dir2/dir3/../foo_mid.txt", data );
			CModule.vfs.addFile( rootDir + "/dir1/dir2/dir3/../../../dir1/dir2/../../dir1/dir2/../dir2/dir3/foo_deep.txt", data );
			info = Util.verifyFile( rootDir + "/foo_root.txt", dataString, info );
			info = Util.verifyFile( rootDir + "/foo_mid.txt", dataString, info );
			return Util.verifyFile( rootDir + "/foo_deep.txt", dataString, info );
		}

		/**
		 * Create a file with a combination of . and .. in the path.
		 **/
		public function addFile_mixedDots( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";

			data.writeUTFBytes( dataString );
			CModule.vfs.addDirectory( rootDir + "/dir1" );
			CModule.vfs.addDirectory( rootDir + "/dir2" );
			CModule.vfs.addDirectory( rootDir + "/dir3" );

			CModule.vfs.addFile( "./../../." + rootDir + "/./dir1/.././dir1/dir2/.././../dir1/dir2/dir3/foo.txt", data );
			return Util.verifyFile( rootDir + "/dir1/dir2/dir3/foo.txt", dataString, info );
		}

		/**
		 * Use multiple slashes in the path.
		 **/
		public function addFile_multipleSlashes( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";

			data.writeUTFBytes( dataString );
			CModule.vfs.addDirectory( rootDir + "/dir1" );
			CModule.vfs.addDirectory( rootDir + "/dir2" );
			CModule.vfs.addDirectory( rootDir + "/dir3" );

			CModule.vfs.addFile( rootDir + "//dir1////dir2///foo.txt", data )
			return Util.verifyFile( rootDir + "/dir1/dir2/foo.txt", dataString, info );
		}

		/**
		 * Append a trailing slash.
		 **/
		public function addFile_trailingSlash( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";

			data.writeUTFBytes( dataString );
			CModule.vfs.addDirectory( rootDir + "/dir1/" );
			CModule.vfs.addFile( rootDir + "/dir1/foo.txt/", data )
			return Util.verifyFile( rootDir + "/dir1/foo.txt", dataString, info );
		}

		/**
		 * Put a space in the name.
		 **/
		public function addFile_spacesInName( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/Adobe Systems, Inc.", data )
			return Util.verifyFile( rootDir + "/Adobe Systems, Inc.", dataString, info );
		}

		/**
		 * Make a name that's all spaces.  Why?  Why not?
		 **/
		public function addFile_allSpaces( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data:ByteArray = new ByteArray();
			var dataString:String = "data in foo.txt";

			data.writeUTFBytes( dataString );
			CModule.vfs.addFile( rootDir + "/     ", data )
			return Util.verifyFile( rootDir + "/     ", dataString, info );
		}

		/**
		 * File already exists.  Should throw an error: "cannot create '" + path + "', file exists"
		 **/
		public function addFile_alreadyExists( info:TestInfo ):TestInfo{
			var rootDir:String = Util.getCleanMountPoint();
			var data1:ByteArray = new ByteArray();
			var data2:ByteArray = new ByteArray();
			var dataString1:String = "blue";
			var dataString2:String = "red";
			var err:String = null;

			data1.writeUTFBytes( dataString1 );
			data2.writeUTFBytes( dataString2 );
			CModule.vfs.addFile( rootDir + "/foo.txt", data1 );

			try{
				CModule.vfs.addFile( rootDir + "/foo.txt", data2 );
			}catch( e:Error ){
				err = e.message;
			}

			return Util.verifyFile( rootDir + "/foo.txt", dataString1, info );
		}

	}
}


