CC  = gcc
CXX = g++ -std=c++17

CXXFLAGS = -I inc -Wall -Wextra -g #-Wall -Wextra \
-Weffc++ -Wc++0x-compat \
-Wc++11-compat -Wc++14-compat -Wcast-qual \
-Wchar-subscripts   \
-Wctor-dtor-privacy -Wempty-body -Wfloat-equal \
-Wformat-nonliteral -Wformat-security  \
-Wformat=2 -Winline \
-Wmissing-declarations -Wnon-virtual-dtor \
-Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls \
-Wshadow -Wsign-conversion -Wsign-promo \
-Wstrict-overflow=2 \
-Wsuggest-override \
-Wundef -Wunreachable-code -Wunused \
-Wvariadic-macros \
-Wno-missing-field-initializers -Wno-narrowing \
-Wno-old-style-cast -Wno-varargs -fcheck-new \
-fsized-deallocation -fstack-check -fstack-protector \
-fstrict-overflow \
-fno-omit-frame-pointer \
-fsanitize=address \
-fsanitize=bool \
-fsanitize=bounds \
-fsanitize=enum \
-fsanitize=float-cast-overflow \
-fsanitize=float-divide-by-zero \
-fsanitize=integer-divide-by-zero \
-fsanitize=nonnull-attribute \
-fsanitize=null \
-fsanitize=return \
-fsanitize=returns-nonnull-attribute \
-fsanitize=shift \
-fsanitize=signed-integer-overflow \
-fsanitize=undefined \
-fsanitize=unreachable \
-fsanitize=vla-bound \
-fsanitize=vptr

DBGFLAGS = -fsanitize=address -ggdb3

TARGET = cache
TEST_TARGET = cache_test

SRCDIR     = ./
INCDIR 	   = include/
TEST_SRCDIR = test/
TEST_INCDIR = test/
OBJDIR 	    = build/

SOURCES  	= $(foreach SRC, $(SRCDIR), $(wildcard $(SRC)*.cpp))
OBJECTS     = $(foreach SRC, $(SOURCES), $(basename $(OBJDIR)$(SRC)).o)
DEPENDS     = $(patsubst %.o, %.d, $(OBJECTS))
TEST_SOURCES = $(foreach TEST_SRC, $(TEST_SRCDIR), $(wildcard $(TEST_SRC)*.cpp))
TEST_OBJECTS = $(foreach TEST_SRC, $(TEST_SOURCES), $(basename $(OBJDIR)$(TEST_SRC)).o)
TEST_DEPENDS = $(patsubst %.o, %.d, $(TEST_OBJECTS))


all: build

.PHONY: clean dirs test_dirs build test

clean:
	$(RM) $(OBJECTS) $(DEPENDS) $(TARGET) $(TEST_OBJECTS) $(TEST_DEPENDS) $(TEST_TARGET)

dirs:
	@ mkdir -p $(OBJDIR)
	@ mkdir -p $(addprefix $(OBJDIR), $(SRCDIR))
	@ mkdir -p $(addprefix $(OBJDIR), $(INCDIR))

test_dirs:
	@ mkdir -p $(OBJDIR)
	@ mkdir -p $(addprefix $(OBJDIR), $(TEST_SRCDIR))
	@ mkdir -p $(addprefix $(OBJDIR), $(TEST_INCDIR))

build: dirs $(TARGET)

test: test_dirs $(TEST_TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -g $(CXXFLAGS) $^ -o $@

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CXX) -g $(CXXFLAGS) -mmacosx-version-min=12.6 -pthread $^ -o $@ -lgtest_main  -lgtest -lpthread

-include $(DEPENDS)
-include $(TEST_DEPENDS)

$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@
