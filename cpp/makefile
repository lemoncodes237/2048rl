CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O3 -fopenmp
CXXFLAGS += -I.
# Default to standard MCTS if not specified
MCTS_TYPE ?= standard

ifeq ($(MCTS_TYPE),randomrandom)
    MCTS_SRC = random_random/mcts_random_random.cpp
    CXXFLAGS += -DUSE_RANDOM_RANDOM
endif
ifeq ($(MCTS_TYPE),merge)
    MCTS_SRC = merge/mcts_merge.cpp
    CXXFLAGS += -DUSE_MERGE
endif
ifeq ($(MCTS_TYPE),score)
    MCTS_SRC = score/mcts_score.cpp
    CXXFLAGS += -DUSE_SCORE
endif
ifeq ($(MCTS_TYPE), random)
	MCTS_SRC = random/mcts_random.cpp
	CXXFLAGS += -DUSE_RANDOM
endif
ifeq ($(MCTS_TYPE), explorescore)
	MCTS_SRC = pUCT/mcts_pUCT.cpp
	CXXFLAGS += -DUSE_EXPLORE_SCORE
endif

TARGET = game2048
SRCS = main.cpp env2048.cpp $(MCTS_SRC)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean