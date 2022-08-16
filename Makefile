CXX = g++
CXXFLAGS = -std=c++14 -g3 -Wall -Wpedantic -Wextra -fsanitize=address
OUT = out/
##########################################
## Sources
SRCS = $(addprefix src/, main.cpp server/config.cpp lfs.cpp tool/logger.cpp tool/util.cpp)
SRCS_O = $(SRCS:.cpp=.o)
##########################################
## Includes
INC_3RD_FLD = third_party/
INC_TOOLS = src/tool
INC_SERVER = src/server
INCS_3RD_SRC = $(addprefix $(INC_3RD_FLD), httplib.cpp json11.cpp base64.cpp)
INCS_3RD_O = $(INCS_3RD_SRC:.cpp=.o)
##########################################
## Links
LDFLAGS = -lssl -lcrypto -lpthread

# $^ -> prerequisites reference.
# $@ -> target reference.

all : build out/lfs_server

build :
	mkdir -p $(OUT)

out/lfs_server : $(SRCS_O) $(INCS_3RD_O)
	$(CXX) $(CXXFLAGS) -DCPPHTTPLIB_OPENSSL_SUPPORT=1 -I$(INC_3RD_FLD) $(SRCS_O) $(INCS_3RD_O) -o $@ $(LDFLAGS)

src/main.o :
	$(CXX) $(CXXFLAGS) -DCPPHTTPLIB_OPENSSL_SUPPORT=1 -I$(INC_3RD_FLD) -Isrc/tool -Isrc/server -c src/main.cpp -o $@

src/server/config.o : src/server/config.cpp
	$(CXX) $(CXXFLAGS) -Isrc/tool -c $^ -o $@

src/lfs.o : src/lfs.cpp
	$(CXX) $(CXXFLAGS) -I$(INC_3RD_FLD) -I$(INC_TOOLS) -I$(INC_SERVER) -c $^ -o $@

src/tool/logger.o : src/tool/logger.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

src/tool/util.o : src/tool/util.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(INC_3RD_FLD)httplib.o : $(INC_3RD_FLD)httplib.cpp
	$(CXX) $(CXXFLAGS) -DCPPHTTPLIB_OPENSSL_SUPPORT=1 -c $^ -o $@ $(LDFLAGS)

$(INC_3RD_FLD)json11.o : $(INC_3RD_FLD)json11.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(INC_3RD_FLD)base64.o : $(INC_3RD_FLD)base64.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

clean :
	rm -rf $(OUT)
	rm -f src/*.o
	rm -f inc/*.o
	rm -f lfs.log

# If there is a target called clean/all/run then GNUmake could be confused, so we
# will avoid that, just in case.
.PHONY : all clean run
