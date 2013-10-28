OBJECTS=parser.tab.o lexer.yy.o main.o
SOURCES=*.cpp *.h
CFLAGS=-g
LDFLAGS=

all: test

test: $(OBJECTS)
	g++ $(LDFLAGS) $(CFLAGS) -o test $^

clean:
	rm *.o lexer.yy.cpp parser.tab.cpp parser.tab.hpp

parser.tab.cpp parser.tab.hpp: parser.ypp
	bison -d parser.ypp

lexer.yy.cpp: lexer.lex
	flex -+ -olexer.yy.cpp lexer.lex

.cpp.o:
	g++ -c $(CFLAGS) -o $@ $<

.PHONY: all clean
