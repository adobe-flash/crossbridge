CHANGES
=======

## Adobe Releases

### 1.0.1 

* Debug builds no longer fails at runtime with Reference Errors mentioning symbols with “THUNK” in their name
* avm2_uithunk no longer fails when content was started using CModule.startAsync() due to incorrect assignment of thread ids
* AS3_GoAsync() can now be used from main when started via CModule.startBackground()
* Adding directories via the InMemoryBackingStore AS3 API with trailing slashes now works

## Community Releases

### 1.0.2 

* Updated BMake from VCS
* Updated CMake to v2.8.12.2
* Updated Make to v4.0
* Updated SWIG to v3.0.0
* Updated PlayerGlobal to v13.0.0
* Merged libs from https://github.com/alexmac/alcextra
* Merged examples from https://github.com/alexmac/alcexamples
* Improved cross-platform compatibility of sources
* Created SourceForge project: http://sourceforge.net/projects/crossbridge-community/

### 1.0.3

* Updated PlayerGlobal to v14.0.0
* Updated ASC2 Compiler to v14.0.0
* Deprecated ObjectiveC support
* Deprecated Legacy ASC support
* Enabled ASC2 parallel compilation
* Improved AS3 logging for threads

### 1.0.4

* Updated AVMPlus from https://github.com/adobe-flash/avmplus