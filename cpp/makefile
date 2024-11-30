CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O3 -fopenmp

TARGET = game2048
SRCS = main.cpp env2048.cpp mcts_random.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean