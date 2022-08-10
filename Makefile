CXX = g++
CXXFLAGS = -O2 -Wall -Wpedantic -Wextra -fsanitize=address
CXXFLAGS_D = -g3 -Wall -Wpedantic -Wextra -fsanitize=address -DNDEBUG
LDFLAGS =
SRC = src/
OUT = out/

all: outdir files
	$(CXX) $(CXXFLAGS_D) $(SRC)main.cpp $(SRC)simple_git_lfs.cpp $(SRC)json11.cpp $(SRC)base64.cpp -o $(OUT)main $(LDFLAGS)

outdir:
	mkdir -p out

files:
	mkdir -p files

run:
	./out/main

clean:
	rm -rf out
