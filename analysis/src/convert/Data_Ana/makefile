ROOTCFLAGS  = $(shell root-config --cflags)
ROOTLIBS    = $(shell root-config --glibs)

SRC = $(wildcard *.cpp)
INCH = $(wildcard *.h)
OBJ = $(patsubst %.cpp,%.o,$(filter %.cpp,$(SRC)))
DEP = $(patsubst %.cpp,%.d,$(filter %.cpp,$(SRC)))
EXE = DataAna
ROOT_DICT=classDataConvert

#CFLAGS = -Wall -O2 -I. -fpermissive -DMYTEST=1
CFLAGS = -Wall -g -I. -fpermissive -DMYTEST=1

LIBS= -lrt    

ROOTCINT = rootcint #-f
GXX = g++
GCC = gcc

all:$(EXE)

$(EXE): DIC $(OBJ) $(INCH)
	$(GXX) $(CFLAGS) $(ROOTCFLAGS) -o $@ $(OBJ) $(ROOTLIBS) $(LIBS)
%.o:%.cpp setup.h
	$(GXX) $(CFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) -o $@ -c $< 

DIC:
	$(ROOTCINT) $(ROOT_DICT).cpp  $(CFLAGS) -c XIA_Decode.h classLinkDef.h

clean:
	rm -f *~ .*.cpp.*  *.d *.o $(EXE)

install:
	cp $(EXE) $(ROOT_DICT)_rdict.pcm ../
