# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -w -Wall -Wextra -pthread -I./src -I./tests

# Directories
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

# Google Test
GTEST_DIR = /usr/local
GTEST_LIBS = -lgtest -lgtest_main -pthread

# Source files for main program
SRCS = $(SRC_DIR)/task.cpp
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Test files (Excludes task.cpp, which is compiled separately for tests)
TEST_SRCS = $(TEST_DIR)/test_contour.cpp
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(TEST_SRCS))

# Task object for tests (compiled separately with -DRUNNING_TESTS)
TEST_TASK_OBJ = $(BUILD_DIR)/task_test.o

# Executables
TARGET = my_program
TEST_TARGET = test_runner

.PHONY: all clean test force_rebuild

# ✅ Compile everything when running `make`
all: $(TARGET) $(TEST_TARGET)

# ✅ Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ✅ Compile the main program (without -DRUNNING_TESTS)
$(TARGET): $(BUILD_DIR)/task.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(BUILD_DIR)/task.o

# ✅ Compile task.o normally for `my_program`
$(BUILD_DIR)/task.o: $(SRC_DIR)/task.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ✅ Compile task_test.o separately with `-DRUNNING_TESTS` for test builds
$(TEST_TASK_OBJ): $(SRC_DIR)/task.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -DRUNNING_TESTS -c $< -o $@

# ✅ Compile test executable (Use task_test.o, not task.o)
$(TEST_TARGET): $(BUILD_DIR) $(TEST_OBJS) $(TEST_TASK_OBJ)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJS) $(TEST_TASK_OBJ) $(GTEST_LIBS)

# ✅ Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ✅ Compile test files into object files
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ✅ Run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# ✅ Force rebuild everything
force_rebuild:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)
	make -B

# ✅ Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)
