Standalone Python SWF Utilities
===============================

## add-opt-in.py

Adds the EnableTelemetry tag to a SWF file for use with Adobe Scout.

Run this script on your SWF to make it generate advanced telemetry, which is
needed for the ActionScript Sampler, Stage3D Recording, and other features.

This script is provided as a last resort. If possible, you should compile your
application with the -advanced-telemetry option. 

### Usage

> ./add-opt-in.py swf_file [password]

If password is provided, advanced telemetry will only be visible if a matching 
password is entered in Adobe Scout. 

## swf-info.py

Dumps  information about the structure of a SWF file including the various tags, SWF version, SWF size, etc.

## swfdink.py

Lets you change the SWF version and make the SWF compressed or uncompressed.
          
## projector-dis.py

Breaks apart a shell projector executable into its components.