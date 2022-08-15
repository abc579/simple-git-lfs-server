CXX = g++
CXXFLAGS = -std=c++14 -g3 -Wall -Wpedantic -Wextra -fsanitize=address
OUT = out/
SRCS = $(addprefix src/, main.cpp server_config.cpp simple_git_lfs.cpp logger.cpp util.cpp)
SRCS_O = $(SRCS:.cpp=.o)
INCS = $(addprefix inc/, httplib.cpp json11.cpp base64.cpp)
INCS_O = $(INCS:.cpp=.o)
LDFLAGS = -lssl -lcrypto -lpthread

# $^ -> prerequisites reference.
# $@ -> target reference.

all : build out/lfs_server

build :
	mkdir -p $(OUT)

out/lfs_server : $(SRCS_O) $(INCS_O)
	$(CXX) $(CXXFLAGS) -DCPPHTTPLIB_OPENSSL_SUPPORT=1 -Iinc $(SRCS_O) $(INCS_O) -o out/lfs_server $(LDFLAGS)

src/main.o :
	$(CXX) $(CXXFLAGS) -DCPPHTTPLIB_OPENSSL_SUPPORT=1 -Iinc -c src/main.cpp -o src/main.o

src/server_config.o : src/server_config.cpp
	$(CXX) $(CXXFLAGS) -Iinc -c src/server_config.cpp -o src/server_config.o

src/simple_git_lfs.o : src/simple_git_lfs.cpp
	$(CXX) $(CXXFLAGS) -Iinc -c src/simple_git_lfs.cpp -o src/simple_git_lfs.o

src/logger.o : src/logger.cpp
	$(CXX) $(CXXFLAGS) -c src/logger.cpp -o src/logger.o

src/util.o : src/util.cpp
	$(CXX) $(CXXFLAGS) -c src/util.cpp -o src/util.o

inc/httplib.o : inc/httplib.cpp
	$(CXX) $(CXXFLAGS) -DCPPHTTPLIB_OPENSSL_SUPPORT=1 -c inc/httplib.cpp -o inc/httplib.o $(LDFLAGS)

inc/json11.o : inc/json11.cpp
	$(CXX) $(CXXFLAGS) -c inc/json11.cpp -o inc/json11.o

inc/base64.o : inc/base64.cpp
	$(CXX) $(CXXFLAGS) -c inc/base64.cpp -o inc/base64.o

run :
	./$(OUT)$(MAINT)

clean :
	rm -rf $(OUT)
	rm -f src/*.o
	rm -f inc/*.o
	rm -f lfs.log

# If there is a target called clean/all/run then GNUmake could be confused, so we
# will avoid that, just in case.
.PHONY : all clean run
