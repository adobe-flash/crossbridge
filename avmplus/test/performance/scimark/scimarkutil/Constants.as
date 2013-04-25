//package jnt.scimark2;

public class Constants
{

        public static  var RESOLUTION_DEFAULT:Number = 2.0;  /*secs*/
        public static  var RANDOM_SEED:int = 101010;

        // default: small (cache-contained) problem sizes
        //
        public static  var FFT_SIZE:int = 1024;  // must be a power of two
        public static  var SOR_SIZE:int =100; // NxN grid
    if (CONFIG::desktop) {
        public static  var SPARSE_SIZE_M:int = 1000;
            public static  var SPARSE_SIZE_nz:int = 5000;
    }
    else { // mobile
        public static  var SPARSE_SIZE_M:int = 100;
            public static  var SPARSE_SIZE_nz:int = 500;
    }
        public static  var LU_SIZE:int = 100;

        // large (out-of-cache) problem sizes
        //
        public static  var LG_FFT_SIZE:int = 1048576;  // must be a power of two
        public static  var LG_SOR_SIZE:int =1000; // NxN grid
        public static  var LG_SPARSE_SIZE_M:int = 100000;
        public static  var LG_SPARSE_SIZE_nz:int =1000000;
        public static  var LG_LU_SIZE:int = 1000;

        // tiny problem sizes (used to mainly to preload network classes
        //                     for applet, so that network download times
        //                     are factored out of benchmark.)
        //
        public static  var TINY_FFT_SIZE:int = 16;  // must be a power of two
        public static  var TINY_SOR_SIZE:int =10; // NxN grid
        public static  var TINY_SPARSE_SIZE_M:int = 10;
        public static  var TINY_SPARSE_SIZE_N:int = 10;
        public static  var TINY_SPARSE_SIZE_nz:int = 50;
        public static  var TINY_LU_SIZE:int = 10;

}

