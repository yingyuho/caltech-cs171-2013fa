PRG = shaded
LEXER = inventor
PARSER = inventor

OBJECTS = \
obj/$(PARSER).tab.o 	obj/$(LEXER).yy.o \
obj/canvas.o 			obj/face.o 			obj/illuminator.o 		obj/inventor.o\
obj/shaded.o 			obj/material.o 		obj/mesh.o 				obj/mesh_utility.o \
obj/point_light.o 		obj/raster.o 		obj/shading_data.o \
obj/shading_complex.o 	obj/transform.o \

CC = g++
LD = g++
IN = include
PC = parser_code
CFLAGS = -O2 -Wno-tautological-compare -I./$(IN)
LDFLAGS = 

all: obj tmp $(PC) $(PRG)

$(PRG): $(OBJECTS)
	$(LD) $(LDFLAGS) $(CFLAGS) -o $(PRG) $^

clean:
	rm -f obj/*.o $(PC)/*.* tmp/*.*

obj:
	mkdir obj

tmp:
	mkdir tmp

$(PC):
	mkdir $(PC)

$(PC)/$(PARSER).tab.cpp $(PC)/$(PARSER).tab.hpp: src/$(PARSER).ypp
	bison -d src/$(PARSER).ypp -o $(PC)/$(PARSER).tab.cpp

$(PC)/$(LEXER).yy.cpp: src/$(LEXER).lex
	flex -+ -o $(PC)/$(LEXER).yy.cpp src/$(LEXER).lex

obj/$(PARSER).tab.o: $(PC)/$(PARSER).tab.cpp tmp/inventor.tag tmp/parser_param.tag
	$(CC) -c $(CFLAGS) -o $@ $<

obj/$(LEXER).yy.o: $(PC)/$(LEXER).yy.cpp $(PC)/$(PARSER).tab.hpp tmp/inventor.tag tmp/parser_param.tag
	$(CC) -c $(CFLAGS) -o $@ $<

tmp/color.tag: $(IN)/color.h $(IN)/matrix.h
	echo > tmp/color.tag

tmp/mesh_processor.tag: $(IN)/mesh_processor.h tmp/mesh.tag
	echo > tmp/mesh_processor.tag

tmp/mesh_utility.tag: $(IN)/mesh_utility.h tmp/mesh.tag
	echo > tmp/mesh_utility.tag

tmp/parser_param.tag: $(IN)/parser_param.h tmp/inventor.tag
	echo > tmp/parser_param.tag

tmp/canvas.tag: $(IN)/canvas.h tmp/pixel.tag $(IN)/matrix.h
	echo > tmp/canvas.tag

tmp/face.tag: $(IN)/face.h $(IN)/matrix.h
	echo > tmp/face.tag

tmp/illuminator.tag: $(IN)/illuminator.h $(IN)/point_light.h $(IN)/material.h $(IN)/ptr_container.h
	echo > tmp/illuminator.tag

tmp/material.tag: $(IN)/material.h tmp/color.tag
	echo > tmp/material.tag

tmp/mesh.tag: $(IN)/mesh.h tmp/face.tag tmp/transform.tag $(IN)/ptr_container.h
	echo > tmp/mesh.tag

tmp/pixel.tag: $(IN)/pixel.h tmp/color.tag
	echo > tmp/pixel.tag

tmp/point_light.tag: $(IN)/point_light.h tmp/color.tag
	echo > tmp/point_light.tag

tmp/raster.tag: $(IN)/raster.h $(IN)/face.h tmp/shading_data.tag
	echo > tmp/raster.tag

tmp/shading_data.tag: $(IN)/shading_data.h $(IN)/matrix.h
	echo > tmp/shading_data.tag

tmp/shading_complex.tag: $(IN)/shading_complex.h tmp/shading_data.tag tmp/inventor.tag tmp/mesh_utility.tag
	echo > tmp/shading_complex.tag

tmp/transform.tag: $(IN)/transform.h $(IN)/matrix.h $(IN)/ptr_container.h
	echo > tmp/transform.tag

tmp/inventor.tag: $(IN)/inventor.h tmp/transform.tag tmp/mesh_processor.tag tmp/point_light.tag $(IN)/ptr_container.h tmp/material.tag
	echo > tmp/inventor.tag

obj/canvas.o: src/canvas.cpp tmp/canvas.tag
	$(CC) -c $(CFLAGS) -o $@ $<

obj/face.o: src/face.cpp tmp/face.tag
	$(CC) -c $(CFLAGS) -o $@ $<

obj/illuminator.o: src/illuminator.cpp tmp/illuminator.tag
	$(CC) -c $(CFLAGS) -o $@ $<

obj/mesh.o: src/mesh.cpp tmp/mesh.tag
	$(CC) -c $(CFLAGS) -o $@ $<

obj/mesh_utility.o: src/mesh_utility.cpp tmp/mesh_utility.tag
	$(CC) -c $(CFLAGS) -o $@ $<

obj/shaded.o: src/shaded.cpp tmp/inventor.tag tmp/canvas.tag tmp/raster.tag tmp/shading_complex.tag
	$(CC) -c $(CFLAGS) -o $@ $<

obj/material.o: src/material.cpp tmp/material.tag
	$(CC) -c $(CFLAGS) -o $@ $<

obj/point_light.o: src/point_light.cpp tmp/point_light.tag
	$(CC) -c $(CFLAGS) -o $@ $<

obj/inventor.o: src/inventor.cpp tmp/inventor.tag
	$(CC) -c $(CFLAGS) -o $@ $<

obj/raster.o: src/raster.cpp tmp/raster.tag tmp/illuminator.tag tmp/face.tag tmp/pixel.tag
	$(CC) -c $(CFLAGS) -o $@ $<

obj/shading_data.o: src/shading_data.cpp tmp/shading_data.tag
	$(CC) -c $(CFLAGS) -o $@ $<

obj/shading_complex.o: src/shading_complex.cpp tmp/shading_complex.tag
	$(CC) -c $(CFLAGS) -o $@ $<

obj/transform.o: src/transform.cpp tmp/transform.tag
	$(CC) -c $(CFLAGS) -o $@ $<


.PHONY: all clean
