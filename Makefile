# Makefile for the pilFflonkProver project

# Directories
BUILD_DIR := build
SRC_DIRS  := src src/chelpers
DPNDS_DIR := depends/ffiasm/c
TEST_DIRS := src src/helpers test

# Libraries
LIBOMP := $(shell find /usr/lib/llvm-* -name "libomp.so" | sed 's/libomp.so//')
ifndef LIBOMP
$(error LIBOMP is not set, you need to install libomp-dev)
endif

# Compiler and flags
CXX := g++
AS := nasm

CXXFLAGS := -std=c++17 -g3 -Wall -pthread -flarge-source-files -Wno-unused-label -rdynamic -mavx2 -lgmp #-Wfatal-errors
LDFLAGS := -lsodium -lpthread -lgmp -lstdc++ -lomp -lgmpxx -L$(LIBOMP) -lgtest
CFLAGS := -fopenmp
ASFLAGS := -felf64

# Targets
TARGET_PFP := pfProver
TARGET_TEST := pfProverTest

INC_DIRS := depends/ffiasm/c depends/json/single_include/nlohmann depends/xkcp/Standalone/CompactFIPS202/C src/chelpers
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Files
SRCS_PFP := $(shell find $(SRC_DIRS) -name *.cpp)
SRCS_PFP += $(addprefix $(DPNDS_DIR)/, alt_bn128.cpp fr.cpp fq.cpp misc.cpp binfile_utils.cpp naf.cpp splitparstr.cpp)
SRCS_PFP += $(shell find $(DPNDS_DIR) -name *.asm)
OBJS_PFP := $(patsubst %,$(BUILD_DIR)/%.o,$(SRCS_PFP))
DEPS_PFP := $(OBJS_PFP:.o=.d)

# Test Files
SRCS_TEST := $(shell find $(TEST_DIRS) -name *.cpp)
SRCS_TEST += $(addprefix $(DPNDS_DIR)/, alt_bn128.cpp fr.cpp fq.cpp misc.cpp binfile_utils.cpp naf.cpp splitparstr.cpp)
SRCS_TEST += $(shell find $(DPNDS_DIR) -name *.asm)
OBJS_TEST := $(patsubst %,$(BUILD_DIR)/%.o,$(SRCS_TEST))
OBJS_TEST := $(filter-out $(BUILD_DIR)/src/main.cpp.o, $(OBJS_TEST)) # Exclude main.cpp from test build
DEPS_TEST := $(OBJS_TEST:.o=.d)

# Default target
all: $(BUILD_DIR)/$(TARGET_PFP)

test: $(BUILD_DIR)/$(TARGET_TEST)

# Build pilFflonkProver target
$(BUILD_DIR)/$(TARGET_PFP): $(OBJS_PFP)
	$(CXX) $(OBJS_PFP) $(CXXFLAGS) $(LDFLAGS) -o $@

# Object file rules
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(INC_FLAGS) $(CFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@ 

$(BUILD_DIR)/%.asm.o: %.asm
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

### TEST
# Build pilFflonkProver test target
$(BUILD_DIR)/$(TARGET_TEST): $(OBJS_TEST)
	$(CXX) $(OBJS_TEST) $(CXXFLAGS) $(LDFLAGS) -o $@

init:
	cd $(DPNDS_DIR) && node ../src/buildzqfield.js -q 21888242871839275222246405745257275088696311157297823662689037894645226208583 -n Fq
	cd $(DPNDS_DIR) && node ../src/buildzqfield.js -q 21888242871839275222246405745257275088548364400416034343698204186575808495617 -n Fr

# Clean target
clean:
	$(RM) -r $(BUILD_DIR)

.PHONY: all clean init

# Include dependencies
-include $(DEPS_PFP)

# Utility for creating directories
MKDIR_P ?= mkdir -p
