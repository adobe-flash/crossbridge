CHANGES
=======

## Adobe Releases

### 1.0.1 

* Debug builds no longer fails at runtime with Reference Errors mentioning symbols with “THUNK” in their name
* avm2_uithunk no longer fails when content was started using CModule.startAsync() due to incorrect assignment of thread ids
* AS3_GoAsync() can now be used from main when started via CModule.startBackground()
* Adding directories via the InMemoryBackingStore AS3 API with trailing slashes now works

## Community Releases

### 15.0.0 

* Aligned version scheme to Adobe AIR SDK
* Added LibOpenSSL v1.0.1i
* Added LibXML2
* Added MCrypt Cryptography library
* Added MHash Hashing library
* Added BeeCrypt Cryptography library
* Added Nettle Cryptography library
* Added GLS3D library
* Deprecated ObjectiveC support
* Deprecated Legacy ASC support
* Updated BMake from VCS
* Updated CMake to v3.0.0
* Updated Make to v4.0
* Updated SWIG to v3.0.0
* Updated 3rd party libraries
* Updated PlayerGlobal to v15.0.0
* Updated ASC2 Compiler to v14.0.0
* Updated AVMPlus from https://github.com/adobe-flash/avmplus
* Merged libraries from https://github.com/alexmac/alcextra
* Merged examples from https://github.com/alexmac/alcexamples
* Moved Examples to https://github.com/crossbridge-community
* Migrated SourceForge project: http://sourceforge.net/projects/crossbridge-community/
* Minor bug fixes
* Improved ASC2 compilation
* Improved cross-platform compatibility of sources
* Improved AS3 CModule logging
* Improved AS3 CModule performance

### 16.0.0

TBA