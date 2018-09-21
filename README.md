# libgul

The general utility library contains often-used utility functions and types that form the foundation for other libraries and programs. The main fields covered by the library are:


  * Strings
  * Exceptions
  * Time
  * Numerical helpers
  * Backports from new standard library extensions

# Building

Clone this repository:

        git clone ssh://git@stash.desy.de:7999/gul/libgul.git
        cd libgul

Use meson to configure the build you want to have. Build directory names can be what you want. If you use directories starting with 'build' they will be ignored by git. Examples:

        meson -D warning_level=3 build
        meson -D warning_level=3 -D tests=false build_without_tests
        meson -D buildtype=release --prefix /usr build_rel
        meson -D buildtype=release --prefix /usr -D unity=on build_rel_unity
        meson -D buildtype=release --prefix /usr -D docs=true build_docs_rel

Afterwards enter you build directory and fire the build process:

        cd build_rel_unity
        ninja
        ninja test
        ninja resources/docs
        makeDdeb -D

## Notes

1. To enable debian packaging the meson build directory needs to be configured with ``--prefix /usr``.
1. You are encouraged to use a recent version of meson. Meson has no dependencies except phyton. It does not have to be compiled or something. Just do ``pip3 install --user meson``. This will prevent some unneeded things that are complicated to fix in the build specifications. (Make sure the new meson is in the $PATH before /usr/bin/meson.)
1. You can list all possible build configuration switches with ``meson configure`` in an existing build directory [1]. This command can also be used to change build configurations after the directory has been created. For example to increase the warning level after the build directory has already been created: ``meson configure -D warning_level=3`` [2].
2. The configuration to be changed can immediately follow the ``-D`` for example ``-Dwarning_level=3``.
3. You must call ``ninja`` in the build directory before you call ``makeDdeb`` once. The first call sets up the build directory for makeDdeb to run. Afterwards you can always trigger rebuilds by just calling makeDdeb. (The only exception is changes in debian/, run ninja always after you changed a file in debian/.)
4. ``makeDdeb`` has to be called in the build directory of choice, usually with buildtype = release

[1] Ancient versions of meson do not support this, unfortunately.
[2] Use ``mesonconf`` if your meson is too old.

## Build configuration switches

In addidion to meson's standard switches there are:

    Option Default Value Possible Values Description
    ------ ------------- --------------- -----------
    docs   false         [true, false]   generate documentation via doxygen
    tests  true          [true, false]   generate tests
