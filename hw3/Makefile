PRG = shaded
LEXER = inventor
PARSER = inventor
OBJECTS = $(PARSER).tab.o $(LEXER).yy.o main.o inventor.o transform.o
SOURCES = *.cpp *.h

CC = g++
LD = g++
CFLAGS = -O0
LDFLAGS = 

all: $(PRG)

$(PRG): $(OBJECTS)
	$(LD) $(LDFLAGS) $(CFLAGS) -o $(PRG) $^

clean:
	rm -f *.o *.yy.cpp *.tab.cpp *.tab.hpp *.tag

$(PARSER).tab.cpp $(PARSER).tab.hpp: $(PARSER).ypp
	bison -d $(PARSER).ypp

$(LEXER).yy.cpp: $(LEXER).lex
	flex -+ -o $(LEXER).yy.cpp $(LEXER).lex

$(PARSER).tab.o: $(PARSER).tab.cpp inventor.o parser_param.tag
	$(CC) -c $(CFLAGS) -o $@ $<

$(LEXER).yy.o: $(LEXER).yy.cpp $(PARSER).tab.hpp inventor.o parser_param.tag
	$(CC) -c $(CFLAGS) -o $@ $<

mesh.tag: mesh.h mesh.cpp face.h transform.o
	echo > mesh.tag

mesh_processor.tag: mesh_processor.h mesh.tag
	echo > mesh_processor.tag

mesh_utility.tag: mesh_utility.h mesh.tag
	echo > mesh_utility.tag

parser_param.tag: parser_param.h inventor.o
	echo > parser_param.tag

canvas.tag: canvas.h canvas.cpp
	echo > canvas.tag

main.o: main.cpp inventor.o canvas.tag mesh.tag mesh_utility.tag ptr_container.h
	$(CC) -c $(CFLAGS) -o $@ $<

inventor.o: inventor.cpp inventor.h transform.o mesh_processor.tag ptr_container.h
	$(CC) -c $(CFLAGS) -o $@ $<

#parser_param.o: parser_param.cpp parser_param.h inventor.o
#	$(CC) -c $(CFLAGS) -o $@ $<

transform.o: transform.cpp transform.h matrix.h ptr_container.h
	$(CC) -c $(CFLAGS) -o $@ $<

#.cpp.o: $(HEADERS)
#	g++ -c $(CFLAGS) -o $@ $<

.PHONY: all clean
