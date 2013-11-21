PRG = shaded
LEXER = inventor
PARSER = inventor

OBJECTS = \
$(PARSER).tab.o $(LEXER).yy.o main.o \
canvas.o inventor.o transform.o face.o mesh.o raster.o \
shading_data.o shading_complex.o mesh_utility.o point_light.o material.o illuminator.o

SOURCES = *.cpp *.h

CC = g++
LD = g++
CFLAGS = -O0 -Wno-tautological-compare
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

color.tag: color.h matrix.h
	echo > color.tag

mesh_processor.tag: mesh_processor.h mesh.tag
	echo > mesh_processor.tag

mesh_utility.tag: mesh_utility.h mesh.tag
	echo > mesh_utility.tag

parser_param.tag: parser_param.h inventor.tag
	echo > parser_param.tag

canvas.tag: canvas.h pixel.tag
	echo > canvas.tag

face.tag: face.h matrix.h
	echo > face.tag

illuminator.tag: illuminator.h point_light.h material.h ptr_container.h
	echo > illuminator.tag

material.tag: material.h color.tag
	echo > material.tag

mesh.tag: mesh.h face.tag transform.tag ptr_container.h
	echo > mesh.tag

pixel.tag: pixel.h matrix.h
	echo > pixel.tag

point_light.tag: point_light.h color.tag
	echo > point_light.tag

raster.tag: raster.h mesh.h
	echo > raster.tag

shading_data.tag: shading_data.h matrix.h
	echo > shading_data.tag

shading_complex.tag: shading_complex.h shading_data.tag inventor.tag mesh_utility.tag
	echo > shading_complex.tag

transform.tag: transform.h matrix.h ptr_container.h
	echo > transform.tag

inventor.tag: inventor.h transform.tag mesh_processor.tag point_light.tag ptr_container.h material.tag
	echo > inventor.tag

canvas.o: canvas.cpp canvas.tag
	$(CC) -c $(CFLAGS) -o $@ $<

face.o: face.cpp face.tag
	$(CC) -c $(CFLAGS) -o $@ $<

illuminator.o: illuminator.cpp illuminator.tag
	$(CC) -c $(CFLAGS) -o $@ $<

mesh.o: mesh.cpp mesh.tag
	$(CC) -c $(CFLAGS) -o $@ $<

mesh_utility.o: mesh_utility.cpp mesh_utility.tag
	$(CC) -c $(CFLAGS) -o $@ $<

main.o: main.cpp inventor.tag canvas.tag raster.tag shading_complex.tag
	$(CC) -c $(CFLAGS) -o $@ $<

material.o: material.cpp material.tag
	$(CC) -c $(CFLAGS) -o $@ $<

point_light.o: point_light.cpp point_light.tag
	$(CC) -c $(CFLAGS) -o $@ $<

inventor.o: inventor.cpp inventor.tag
	$(CC) -c $(CFLAGS) -o $@ $<

raster.o: raster.cpp raster.tag illuminator.tag
	$(CC) -c $(CFLAGS) -o $@ $<

shading_data.o: shading_data.cpp shading_data.tag
	$(CC) -c $(CFLAGS) -o $@ $<

shading_complex.o: shading_complex.cpp shading_complex.tag
	$(CC) -c $(CFLAGS) -o $@ $<

transform.o: transform.cpp transform.tag
	$(CC) -c $(CFLAGS) -o $@ $<

#.cpp.o: $(HEADERS)
#   g++ -c $(CFLAGS) -o $@ $<

.PHONY: all clean
