##########################################
## Compiler.
CXX = g++
CXXFLAGS = -std=c++14 -O2 -Wall -Wpedantic -Wextra -fsanitize=address
##########################################
## Sources.
MAIN_SRC = src/main.cpp
MAIN_OBJ = $(MAIN_SRC:.cpp=.o)
#
LFS_FILES = $(wildcard src/lfs/*)
LFS_SRCS = $(filter %.cpp, $(LFS_FILES))
LFS_OBJS = $(LFS_SRCS:.cpp=.o)
#
SV_FILES = $(wildcard src/server/*)
SV_SRCS = $(filter %.cpp, $(SV_FILES))
SV_OBJS = $(SV_SRCS:.cpp=.o)
#
LOG_FILES = $(wildcard src/log/*)
LOG_SRCS = $(filter %.cpp, $(LOG_FILES))
LOG_OBJS = $(LOG_SRCS:.cpp=.o)
#
UTIL_FILES = $(wildcard src/util/*)
UTIL_SRCS = $(filter %.cpp, $(UTIL_FILES))
UTIL_OBJS = $(UTIL_SRCS:.cpp=.o)
#
DEP_FILES = $(wildcard third_party/*)
DEP_SRCS = $(filter %.cpp, $(DEP_FILES))
DEP_OBJS = $(DEP_SRCS:.cpp=.o)
#
ALL_OBJS = $(MAIN_OBJ)  \
           $(LFS_OBJS)  \
           $(SV_OBJS)   \
           $(LOG_OBJS)  \
           $(UTIL_OBJS) \
           $(DEP_OBJS)
##########################################
## Link.
LDFLAGS = -lpthread -lssl -lcrypto
##########################################
## Includes.
INCS = -DCPPHTTPLIB_OPENSSL_SUPPORT=1 -Ithird_party -Isrc/util -Isrc/log -Isrc/lfs -Isrc/server
##########################################
## TARGET
SLFS = out/lfs_server

all: build $(MAIN_OBJ)
	 $(CXX) $(CXXFLAGS) $(INCS) $(ALL_OBJS) -o $(SLFS) $(LDFLAGS)

$(MAIN_OBJ) : $(DEP_OBJS) $(LFS_OBJS) $(SV_OBJS) $(LOG_OBJS) $(UTIL_OBJS)
	$(CXX) $(CXXFLAGS) $(INCS) -c $(patsubst %.o, %.cpp, $@) -o $@

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

build :
	mkdir -p out

clean :
	rm -rf out
	rm -f src/*.o
	rm -f third_party/*.o
	rm -f src/lfs/*.o
	rm -f src/log/*.o
	rm -f src/server/*.o
	rm -f src/util/*.o
	rm -f lfs.log

# If there is a target called clean/all/run then GNUmake could be confused, so we
# will avoid that, just in case.
.PHONY : all clean run
