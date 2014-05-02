telemetry-utils
=============== 
Tools to complement Flash's Telemetry feature.

## add-opt-in.py

Adds the EnableTelemetry tag to a SWF file for use with Adobe Scout.

Run this script on your SWF to make it generate advanced telemetry, which is
needed for the ActionScript Sampler, Stage3D Recording, and other features.

This script is provided as a last resort. If possible, you should compile your
application with the -advanced-telemetry option.

### Setup

1. You need Python. I've tested 2.6.1 (Mac) and ActivePython 2.7.2 (Win).
1. For LZMA-compressed SWFs, you need [pylzma](http://www.joachim-bauch.de/projects/pylzma/).

### Usage

        ./add-opt-in.py swf_file [password]

If password is provided, advanced telemetry will only be visible if a matching 
password is entered in Adobe Scout. 
