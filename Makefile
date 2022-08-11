CXX = g++
CXXFLAGS = -std=c++14 -g3 -Wall -Wpedantic -Wextra -fsanitize=address -Iinc
OUT = out/
OBJS = $(addprefix $(OUT), main.o server_config.o httplib.o simple_git_lfs.o json11.o base64.o)
MAINT = $(addprefix $(OUT), lfs_server)

# We compile separately because it is faster since we avoid recompiling all files,
# especially the libraries that we are using and that take significant seconds.
#
# $^ -> prerequisites reference.
# $@ -> target reference.
#

$(MAINT) : $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

out/main.o : src/server_config.h
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o out/main.o

out/server_config.o : src/server_config.h
	$(CXX) $(CXXFLAGS) -c src/server_config.cpp -o out/server_config.o

out/simple_git_lfs.o : out/httplib.o out/json11.o out/base64.o
	$(CXX) $(CXXFLAGS) -c src/simple_git_lfs.cpp -o out/simple_git_lfs.o

out/httplib.o :
	$(CXX) $(CXXFLAGS) -c inc/httplib.cpp -o out/httplib.o

out/json11.o :
	$(CXX) $(CXXFLAGS) -c inc/json11.cpp -o out/json11.o

out/base64.o :
	$(CXX) $(CXXFLAGS) -c inc/base64.cpp -o out/base64.o

build :
	mkdir -p $(OUT)

run :
	./$(MAINT)

clean :
	rm -rf $(OUT)
	rm test/test_env

# If there is a target called clean/all/run then GNUmake could be confused, so we
# will avoid that, just in case.
.PHONY : all clean run
