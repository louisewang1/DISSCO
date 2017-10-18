# DISSCO
# DISSCO
For more information, please see the homepage at: http://ems.music.illinois.edu/software/dissco/index.php

Building CMOD
================================================================================
To create the build files you must have premake4 installed:
http://industriousone.com/premake
 
Then create build files:
premake4

To clean all build files:
premake4 clean

   
Then to make CMOD:
make

CMOD is always built in the root level (instead of in 'bin')

Building Debugging Binaries
================================================================================
At any time you can use:
make config=debug

to build binaries that contain symbols. Then you can run gdb:
gdb cmod
...
run
(assuming a crash happens at runtime)
backtrace

Building the LASS Examples
================================================================================
To create build files including the LASS examples:
premake4 --examples

To clean all build files:
premake4 clean

Then to make all the examples (and CMOD):
make

LASS examples are built in the 'bin' directory.

To make just a specific binary just indicate which example you want to make:
make cmod
make sample0

To create a new LASS example, simply make a .cpp file in the LASS/examples
directory and rerun:
premake4 --examples

Note: the makefiles for each LASS example are self-deleting. When a makefile
successfully builds the binary for its project, it deletes its makefile and
deletes all the object files associated with the build. This has been done to
help mitigate file congestion in the root level of DISSCO.

There are a few LASS examples which do not build and so are not included in the
build. These have been renamed to a .cxx extension to indicate that they need to
be fixed.

Viewing the Source Code Documentation
================================================================================
Change directory to 'docs' and view the README there. If you have doxygen
installed then do:

./view-latest-docs.sh

to generate the documentation and view it in Firefox.

If you already have the documentation generated you can just do:

./view-docs.sh

Regenerating the Parser
================================================================================
The parser generator requires Flex and Bison. View the README in
CMOD/src/lexyacc to learn how to change the parser. The script that updates the
parser is called 'regenerate-parser.sh'
