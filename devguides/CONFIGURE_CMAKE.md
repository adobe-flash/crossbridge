
Usage: /cygdrive/f/crossbridge/build/win/cmake/bootstrap [<options>...] [-- <cmake-options>...]
Options: [defaults in brackets after descriptions]
Configuration:
  --help                  print this message
  --version               only print version information
  --verbose               display more information
  --parallel=n            bootstrap cmake in parallel, where n is
                          number of nodes [1]
  --enable-ccache         Enable ccache when building cmake
  --init=FILE             load FILE as script to populate cache
  --system-libs           use all system-installed third-party libraries
                          (for use only by package maintainers)
  --no-system-libs        use all cmake-provided third-party libraries
                          (default)
  --system-curl           use system-installed curl library
  --no-system-curl        use cmake-provided curl library (default)
  --system-expat          use system-installed expat library
  --no-system-expat       use cmake-provided expat library (default)
  --system-zlib           use system-installed zlib library
  --no-system-zlib        use cmake-provided zlib library (default)
  --system-bzip2          use system-installed bzip2 library
  --no-system-bzip2       use cmake-provided bzip2 library (default)
  --system-libarchive     use system-installed libarchive library
  --no-system-libarchive  use cmake-provided libarchive library (default)

  --qt-gui                build the Qt-based GUI (requires Qt >= 4.2)
  --no-qt-gui             do not build the Qt-based GUI (default)
  --qt-qmake=<qmake>      use <qmake> as the qmake executable to find Qt

Directory and file names:
  --prefix=PREFIX         install files in tree rooted at PREFIX
                          [/usr/local]
  --datadir=DIR           install data files in PREFIX/DIR
                          [share/cmake-2.8]
  --docdir=DIR            install documentation files in PREFIX/DIR
                          [doc/cmake-2.8]
  --mandir=DIR            install man pages files in PREFIX/DIR/manN
                          [man]

