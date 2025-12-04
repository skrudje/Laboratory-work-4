CXX = g++
CXXFLAGS = -std=c++17 -O2 -pthread
SRCS = main.cpp primitives.cpp
TARGET = task1

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: clean 