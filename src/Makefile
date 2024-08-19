###
### Files
###

EXE = sonic

OBJS = main.o bench/benchmark.o bench/perft.o chess/attacks.o chess/movegen.o chess/position.o utils/strings.o utils/misc.o \
       uci.o search.o evaluate.o movesort.o book.o tt.o

###
### Rules
###

all: $(EXE)

clean: 
	$(RM) $(EXE) *.o bench/*.o chess/*.o utils/*.o

###
### Compiler:
###

###
### Dependencies
###

$(EXE): $(OBJS)
	+$(CXX) -o $@ $(OBJS) $(LDFLAGS)

###
### Compiler and linker switches
###

# Enable/disable debugging:

# CXXFLAGS += -DNDEBUG

# Compile with full warnings, and symbol names

CXXFLAGS += -Wall -g -std=c++17

# General optimization flags.  Note that -O2 might be faster than -O3 on some
# systems; this requires testing.

CXXFLAGS += -O3 -fno-exceptions -fomit-frame-pointer -fno-rtti -fstrict-aliasing
