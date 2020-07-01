CFLAGS = -Wall -g -O2

all: freesub.dll

SDL_bdf.o: SDL_bdf.c SDL_bdf.h
	gcc -c SDL_bdf.c $(CFLAGS) -o SDL_bdf.o

libavisynth.a: avisynth.def
	dlltool -d avisynth.def -A -k -l libavisynth.a

freesub.o: freesub.c SDL_bdf.h
	gcc -c freesub.c $(CFLAGS) -o freesub.o

freesub.dll: freesub.o SDL_bdf.o libavisynth.a
	gcc freesub.o SDL_bdf.o libavisynth.a $(CFLAGS) -o freesub.dll -shared




