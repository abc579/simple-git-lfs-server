CXX = g++
CXXFLAGS = -std=c++14 -g3 -Wall -Wpedantic -Wextra -fsanitize=address -Iinc -DCPPHTTPLIB_OPENSSL_SUPPORT=1
OUT = out/
SRCS = $(addprefix src/, main.cpp server_config.cpp simple_git_lfs.cpp logger.cpp util.cpp)
INCS = $(addprefix inc/, httplib.cpp json11.cpp base64.cpp)
MAINT = lfs_server
LDFLAGS = -lssl -lcrypto

# @TODO(lev): figure out how to use makefiles effectively.
#
# $^ -> prerequisites reference.
# $@ -> target reference.
#

$(MAINT) : build
	$(CXX) $(CXXFLAGS) $(INCS) $(SRCS) -o $(OUT)$(MAINT) $(LDFLAGS)

build :
	mkdir -p $(OUT)

run :
	./$(OUT)$(MAINT)

clean :
	rm -rf $(OUT)

# If there is a target called clean/all/run then GNUmake could be confused, so we
# will avoid that, just in case.
.PHONY : all clean run
