rootcint -f classDataConvert.cpp -c setup.h XIA_Decode.h classLinkDef.h
g++ -c main.cpp `root-config --cflags --glibs` 
g++ -c classDataConvert.cpp `root-config --cflags --glibs` 
g++ -c XIA_Decode.cpp `root-config --cflags --glibs` 
g++ -o DataAna XIA_Decode.o main.o classDataConvert.o `root-config --cflags --glibs`
