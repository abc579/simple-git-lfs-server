EXECUTABLES	 = $(wildcard *.test)
CXX = g++
LDFLAGS = -lpthread
CXXFLAGS = -std=c++14 -g3 -I../src -I../third_party -I../src/tool -I../src/server

all : test_env.o test_auth.o test_batch_request.o test_base64.o
	$(CXX) $(CXXFLAGS) test_env.o ../src/server/config.o ../src/tool/util.o ../src/lfs.o ../src/tool/logger.o ../third_party/base64.o ../third_party/httplib.o ../third_party/json11.o -o test_env.test
	$(CXX) $(CXXFLAGS) test_env.cpp ../src/server/config.o ../src/tool/util.o ../src/tool/logger.o -o test_env.test
	$(CXX) $(CXXFLAGS) test_auth.cpp ../src/server/config.o ../src/lfs.o ../src/tool/util.o ../src/tool/logger.o ../third_party/base64.o ../third_party/httplib.o ../third_party/json11.o -o test_auth.test
	$(CXX) $(CXXFLAGS) test_batch_request.o ../src/server/config.o ../src/lfs.o ../src/tool/util.o ../src/tool/logger.o ../third_party/base64.o ../third_party/httplib.o ../third_party/json11.o -o test_batch_request.test
	$(CXX) $(CXXFLAGS) test_base64.o ../third_party/base64.o -o test_base64.test

test_env.o : ../src/server/config.o ../src/tool/util.o ../src/lfs.o
	$(CXX) $(CXXFLAGS) -c test_env.cpp -o test_env.o

test_auth.o : ../src/server/config.o ../src/lfs.o ../third_party/httplib.o
	$(CXX) $(CXXFLAGS) -c test_auth.cpp -o test_auth.o

test_batch_request.o : ../src/server/config.o ../src/lfs.o ../third_party/httplib.o ../third_party/json11.o
	$(CXX) $(CXXFLAGS) -c test_batch_request.cpp -o test_batch_request.o

test_base64.o : ../third_party/base64.o
	$(CXX) $(CXXFLAGS) -c test_base64.cpp -o test_base64.o

../src/server/config.o : ../src/server/config.cpp
	$(CXX) $(CXXFLAGS) -c ../src/server/config.cpp -o $@

../src/util.o : ../src/tool/util.cpp
	$(CXX) $(CXXFLAGS) -c ../src/tool/util.cpp -o $@

../src/lfs.o : ../third_party/json11.o ../third_party/httplib.o ../third_party/base64.o ../src/tool/logger.o ../src/lfs.cpp
	$(CXX) $(CXXFLAGS) -c ../src/lfs.cpp -o $@

../third_party/json11.o : ../third_party/json11.cpp
	$(CXX) $(CXXFLAGS) -c ../third_party/json11.cpp -o $@

../third_party/httplib.o : ../third_party/httplib.cpp
	$(CXX) $(CXXFLAGS) -c ../third_party/httplib.cpp -o $@

../third_party/base64.o : ../third_party/base64.cpp
	$(CXX) $(CXXFLAGS) -c ../third_party/base64.cpp -o $@

../src/tool/logger.o : ../src/tool/logger.cpp
	$(CXX) $(CXXFLAGS) -c ../src/tool/logger.cpp -o $@

clean :
	rm -f *.test
	rm -f *.o
	rm -f lfs.log
	rm -f ../src/*.o ../src/tool/*.o ../src/server/*.o 
	rm -f ../third_party/*.o

run : $(EXECUTABLES)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done

.PHONY : clean
