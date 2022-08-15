EXECUTABLES = $(wildcard *.test)
CXX = g++
LDFLAGS = -lpthread
CXXFLAGS = -std=c++14 -g3 -I../src -I../inc

all : test_env.cpp test_auth.o test_batch_request.o test_base64.o
	$(CXX) $(CXXFLAGS) test_env.o ../src/server_config.o ../src/util.o ../src/simple_git_lfs.o ../inc/base64.o ../inc/httplib.o ../inc/json11.o ../src/logger.o -o test_env.test
	$(CXX) $(CXXFLAGS) test_env.cpp ../src/server_config.o ../src/util.o ../src/logger.o -o test_env.test
	$(CXX) $(CXXFLAGS) test_auth.cpp ../src/server_config.o ../src/simple_git_lfs.o ../inc/base64.o ../inc/httplib.o ../inc/json11.o ../src/util.o ../src/logger.o -o test_auth.test
	$(CXX) $(CXXFLAGS) test_batch_request.o ../src/server_config.o ../src/simple_git_lfs.o ../inc/base64.o ../inc/httplib.o ../inc/json11.o ../src/util.o ../src/logger.o -o test_batch_request.test
	$(CXX) $(CXXFLAGS) test_base64.o ../inc/base64.o -o test_base64.test

test_env.cpp : ../src/server_config.o ../src/util.o ../src/simple_git_lfs.o
	$(CXX) $(CXXFLAGS) -c test_env.cpp -o test_env.o

test_auth.o : ../src/server_config.o ../src/simple_git_lfs.o ../inc/httplib.o
	$(CXX) $(CXXFLAGS) -c test_auth.cpp -o test_auth.o

test_batch_request.o : ../src/server_config.o ../src/simple_git_lfs.o ../inc/httplib.o ../inc/json11.o
	$(CXX) $(CXXFLAGS) -c test_batch_request.cpp -o test_batch_request.o

test_base64.o : ../inc/base64.o
	$(CXX) $(CXXFLAGS) -c test_base64.cpp -o test_base64.o

../src/server_config.o :
	$(CXX) $(CXXFLAGS) -c ../src/server_config.cpp -o $@

../src/util.o :
	$(CXX) $(CXXFLAGS) -c ../src/util.cpp -o $@

../src/simple_git_lfs.o : ../inc/json11.o ../inc/httplib.o ../inc/base64.o ../src/logger.o
	$(CXX) $(CXXFLAGS) -c ../src/simple_git_lfs.cpp -o $@

../inc/json11.o :
	$(CXX) $(CXXFLAGS) -c ../inc/json11.cpp -o $@

../inc/httplib.o :
	$(CXX) $(CXXFLAGS) -c ../inc/httplib.cpp -o $@

../inc/base64.o :
	$(CXX) $(CXXFLAGS) -c ../inc/base64.cpp -o $@

../src/logger.o :
	$(CXX) $(CXXFLAGS) -c ../src/logger.cpp -o $@

clean :
	rm *.test
	rm *.o
	rm lfs.log
	rm ../src/*.o
	rm ../inc/*.o

run : $(EXECUTABLES)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done

.PHONY : clean
