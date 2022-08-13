CXX = g++
CXXFLAGS = -std=c++14 -g3 -Wall -Wpedantic -Wextra -fsanitize=address -Iinc
OUT = out/
SRCS = $(addprefix src/, main.cpp server_config.cpp simple_git_lfs.cpp logger.cpp)
INCS = $(addprefix inc/, httplib.cpp json11.cpp base64.cpp)
MAINT = lfs_server

# @TODO(lev): figure out how to use makefiles effectively.
#
# $^ -> prerequisites reference.
# $@ -> target reference.
#

$(MAINT) : build
	$(CXX) $(CXXFLAGS) $(SRCS) $(INCS) -o $(OUT)$(MAINT)

build :
	mkdir -p $(OUT)

run :
	./$(OUT)$(MAINT)

clean :
	rm -rf $(OUT)
	rm test/test_env

# If there is a target called clean/all/run then GNUmake could be confused, so we
# will avoid that, just in case.
.PHONY : all clean run
