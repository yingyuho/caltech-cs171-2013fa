PRG = wireframe
LEXER = inventor
PARSER = inventor
OBJECTS = $(PARSER).tab.o $(LEXER).yy.o main.o inventor.o
SOURCES = *.cpp *.h

CC = g++
LD = g++
CFLAGS = -O0
LDFLAGS = 

all: $(PRG)

$(PRG): $(OBJECTS)
	$(LD) $(LDFLAGS) $(CFLAGS) -o $(PRG) $^

clean:
	rm -f *.o *.yy.cpp *.tab.cpp *.tab.hpp

$(PARSER).tab.cpp $(PARSER).tab.hpp: $(PARSER).ypp
	bison -d $(PARSER).ypp

$(LEXER).yy.cpp: $(LEXER).lex
	flex -+ -o $(LEXER).yy.cpp $(LEXER).lex

$(PARSER).tab.o: $(PARSER).tab.cpp inventor.o transform.h parser_param.h matrix.h
	$(CC) -c $(CFLAGS) -o $@ $<

$(LEXER).yy.o: $(LEXER).yy.cpp $(PARSER).tab.hpp inventor.h parser_param.h matrix.h
	$(CC) -c $(CFLAGS) -o $@ $<

main.o: main.cpp inventor.o canvas.h canvas.cpp matrix.h
	$(CC) -c $(CFLAGS) -o $@ $<

inventor.o: inventor.cpp inventor.h transform.h matrix.h
	$(CC) -c $(CFLAGS) -o $@ $<

#.cpp.o: $(HEADERS)
#	g++ -c $(CFLAGS) -o $@ $<

.PHONY: all clean
