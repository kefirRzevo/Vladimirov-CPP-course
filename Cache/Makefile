CC  = gcc
CXX = g++ -std=c++17

CXXFLAGS = -I inc -Wall 

DBGFLAGS = -g -fsanitize=address -ggdb3

TARGET = cache
TEST_LFU_TARGET   = cache_testLFU
TEST_IDEAL_TARGET = cache_testIdeal

SRCDIR     = ./
INCDIR 	   = include/
OBJDIR 	   = build/
TEST_LFU_SRCDIR   = testLFU/
TEST_IDEAL_SRCDIR = testIdeal/

SOURCES  	= $(foreach SRC, $(SRCDIR), $(wildcard $(SRC)*.cpp))
OBJECTS     = $(foreach SRC, $(SOURCES), $(basename $(OBJDIR)$(SRC)).o)
DEPENDS     = $(patsubst %.o, %.d, $(OBJECTS))
TEST_LFU_SOURCES = $(foreach TEST_SRC,  $(TEST_LFU_SRCDIR),  $(wildcard $(TEST_SRC)*.cpp))
TEST_LFU_OBJECTS = $(foreach TEST_SRC,  $(TEST_LFU_SOURCES), $(basename $(OBJDIR)$(TEST_SRC)).o)
TEST_LFU_DEPENDS = $(patsubst %.o, %.d, $(TEST_LFU_OBJECTS))
TEST_IDEAL_SOURCES = $(foreach TEST_SRC,  $(TEST_IDEAL_SRCDIR),  $(wildcard $(TEST_SRC)*.cpp))
TEST_IDEAL_OBJECTS = $(foreach TEST_SRC,  $(TEST_IDEAL_SOURCES), $(basename $(OBJDIR)$(TEST_SRC)).o)
TEST_IDEAL_DEPENDS = $(patsubst %.o, %.d, $(TEST_IDEAL_OBJECTS))


all: build

.PHONY: clean dirs test_dirs build test_lfu test_ideal

clean:
	$(RM) $(OBJECTS) $(DEPENDS) $(TARGET) $(TEST_OBJECTS) $(TEST_DEPENDS) $(TEST_TARGET)

dirs:
	@ mkdir -p $(OBJDIR)
	@ mkdir -p $(addprefix $(OBJDIR), $(SRCDIR))
	@ mkdir -p $(addprefix $(OBJDIR), $(INCDIR))

test_dirs:
	@ mkdir -p $(OBJDIR)
	@ mkdir -p $(addprefix $(OBJDIR), $(TEST_LFU_SRCDIR))
	@ mkdir -p $(addprefix $(OBJDIR), $(TEST_IDEAL_SRCDIR))

build: dirs $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -g $(CXXFLAGS) $^ -o $@

test_lfu: test_dirs $(TEST_LFU_TARGET)

$(TEST_LFU_TARGET): $(TEST_LFU_OBJECTS)
	$(CXX) -g $(CXXFLAGS) -pthread $^ -o $@ -lgtest_main  -lgtest -lpthread

test_ideal: test_dirs $(TEST_IDEAL_TARGET)

$(TEST_IDEAL_TARGET): $(TEST_IDEAL_OBJECTS)
	$(CXX) -g $(CXXFLAGS) -pthread $^ -o $@ -lgtest_main  -lgtest -lpthread

-include $(DEPENDS)
-include $(TEST_LFU_DEPENDS)
-include $(TEST_IDEAL_DEPENDS)

$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@
