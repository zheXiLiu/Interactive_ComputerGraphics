#
# CS418 MP3
#

CC = g++
INC = -I.
FLAGS = -g -W -Wall
LIBS = -lglut -lGLU

SOURCES = liu275_mp4.c readppm.c 
OBJECTS = liu275_mp4.o readppm.o 


all: mp4

mp4: $(OBJECTS)
	$(CC) $(LIBS) $(OBJECTS) -o $@


clean:
	$(RM) *.o mp4 *.d *~
