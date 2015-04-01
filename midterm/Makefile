CXX = g++
CXXFLAGS = -g -I $(INCLUDE_DIR) -std=c++11 -Wall -Werror -fmax-errors=1 -std=c++0x
SRCS = sp13_5.cpp
OBJS = $(patsubst %.cpp, %.o, $(SRCS))
DEPS = $(patsubst %.cpp, %.d, $(SRCS))
TEST = test
all: $(TEST)
test: $(TEST)
	@./$(TEST)
-include $(DEPS)
%.d: %.cpp
	@$(CXX) $< $(CXXFLAGS) -MM > $@

%.o: %.d
	$(CXX) $*.cpp $(CXXFLAGS) -c -o $@

$(TEST): $(OBJS)
	$(CXX) $^ $(GTEST_LIB) $(CXXFLAGS) -pthread -o $@

clean:
	-rm -rf *.o $(shell ls *.o)
	-rm -rf *.d $(shell ls *.d)
