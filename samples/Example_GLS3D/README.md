OpenGL For Stage3D
==================

This project provides an experimental implementation of the OpenGL API for use in the Flash Player. Behind the scenes all of the OpenGL APIs are implemented ontop of the Flash Player's GPU accelerated Stage3D rendering API. The implementation is intended to be used by existing C/C++ code compiled with flascc to target the Flash Player.

- More information about flascc: http://gaming.adobe.com/technologies/flascc
- More information about Stage3D: http://gaming.adobe.com/

Demos
=====

- Neverball: (http://neverball.org/) is currently the showcase application for this project as it renders well and has good performance
- Quake2: renders almost perfectly, but due to its use of old-school immediate mode OpenGL it doesn't map very well onto Stage3D so the performance isn't great at this time (Might be a good test case for VBO support if there are patches to the Q2 code to support them).
- Quake3: renders correctly if lightmaps are disabled as they require multi-texturing to work correctly. Performance is better than Quake2 due to the use of vertex arrays)
- cube2: (http://sauerbraten.org/) Not yet rendering correctly as the engine requires multi-texturing at the very least.


TODO
====

The project should be considered experimental at this point and still has many features that need to be implemented. In descending priority order I would say that the next things that need to be done are:

- implement multitexturing.
- implement support for VBOs.
- integrate the glsl2agal project so we can support GLSL.
- migrate more of libGL.as into the C code.


License
=======

The Adobe written portion of this project is provided under the MIT license. The headers for OpenGL come from the Mesa project. The examples are all licensed differently, please check carefully before reusing any of their code.