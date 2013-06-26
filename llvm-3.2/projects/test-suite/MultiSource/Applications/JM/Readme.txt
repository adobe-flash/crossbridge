JM Reference Software Manual
============================

please send comments and additions to suehring@hhi.de

1. Compilation
2. Command line parameters
3. Input/Output file format
4. Configuration files
5. Platform specific notes


1. Compilation
--------------

1.1 Windows
-----------
  
  Workspaces for MS Visual C++ 6.0/2003/2005 are provided with the nams 

    jm.dsw      - MS Visual C++ 6.0
    jm_vc7.sln  - MS Visual C++ 2003
    jm_vc8.sln  - MS Visual C++ 2005

  These contain encoder and decoder projects.


1.2 Unix
-----------

  Before compiling in a UNIX environment please run the "unixprep.sh" script which
  will remove the DOS LF characters from the files and create object directories.

  Makefiles for GNU make are provided in the lencod and ldecod directory.


2. Command line parameters
--------------------------

2.1 Encoder
-----------

    lencod.exe [-h] [-d default-file] [-f file] [-p parameter=value]

  All Parameters are initially taken from DEFAULTCONFIGFILENAME, defined in 
  configfile.h (typically: "encoder.cfg")

  -h
             Show help on parameters.

  -d default-file    
             Use the specified file as default configuration instead of the file in 
             DEFAULTCONFIGFILENAME.  

  -f file    
             If an -f <config> parameter is present in the command line then 
             this file is used to update the defaults of DEFAULTCONFIGFILENAME.  
             There can be more than one -f parameters present.  

  -p parameter=value 

             If -p <ParameterName = ParameterValue> parameters are present then 
             these overide the default and the additional config file's settings, 
             and are themselfes overridden by future -p parameters.  There must 
             be whitespace between -f and -p commands and their respecitive 
             parameters.

2.2 Decoder
-----------

    ldecod.exe decoder.cfg

  The decoder configuration file name must be provided as the first parameter. All
  decoding parameters are read from this file.


3. Input/Output file format
---------------------------

  The source video material is read from raw YUV 4:2:0 data files.
  For output the same format is used.


4. Configuration files
----------------------

  Sample encoder and decode configuration files are provided in the bin/ directory.
  These contain explanatory comments for each parameter.
  
  The generic structure is explained here.

4.1 Encoder
-----------
  <ParameterName> = <ParameterValue> # Comments

  Whitespace is space and \t

  <ParameterName>  are the predefined names for Parameters and are case sensitive.
                   See configfile.h for the definition of those names and their 
                   mapping to configinput->values.

 <ParameterValue> are either integers [0..9]* or strings.
                  Integers must fit into the wordlengths, signed values are generally 
                  assumed. Strings containing no whitespace characters can be used directly.
                  Strings containing whitespace characters are to be inclosed in double 
                  quotes ("string with whitespace")
                  The double quote character is forbidden (may want to implement something 
                  smarter here).

  Any Parameters whose ParameterName is undefined lead to the termination of the program
  with an error message.

  Known bug/Shortcoming:  zero-length strings (i.e. to signal an non-existing file
                          have to be coded as "".
 
4.2 Decoder
-----------
  <value>    #comment

  The values are read in a predefined order. See the example file for details.


5. Platform specific notes
--------------------------
  This section contains hints for compiling and running the JM software on different 
  operating systems.

5.1 MacOS X
-----------
  MacOs X has a UNIX core so most of the UNIX compile process will work. You might need 
  the following modifications:

  a) in Makefile change "CC = $(shell which gcc)" to "CC = gcc"
     (it seems "which" doesn't work)

  b) MacOS X (before "Tiger") doesn't come with ftime. We suggest using a third party ftime 
     implementation, e.g. from:

     http://darwinsource.opendarwin.org/10.3.4/OpenSSL096-3/openssl/crypto/ftime.c

5.2 FreeBSD
-----------
  You might need to add "-lcompat" to LIBS in the Makefiles for correct linking.

