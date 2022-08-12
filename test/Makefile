# SOURCES = $(wildcard *.cpp)
EXECUTABLES = $(wildcard *.test)
CXX = g++
LDFLAGS =
CXXFLAGS = -std=c++14 -g3 -I../src -I../inc
# INCS = $(wildcard ../inc/*.cpp)
# INCS_EXE = $(INCS:.cpp=.o)

all :
	$(CXX) $(CXXFLAGS) test_env.cpp ../src/server_config.cpp -o test_env.test
	$(CXX) $(CXXFLAGS) test_auth.cpp ../src/server_config.cpp ../src/simple_git_lfs.cpp ../inc/base64.cpp ../inc/httplib.cpp ../inc/json11.cpp -o test_auth.test
	$(CXX) $(CXXFLAGS) test_batch_request.cpp ../src/server_config.cpp ../src/simple_git_lfs.cpp ../inc/base64.cpp ../inc/httplib.cpp ../inc/json11.cpp -o test_batch_request.test

clean :
	rm *.test

run : $(EXECUTABLES)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done

.PHONY : clean
