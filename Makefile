# Makefile for the pilFflonkProver project

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -pthread -flarge-source-files -Wno-unused-label -rdynamic -mavx2 #-Wfatal-errors
CFLAGS := -fopenmp
AS := nasm
ASFLAGS := -felf64

# Directories
BUILD_DIR := ./build
SRC_DIRS := ./src

# Libraries
LIBOMP := $(shell find /usr/lib/llvm-* -name "libomp.so" | sed 's/libomp.so//')
ifndef LIBOMP
$(error LIBOMP is not set, you need to install libomp-dev)
endif

# Targets
TARGET_PFP := pilFflonkProver
TARGET_TEST := pilFflonkProverTest

# Files
SRCS_PFP := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.asm -or -name *.cc)
OBJS_PFP := $(SRCS_PFP:%=$(BUILD_DIR)/%.o)
DEPS_PFP := $(OBJS_PFP:.o=.d)

# Default target
all: $(BUILD_DIR)/$(TARGET_PFP)

# Build pilFflonkProver target
$(BUILD_DIR)/$(TARGET_PFP): $(OBJS_PFP)
	$(CXX) $(OBJS_PFP) $(CXXFLAGS) -o $@ $(LDFLAGS)

# Object file rules
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cc.o: %.cc
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.asm.o: %.asm
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# Clean target
clean:
	$(RM) -r $(BUILD_DIR)

.PHONY: all clean

# Include dependencies
-include $(DEPS_PFP)

# Utility for creating directories
MKDIR_P ?= mkdir -p
