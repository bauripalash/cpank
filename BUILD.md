# Building Pankti Interpreter (Cpank) from source


## Linux 
To build pankti (cpank) you must have a C compiler with support for C11 and 
GNU `make` installed.

* Clone the repo
* run `make build` to build optimized binary

## Windows
To build pankti (cpank) on windows you can either use GCC/Clang or MSVC compiler.
To setup GCC/Clang I recommend installing [MSYS2](https://www.msys2.org/), any 
other method should also work fine, but only MSVC and GCC has been tested.

### GCC/Clang

If you're using MSYS2, install GCC as well as `make`, then open notepad or any 
text editor of your choice and edit `Makefile`. Change the `OUTPUT=pankti` on 
line 16 to `OUTPUT=pankti.exe`, and run `make build`

### MSVC 
you can simply use MSBuild to build the project, you must set required flags
according to your need.

With Visual Studio, just open the solution in `cpankproj` directory and Build 
from the toolbar

