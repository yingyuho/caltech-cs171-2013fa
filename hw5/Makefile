PRG = editSpline
LEXER = inventor
PARSER = inventor

OBJECTS = \
obj/main.o 	obj/Viewer.o
#obj/Complex.o 	obj/Quaternion.o 	obj/Vector.o

CC = g++
LD = g++
IN = include
PC = parser_code
CFLAGS = -O2 -Wall -g -std=c++11 -I./$(IN) -Wno-deprecated-declarations
#-Wno-tautological-compare
LDFLAGS = -std=c++11 -L/usr/X11R6/lib -lXi -lXmu

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
	$(CC) -c $(CFLAGS) -o $@ $<

obj/$(LEXER).yy.o: $(PC)/$(LEXER).yy.cpp $(PC)/$(PARSER).tab.hpp tmp/inventor.tag tmp/parser_param.tag
	$(CC) -c $(CFLAGS) -o $@ $<

tmp/color.tag: $(IN)/color.h $(IN)/matrix.h
	echo > tmp/color.tag

tmp/Viewer.tag: $(IN)/Viewer.h $(IN)/Spline.h
	echo > tmp/Viewer.tag

obj/main.o: src/main.cpp tmp/Viewer.tag
	$(CC) -c $(CFLAGS) -o $@ $<

obj/Viewer.o: src/Viewer.cpp tmp/Viewer.tag
	$(CC) -c $(CFLAGS) -o $@ $<

obj/Complex.o: src/Complex.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

obj/Quaternion.o: src/Quaternion.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

obj/Vector.o: src/Vector.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

.PHONY: all clean
