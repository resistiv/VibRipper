CC = g++
CFLAGS = -Wall -std=c++20
TARGET = VibRipper
RM = rm

$(TARGET): VibRipper.o Repacker.o Unpacker.o
	$(CC) $(CFLAGS) -o $(TARGET) VibRipper.o Repacker.o Unpacker.o

VibRipper.o: VibRipper.cpp Repacker.h Unpacker.h VibRipper.h
	$(CC) $(CFLAGS) -c VibRipper.cpp

Repacker.o: Repacker.cpp Repacker.h VibRipper.h
	$(CC) $(CFLAGS) -c Repacker.cpp

Unpacker.o: Unpacker.cpp Unpacker.h VibRipper.h
	$(CC) $(CFLAGS) -c Unpacker.cpp

clean: 
	-$(RM) *.o *.exe *.out