###########################################################################################
##
## target platform: mac
##
###########################################################################################
## C++
CPP_DEFS = -std=c++17
CPP = gcc

###########################################################################################
## NOTE: uncomment block for ONE of debug, profile or optimised

## debug
#CPP_FLAGS =  $(CPP_DEFS) $(CPP_INCLUDES) -m64 -Og -Wall -Wextra

## profile
#CPP_FLAGS =  $(CPP_DEFS) $(CPP_INCLUDES) -m64 -O3 -pg 

## optimised
CPP_FLAGS =  $(CPP_DEFS) $(CPP_INCLUDES) -m64 -O3 -march=native -mtune=native -Wall -Wpedantic -g

###########################################################################################
## program

## NOTE: uncomment lines for ONE platform
#CPP_INCLUDES = -I/usr/include  # mahuika 
CPP_INCLUDES = -I/usr/local/include/eigen3 # iMac

#CPP_LIBS = -lsundials_cvodes
CPP_LIBS = -lstdc++

###########################################################################################

EXE = ./msh2ply
OBJS = main.o cCellMesh.o cDuctTree.o utils.o
SRC = ./

all: $(EXE)

main.o: $(SRC)/main.cpp
	$(CPP) $(CPP_FLAGS) $(CPP_INCLUDES) -o $@ -c $<

cCellMesh.o: $(SRC)/cCellMesh.cpp $(SRC)/cCellMesh.hpp $(SRC)/utils.hpp $(SRC)/global_defs.hpp
	$(CPP) $(CPP_FLAGS) $(CPP_INCLUDES) -o $@ -c $<

cDuctTree.o: $(SRC)/cDuctTree.cpp $(SRC)/cDuctTree.hpp $(SRC)/utils.hpp $(SRC)/global_defs.hpp
	$(CPP) $(CPP_FLAGS) $(CPP_INCLUDES) -o $@ -c $<

utils.o: $(SRC)/utils.cpp $(SRC)/utils.hpp $(SRC)/global_defs.hpp
	$(CPP) $(CPP_FLAGS) $(CPP_INCLUDES) -o $@ -c $<

$(EXE): $(OBJS)
	$(CPP) $(CPP_FLAGS) $(CPP_LIBS) -o $@ $+

clean:
	rm -f *.o
