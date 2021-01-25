CXX := g++
CXXFLAGS := -O2 -Wall -Wextra -Werror

BIN=bin/apex
all: $(BIN)

run: all
	$(BIN)

$(BIN): apex.cpp
	$(CXX) $(CXXFLAGS) -lrt $< -o $@

clean:
	rm -f $(BIN)
