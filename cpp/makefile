CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O3 -fopenmp

# Default to standard MCTS if not specified
MCTS_TYPE ?= standard

ifeq ($(MCTS_TYPE),random)
    MCTS_SRC = mcts_random_random.cpp
    CXXFLAGS += -DUSE_RANDOM_RANDOM
else
    MCTS_SRC = mcts_random.cpp
endif

TARGET = game2048
SRCS = main.cpp env2048.cpp $(MCTS_SRC)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean