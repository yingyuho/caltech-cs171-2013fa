PRG0=matrixdbg
PRG1=transform4x4
PRG2=draw2d
OBJECTS1=$(PRG1).tab.o $(PRG1).yy.o $(PRG1).o
OBJECTS2=$(PRG2).tab.o $(PRG2).yy.o $(PRG2).o
SOURCES=*.cpp *.h
CFLAGS=
LDFLAGS=

all: $(PRG0) $(PRG1) $(PRG2)

$(PRG0): $(PRG0).o
	g++ $(LDFLAGS) $(CFLAGS) -o $(PRG0) $^

$(PRG1): $(OBJECTS1)
	g++ $(LDFLAGS) $(CFLAGS) -o $(PRG1) $^

$(PRG2): $(OBJECTS2)
	g++ $(LDFLAGS) $(CFLAGS) -o $(PRG2) $^

clean:
	rm *.o *.yy.cpp *.tab.cpp *.tab.hpp

$(PRG1).tab.cpp $(PRG1).tab.hpp: $(PRG1).ypp
	bison -d $(PRG1).ypp

$(PRG1).yy.cpp: $(PRG1).lex
	flex -+ -o $(PRG1).yy.cpp $(PRG1).lex

$(PRG2).tab.cpp $(PRG2).tab.hpp: $(PRG2).ypp
	bison -d $(PRG2).ypp

$(PRG2).yy.cpp: $(PRG2).lex
	flex -+ -o $(PRG2).yy.cpp $(PRG2).lex

.cpp.o:
	g++ -c $(CFLAGS) -o $@ $<

.PHONY: all clean
