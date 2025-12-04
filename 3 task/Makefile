CXX = g++
CXXFLAGS = -std=c++17 -O2 -pthread
TARGET = task3

$(TARGET): main.cpp philosophers.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp philosophers.cpp

clean:
	rm -f $(TARGET)

.PHONY: clean