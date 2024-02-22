CXX = g++

BASE_FLAGS = -std=c++23 -Wall -Wextra -pedantic
LDFLAGS = -lfmt

SOURCES = $(wildcard fastmathart/*.cpp)\
	$(wildcard fastmathart/*/*.cpp)
TEST_SOURCES = $(wildcard tests/*.cpp)

test: $(SOURCES) $(TEST_SOURCES)
	$(CXX) $(BASE_FLAGS) -o test $(SOURCES) $(TEST_SOURCES) $(LDFLAGS) -g -pg
	./test