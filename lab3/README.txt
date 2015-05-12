README

1. Without alteration, the project will not compile due to undefined references.  "Undefined references" means that the linker found a function declaration (in this case, in Lifeform.h), but found no corresponding function definition in any source files.  Lifeform.h declares a bunch of interface functions, but Lifeform-Craig.cpp only implements a subset.  The assignment asks that you implement the remaining functions in Lifeform.cpp.

2. Doing "make clean" or some make target that depends on "clean" may give a message that looks like an error claiming "Makefile ##: .FileName.d: No such file or directory".  This is a harmless bug in the Makefile that can be ignored.  Let me know if it actually causes any issues.

3. If you get an error message that says something to the degree of "X11/Xlib.h: No such file or directory", it means that you are missing the Xlib library, which I believe is responsible for the graphical window of the simulation.  If you are on Ubuntu, the following command resolves this issue: sudo apt-get install libx11-dev

4. To change the parameters of the simulation, you may take a look at the variables inside Param.cpp as well as config.test. The config.test file designates how many and which life forms to initialize at start up, taking the format of: [species_name] [quantity].