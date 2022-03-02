# in CMAKE Path do:
# make build
# make clean
# make execute


###############
## VARIABLES ##
###############
COMPILATION_MODE ?= Debug
ENABLE_WARNINGS ?= 0 #todo set default set to 1
WARNINGS_AS_ERRORS ?= 0
EXECUTABLE_PREFIX ?= HierarchicalStateMachine
CPP_COMPILER ?= g++ # g++, clang++
CPP_STANDARD ?= c++17 # c++11, c++14, c++17, c++20

ifeq ($(COMPILATION_MODE), Debug)
CPP_COMPILER_FLAGS = -g -O0 -std=$(CPP_STANDARD)
EXECUTABLE_NAME = $(EXECUTABLE_PREFIX)Debug
else
CPP_COMPILER_FLAGS = -O3 -Wall -std=$(CPP_STANDARD)
EXECUTABLE_NAME = $(EXECUTABLE_PREFIX)Release
endif

ifeq ($(ENABLE_WARNINGS), 1)
CPP_COMPILER_FLAGS += -Wall -Wextra -Wpedantic -Wconversion
endif

ifeq ($(WARNINGS_AS_ERRORS), 1)
CPP_COMPILER_FLAGS += -Werror
endif

CPP_COMPILER_CALL = $(CPP_COMPILER) $(CPP_COMPILER_FLAGS)

INCLUDE_DIR = src
SOURCE_DIR = src
BUILD_DIR = build

CPP_SRCS = $(wildcard $(SOURCE_DIR)/*.cpp)
CPP_OBJECTS = $(patsubst $(SOURCE_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_SRCS))

CC_SRCS = $(wildcard $(SOURCE_DIR)/*.cc)
CC_OBJECTS = $(patsubst $(SOURCE_DIR)/%.cc, $(BUILD_DIR)/%.o, $(CC_SRCS))

####################
## PESUDO TARGETS ##
####################
build: $(BUILD_DIR)/$(EXECUTABLE_NAME)

#############
## TARGETS ##
#############
$(BUILD_DIR)/$(EXECUTABLE_NAME): $(CPP_OBJECTS) $(CC_OBJECTS)
	$(CPP_COMPILER_CALL) $^ -o $@

execute:
	./$(BUILD_DIR)/$(EXECUTABLE_NAME)

clean:
	rm $(BUILD_DIR)/*.o

##############
## PATTERNS ##
##############
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(CPP_COMPILER_CALL) -I $(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cc
	$(CPP_COMPILER_CALL) -I $(INCLUDE_DIR) -c $< -o $@

###########
## PHONY ##
###########
.PHONY: clean build
