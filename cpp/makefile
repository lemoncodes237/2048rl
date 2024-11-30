# Compiler settings
CXX = g++
CXXFLAGS_COMMON = -Wall -Wextra -std=c++17 -fopenmp
CXXFLAGS_DEBUG = $(CXXFLAGS_COMMON) -g -O0 -DDEBUG
CXXFLAGS_RELEASE = $(CXXFLAGS_COMMON) -O3 -DNDEBUG

# Directories
SRC_DIR = .
BUILD_DIR = build
BUILD_DIR_DEBUG = $(BUILD_DIR)/debug
BUILD_DIR_RELEASE = $(BUILD_DIR)/release

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS_DEBUG = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR_DEBUG)/%.o)
OBJS_RELEASE = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR_RELEASE)/%.o)

# Targets
TARGET_DEBUG = $(BUILD_DIR_DEBUG)/game2048
TARGET_RELEASE = $(BUILD_DIR_RELEASE)/game2048

# Default target
all: debug release

# Debug build
debug: $(TARGET_DEBUG)

$(TARGET_DEBUG): $(OBJS_DEBUG)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_DEBUG) $^ -o $@

$(BUILD_DIR_DEBUG)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_DEBUG) -c $< -o $@

# Release build
release: $(TARGET_RELEASE)

$(TARGET_RELEASE): $(OBJS_RELEASE)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_RELEASE) $^ -o $@

$(BUILD_DIR_RELEASE)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_RELEASE) -c $< -o $@

# Clean targets
clean:
	rm -rf $(BUILD_DIR)

clean_debug:
	rm -rf $(BUILD_DIR_DEBUG)

clean_release:
	rm -rf $(BUILD_DIR_RELEASE)

# Phony targets
.PHONY: all debug release clean clean_debug clean_release

# Dependencies
-include $(OBJS_DEBUG:.o=.d)
-include $(OBJS_RELEASE:.o=.d)