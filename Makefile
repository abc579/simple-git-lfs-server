##########################################
## Compiler.
CXX = g++
CXXFLAGS = -std=c++14 -O3 -Wall -Wpedantic -Wextra -fsanitize=address
##########################################
## Sources.
MAIN_SRC = src/main.cpp
MAIN_OBJ = $(MAIN_SRC:.cpp=.o)
#
SV_FILES = $(shell find src/server -type f -name '*.cpp') # $(wildcard src/server/*)
SV_SRCS = $(filter %.cpp, $(SV_FILES))
SV_OBJS = $(SV_SRCS:.cpp=.o)
#
UTIL_FILES = $(wildcard src/util/*)
UTIL_SRCS = $(filter %.cpp, $(UTIL_FILES))
UTIL_OBJS = $(UTIL_SRCS:.cpp=.o)
#
DEP_FILES = $(wildcard third_party/*)
DEP_SRCS = $(filter %.cpp, $(DEP_FILES))
DEP_OBJS = $(DEP_SRCS:.cpp=.o)
#
STOR_FILES  = src/get_storage_option.cpp
STOR_FILES += $(wildcard src/storage/local/*)
STOR_FILES += $(wildcard src/storage/google/*)
STOR_SRCS = $(filter %.cpp, $(STOR_FILES))
STOR_OBJS = $(STOR_SRCS:.cpp=.o)
#
ALL_OBJS = $(MAIN_OBJ)  \
           $(SV_OBJS)   \
           $(UTIL_OBJS) \
           $(STOR_OBJS) \
           $(DEP_OBJS)
##########################################
## Link.
LDFLAGS = -lpthread -lssl -lcrypto
##########################################
## Includes.
INCS = -DCPPHTTPLIB_OPENSSL_SUPPORT=1 -Ithird_party -Isrc/util -Isrc/lfs -Isrc/server -Isrc/storage
##########################################
## TARGET
SLFS = out/lfs_server

all: build $(MAIN_OBJ)
	 $(CXX) $(CXXFLAGS) $(INCS) $(ALL_OBJS) -o $(SLFS) $(LDFLAGS)

$(MAIN_OBJ) : $(DEP_OBJS) $(SV_OBJS) $(UTIL_OBJS) $(STOR_OBJS)
	$(CXX) $(CXXFLAGS) $(INCS) -c $(patsubst %.o, %.cpp, $@) -o $@

$(SV_OBJS) : $(SV_SRCS)
	$(CXX) $(CXXFLAGS) $(INCS) -c $(patsubst %.o, %.cpp, $@) -o $@

$(UTIL_OBJS) : $(UTIL_SRCS)
	$(CXX) $(CXXFLAGS) $(INCS) -c $(patsubst %.o, %.cpp, $@) -o $@

$(DEP_OBJS) : $(DEP_SRCS)
	$(CXX) $(CXXFLAGS) $(INCS) -c $(patsubst %.o, %.cpp, $@) -o $@

$(STOR_OBJS) : $(STOR_SRCS)
	$(CXX) $(CXXFLAGS) $(INCS) -c $(patsubst %.o, %.cpp, $@) -o $@

build :
	mkdir -p out

clean :
	rm -rf out
	rm -f src/*.o
	rm -f third_party/*.o
	rm -f src/storage/*.o	
	rm -f src/storage/local/*.o
	rm -f src/storage/google/*.o
	rm -f src/server/*.o
	rm -f src/server/httplib/*.o
	rm -f src/util/*.o

# If there is a target called clean/all/run then GNUmake could be confused, so we
# will avoid that, just in case.
.PHONY : all clean run
