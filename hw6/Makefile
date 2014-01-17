PRG = oglRenderer
LEXER = inventor
PARSER = inventor

OBJECTS = \
obj/$(PARSER).tab.o 	obj/$(LEXER).yy.o \
obj/face.o 		obj/inventor.o 		obj/main.o 			obj/material.o \
obj/mesh.o 		obj/point_light.o 	obj/transform.o 	obj/readpng.o \
obj/Viewer.o 	obj/Complex.o 	obj/Quaternion.o 	obj/Vector.o

CC = gcc
CPPC = g++
LD = g++
IN = include
PC = parser_code
CFLAGS = -O3 -Wall -std=c++11 -I./$(IN) -Wno-deprecated-declarations
#-Wno-tautological-compare
LDFLAGS = -O3 -std=c++11 -L/usr/X11R6/lib -lXi -lXmu -lpng

ifeq ($(OS),Windows_NT)
    CFLAGS += -D WIN32
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        CFLAGS += -D AMD64
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
        CFLAGS += -D IA32
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CFLAGS += -D LINUX
        LDFLAGS += -D LINUX
        LDFLAGS += -lglut -lGL -lGLU 
    endif
    ifeq ($(UNAME_S),Darwin)
        CFLAGS += -D MACOSX
        LDFLAGS += -DMACOSX -framework GLUT -framework OpenGL -framework Cocoa 
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CFLAGS += -D ARM
    endif
endif

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
	$(CPPC) -c $(CFLAGS) -o $@ $<

obj/$(LEXER).yy.o: $(PC)/$(LEXER).yy.cpp $(PC)/$(PARSER).tab.hpp tmp/inventor.tag tmp/parser_param.tag
	$(CPPC) -c $(CFLAGS) -o $@ $<

tmp/color.tag: $(IN)/color.h $(IN)/matrix.h
	echo > tmp/color.tag

tmp/mesh_processor.tag: $(IN)/mesh_processor.h tmp/mesh.tag
	echo > tmp/mesh_processor.tag

tmp/parser_param.tag: $(IN)/parser_param.h tmp/inventor.tag
	echo > tmp/parser_param.tag

tmp/face.tag: $(IN)/face.h $(IN)/matrix.h
	echo > tmp/face.tag

tmp/material.tag: $(IN)/material.h tmp/color.tag
	echo > tmp/material.tag

tmp/mesh.tag: $(IN)/mesh.h tmp/face.tag tmp/transform.tag $(IN)/ptr_container.h
	echo > tmp/mesh.tag

tmp/point_light.tag: $(IN)/point_light.h tmp/color.tag
	echo > tmp/point_light.tag

tmp/transform.tag: $(IN)/transform.h $(IN)/matrix.h $(IN)/ptr_container.h
	echo > tmp/transform.tag

tmp/Viewer.tag: $(IN)/Viewer.h tmp/inventor.tag
	echo > tmp/Viewer.tag

tmp/inventor.tag: $(IN)/inventor.h tmp/transform.tag tmp/mesh_processor.tag tmp/point_light.tag $(IN)/ptr_container.h tmp/material.tag
	echo > tmp/inventor.tag

obj/face.o: src/face.cpp tmp/face.tag
	$(CPPC) -c $(CFLAGS) -o $@ $<

obj/mesh.o: src/mesh.cpp tmp/mesh.tag
	$(CPPC) -c $(CFLAGS) -o $@ $<

obj/main.o: src/main.cpp tmp/Viewer.tag
	$(CPPC) -c $(CFLAGS) -o $@ $<

obj/material.o: src/material.cpp tmp/material.tag
	$(CPPC) -c $(CFLAGS) -o $@ $<

obj/point_light.o: src/point_light.cpp tmp/point_light.tag
	$(CPPC) -c $(CFLAGS) -o $@ $<

obj/inventor.o: src/inventor.cpp tmp/inventor.tag
	$(CPPC) -c $(CFLAGS) -o $@ $<

obj/transform.o: src/transform.cpp tmp/transform.tag
	$(CPPC) -c $(CFLAGS) -o $@ $<

obj/readpng.o: src/readpng.c
	$(CPPC) -c $(CFLAGS) -o $@ $<

obj/Viewer.o: src/Viewer.cpp tmp/Viewer.tag
	$(CPPC) -c $(CFLAGS) -o $@ $<

obj/Complex.o: src/Complex.cpp
	$(CPPC) -c $(CFLAGS) -o $@ $<

obj/Quaternion.o: src/Quaternion.cpp
	$(CPPC) -c $(CFLAGS) -o $@ $<

obj/Vector.o: src/Vector.cpp
	$(CPPC) -c $(CFLAGS) -o $@ $<

.PHONY: all clean
