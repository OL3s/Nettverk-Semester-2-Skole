CXX := g++
CXXFLAGS := -std=c++20 -O2 -g -Wall -Wextra -Wpedantic

TARGET := primenum
SRC := primenum.cpp

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
