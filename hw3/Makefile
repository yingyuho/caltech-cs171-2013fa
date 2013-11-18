PRG = shaded
LEXER = inventor
PARSER = inventor
OBJECTS = $(PARSER).tab.o $(LEXER).yy.o main.o inventor.o transform.o face.o mesh.o shading_complex.o mesh_utility.o
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

$(PARSER).tab.o: $(PARSER).tab.cpp inventor.tag parser_param.tag
	$(CC) -c $(CFLAGS) -o $@ $<

$(LEXER).yy.o: $(LEXER).yy.cpp $(PARSER).tab.hpp inventor.tag parser_param.tag
	$(CC) -c $(CFLAGS) -o $@ $<

mesh_processor.tag: mesh_processor.h mesh.tag
	echo > mesh_processor.tag

mesh_utility.tag: mesh_utility.h mesh.tag
	echo > mesh_utility.tag

parser_param.tag: parser_param.h inventor.tag
	echo > parser_param.tag

canvas.tag: canvas.h canvas.cpp pixel.tag
	echo > canvas.tag

face.tag: face.h matrix.h
	echo > face.tag

mesh.tag: mesh.h face.tag transform.tag ptr_container.h
	echo > mesh.tag

pixel.tag: pixel.h matrix.h
	echo > pixel.tag

shading_complex.tag: shading_complex.h inventor.tag mesh_utility.tag
	echo > shading_complex.tag

transform.tag: transform.h matrix.h ptr_container.h
	echo > transform.tag

inventor.tag: inventor.h inventor.h transform.tag mesh_processor.tag ptr_container.h
	echo > inventor.tag

face.o: face.cpp face.tag
	$(CC) -c $(CFLAGS) -o $@ $<

mesh.o: mesh.cpp mesh.tag
	$(CC) -c $(CFLAGS) -o $@ $<

mesh_utility.o: mesh_utility.cpp mesh_utility.tag
	$(CC) -c $(CFLAGS) -o $@ $<

main.o: main.cpp inventor.tag canvas.tag mesh.tag mesh_utility.tag ptr_container.h
	$(CC) -c $(CFLAGS) -o $@ $<

inventor.o: inventor.cpp inventor.tag
	$(CC) -c $(CFLAGS) -o $@ $<

shading_complex.o: shading_complex.cpp shading_complex.tag
	$(CC) -c $(CFLAGS) -o $@ $<

transform.o: transform.cpp transform.tag
	$(CC) -c $(CFLAGS) -o $@ $<

#.cpp.o: $(HEADERS)
#	g++ -c $(CFLAGS) -o $@ $<

.PHONY: all clean
