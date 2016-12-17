CC = gcc
TARGET = fsmb
OBJECTS = fsmb_main.o fsmb_largefile.o fsmb_smallfile.o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

clean :
	rm *.o fsmb
