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

CSQUARELIB=`pkg-config --cflags --libs CSquare`

#linker

LD=gcc
LDFLAGS=$(CSQUARELIB) -export-dynamic

OBJS=	main.o

all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: main.c
	$(CC) -c $(CCFLAGS) main.c $(CSQUARELIB) -o main.o

clean:
	rm -f *.o $(TARGET)