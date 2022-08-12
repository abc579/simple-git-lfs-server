SOURCES = $(wildcard *.cpp)
EXECUTABLE = $(SOURCES:.cpp=.test)
LDFLAGS =
CXXFLAGS = -std=c++14 -O2 -Wall -Werror -Wextra -Wpedantic -fsanitize=address -I../src -I../inc

all : $(EXECUTABLE)

$(EXECUTABLE) : $(SOURCES)
	$(CXX) $< $(LDFLAGS) $(CXXFLAGS) ../src/server_config.cpp -o $@

clean:
	rm $(EXECUTABLE)

run : $(EXECUTABLE)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done
