# APP NAME
TARGET=main 

# Compiler
CC=gcc
# Debug
DEBUG=-g
#optimsation
OPT=-O0
# warnings
WARN=-Wall

CCFLAGS=$(WARN) $(DEBUG) $(OPT) -pipe 

CSYNTH50LIB=`pkg-config --cflags --libs CSynth50`

#linker

LD=gcc
LDFLAGS=$(CSYNTH50LIB)

OBJS=	main.o

all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: main.c
	$(CC) -c $(CCFLAGS) main.c $(CSYNTH50LIB) -o main.o

clean:
	rm -f *.o $(TARGET)
