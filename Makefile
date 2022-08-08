CXX = g++
CXXFLAGS = -g3 -Wall -Wpedantic -Wextra -fsanitize=address
LDFLAGS =

all: outdir files
	$(CXX) $(CXXFLAGS) src/main.cpp src/simple_git_lfs.cpp src/json11.cpp -o out/main $(LDFLAGS)

outdir:
	mkdir -p out

files:
	mkdir -p files

run:
	./out/main

clean:
	rm -rf out
