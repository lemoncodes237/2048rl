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
ifeq ($(MCTS_TYPE), puct)
	MCTS_SRC = pUCT/mcts_pUCT.cpp
	CXXFLAGS += -DUSE_PUCT_SINGLE
endif
ifeq ($(MCTS_TYPE), puctmult)
	MCTS_SRC = pUCT_multiple/mcts_pUCT.cpp
	CXXFLAGS += -DUSE_PUCT_MULTIPLE
endif
ifeq ($(MCTS_TYPE), puctminmult)
	MCTS_SRC = pUCT_min_multiple/mcts_pUCT.cpp
	CXXFLAGS += -DUSE_PUCT_MIN_MULTIPLE
endif
ifeq ($(MCTS_TYPE), puctcombmult)
	MCTS_SRC = pUCT_comb_multiple/mcts_pUCT.cpp
	CXXFLAGS += -DUSE_PUCT_COMB_MULTIPLE
endif

TARGET = game2048
SRCS = main.cpp env2048.cpp $(MCTS_SRC)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean