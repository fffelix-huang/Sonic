###
### Files
###

EXE = sonic

OBJS = main.o chess/attacks.o chess/movegen.o chess/position.o utils/strings.o

###
### Rules
###

all: $(EXE)

clean: 
	$(RM) $(EXE) *.o chess/*.o utils/*.o

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

CXXFLAGS += -DNDEBUG

# Compile with full warnings, and symbol names

CXXFLAGS += -Wall -g -std=c++20

# General optimization flags.  Note that -O2 might be faster than -O3 on some
# systems; this requires testing.

CXXFLAGS += -O3 -fno-exceptions -fomit-frame-pointer -fno-rtti -fstrict-aliasing
