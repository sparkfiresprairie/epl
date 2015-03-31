#These variables hold the relative path to the gtest root and
#include directories.
#GTEST_DIR = ../../../gtest
#GTEST_INC = $(GTEST_DIR)/include

#A *.a file is a statically linked library.  The library
#has been compiled for 64bit and 32bit architectures.  I
#have assumed you are using a 64bit architecture, but if
#you have a 32bit architecture then be sure to select
#the correct GTEST_LIB location.
#GTEST_LIB = $(GTEST_DIR)/lib/gtest64.a
#GTEST_LIB = $(GTEST_DIR)/lib/gtest32.a

#The following variables can be found on the gnu website,
#however many makefiles change their canonical meaning
#based purely on stylistic preferences:
#  CXX is the compiler that compiles C++ files.
#  CC is the compiler compiles C files.
#  CPP equivalent to '$(CC) -E', but I've seen it used for g++ as well.
#  CXXFLAGS is a list of flags for the $(CXX) compiler.
#See the following link for more information:
#http://www.gnu.org/software/make/manual/html_node/Implicit-Variables.html
CXX = g++

#Compiler flags:
#  -g inserts debugging capabilities in compiled code.
#  -I $(dir) includes $(dir) when searching for header files.
#  -std=c++11 enables c++11 syntax
#  -Wall enables all warnings.
#  -Werror treats warnings as errors and stops compilation (not included).
#  -fmax-errors=1 will stop compilation at the first error
CXXFLAGS = -g -I $(INCLUDE_DIR) -std=c++11 -Wall -Werror -fmax-errors=1 -std=c++0x
#SRCS will list all of the cpp sources.  For this project, I've
#simply invoked "ls *.cpp" to list all cpp files in this directory.
#SRCS = $(shell ls *.cpp)
SRCS = sp13_5.cpp

#patsubst is a makefile function that replaces "pattern.cpp"
#with "pattern.o". Essentially this lists an object file for
#every source file.
OBJS = $(patsubst %.cpp, %.o, $(SRCS))

#These files will contain the dependency information for each
#.cpp file.  GCC generates these with the -MM flag.
DEPS = $(patsubst %.cpp, %.d, $(SRCS))

#TEST is the output name for the test executable. For releases, many 
#makefiles use the TARGET variable.
TEST = test

#Makefiles revolve around targets, recipes, and dependencies.
#If a target is requested, it will check to make sure all
#dependencies are up-to-date.  If they are not, it will run
#the dependency targets first.  If they are up to date, then it 
#will execute the recipe shell script.
#NOTE: the tab before the $(RECIPE) is required
#
#Example:
#$(TARGET): $(DEPENDENCIES)
#	 $(RECIPE)

#"make" and "make all" will both compile the test executable,
#but not run it.
all: $(TEST)

#"make test" will compile and run the test executable.  The '@'
#sign just supresses make from echoing the command.
test: $(TEST)
	@./$(TEST)

#Taken from http://scottmcpeak.com/autodepend/autodepend.html
#<Automatic Dependency Generation>
#pull in all dependency information if it exists
-include $(DEPS)

#The flag -MM produces the headerfile depenencies
#to stdout, which we then redirect to a dependency file (.d).
#The .o file will be recompiled from the .cpp file if a header file
#changes. if the .cpp file changes, the dependencies also potentially change.
%.d: %.cpp
	@$(CXX) $< $(CXXFLAGS) -MM > $@

#Using the generated dependency information, we determine of the
#.o file needs to be recompiled.  If it does, then we simply use the
#.cpp file and the -c flag to avoid linking; we only link at the very end.
#Note that $* is a special variable like $< that simply represents the characters
#matched by the % wildcard.
%.o: %.d
	$(CXX) $*.cpp $(CXXFLAGS) -c -o $@
#<\Automatic Dependency Generation>

#Compile rules for the test
#-pthread signals the compiler to use pthreads.  This is needed by gtest.
#NOTE: $^ inserts the names of all dependencies.
$(TEST): $(OBJS)
	$(CXX) $^ $(GTEST_LIB) $(CXXFLAGS) -pthread -o $@

#"make clean" will delete object files and the test target executable.
clean:
	-rm -rf *.o $(shell ls *.o)
	-rm -rf *.d $(shell ls *.d)
