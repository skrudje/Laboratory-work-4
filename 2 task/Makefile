CXX = g++
CXXFLAGS = -std=c++17 -O2 -pthread
TARGET = task2

$(TARGET): main.cpp students.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp students.cpp

clean:
	rm -f $(TARGET)

.PHONY: clean