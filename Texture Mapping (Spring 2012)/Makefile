#
# CS418 MP3
# SPRING 2012
# liu275
#

CC = g++
INC = -I.
FLAGS = -g -W -Wall
LIBS = -lglut -lGL -lGLU -lGLEW

SOURCES = liu275_mp3.c readppm.c Object.c
OBJECTS = liu275_mp3.o readppm.o Object.o


all: mp3


mp3: $(OBJECTS)
	$(CC) $(LIBS) $(OBJECTS) -o $@

clean:
	$(RM) *.o mp3 *.d *~
