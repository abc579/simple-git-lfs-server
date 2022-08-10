# TODO(lev): I definitely need to learn about GNUmake.
CXX = g++
CXXFLAGS = -std=c++14 -g3 -Wall -Wpedantic -Wextra -fsanitize=address -Iinc

all :
	$(CXX) $(CXXFLAGS) src/main.cpp src/server_config.cpp inc/httplib.cpp -o out/lfs_server

run :
	./out/lfs_server

# If there is a target called clean/all/run then GNUmake could be confused, so we
# will avoid that, just in case.
.PHONY : all clean run

clean :
	rm -rf out
