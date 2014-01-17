OBJECTS=main.o readpng.o
CFLAGS=-g
LDFLAGS= -L/usr/lib/nvidia -L/usr/X11R6/lib -lpng -lXi -lXmu

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

glslRenderer: $(OBJECTS)
	g++ $(LDFLAGS) -o glslRenderer $^

.cpp.o:
	g++ $(CFLAGS) -c $<

clean:
	-rm $(OBJECTS) glslRenderer
