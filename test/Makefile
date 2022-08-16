##########################################
## Compiler.
CXX = g++
CXXFLAGS = -std=c++14 -g3 -I../src -I../third_party -I../src/tool -I../src/server
##########################################
## Sources.
TEST_FILES = $(wildcard *)
TEST_SRCS = $(filter %.cpp, $(TEST_FILES))
TEST_OBJS = $(TEST_SRCS:.cpp=.test)
#
LFS_FILES = $(wildcard ../src/lfs/*)
LFS_SRCS = $(filter %.cpp, $(LFS_FILES))
LFS_OBJS = $(LFS_SRCS:.cpp=.o)
#
SV_FILES = $(wildcard ../src/server/*)
SV_SRCS = $(filter %.cpp, $(SV_FILES))
SV_OBJS = $(SV_SRCS:.cpp=.o)
#
LOG_FILES = $(wildcard ../src/log/*)
LOG_SRCS = $(filter %.cpp, $(LOG_FILES))
LOG_OBJS = $(LOG_SRCS:.cpp=.o)
#
UTIL_FILES = $(wildcard ../src/util/*)
UTIL_SRCS = $(filter %.cpp, $(UTIL_FILES))
UTIL_OBJS = $(UTIL_SRCS:.cpp=.o)
#
DEP_FILES = $(wildcard ../third_party/*)
DEP_SRCS = $(filter %.cpp, $(DEP_FILES))
DEP_OBJS = $(DEP_SRCS:.cpp=.o)
#
ALL_OBJS = $(LFS_OBJS)  \
           $(SV_OBJS)   \
           $(LOG_OBJS)  \
           $(UTIL_OBJS) \
           $(DEP_OBJS)
##########################################
## Link.
LDFLAGS = -lpthread -lssl -lcrypto
## Includes.
INCS = -DCPPHTTPLIB_OPENSSL_SUPPORT=1 -I../third_party -I../src/util -I../src/log -I../src/lfs -I../src/server

all : $(TEST_OBJS)

$(TEST_OBJS) : $(DEP_OBJS) $(LFS_OBJS) $(SV_OBJS) $(LOG_OBJS) $(UTIL_OBJS)
	$(CXX) $(CXXFLAGS) $(INCS) $(ALL_OBJS) $(patsubst %.test, %.cpp, $@) -o $@ $(LDFLAGS)

$(LFS_OBJS) : $(LFS_SRCS)
	$(CXX) $(CXXFLAGS) $(INCS) -c $(patsubst %.o, %.cpp, $@) -o $@

$(SV_OBJS) : $(SV_SRCS)
	$(CXX) $(CXXFLAGS) $(INCS) -c $(patsubst %.o, %.cpp, $@) -o $@

$(LOG_OBJS) : $(LOG_SRCS)
	$(CXX) $(CXXFLAGS) $(INCS) -c $(patsubst %.o, %.cpp, $@) -o $@

$(UTIL_OBJS) : $(UTIL_SRCS)
	$(CXX) $(CXXFLAGS) $(INCS) -c $(patsubst %.o, %.cpp, $@) -o $@

$(DEP_OBJS) : $(DEP_SRCS)
	$(CXX) $(CXXFLAGS) $(INCS) -c $(patsubst %.o, %.cpp, $@) -o $@

clean :
	rm -f ../src/*.o
	rm -f ../third_party/*.o
	rm -f ../src/lfs/*.o
	rm -f ../src/log/*.o
	rm -f ../src/server/*.o
	rm -f ../src/util/*.o
	rm -f lfs.log
	rm -f *.test


run : $(TEST_OBJS)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done

.PHONY : clean
