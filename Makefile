CC=gcc
CFLAGS=-lwiringPi -DRaspberryPi -c -pedantic -Wall
LDFLAGS=-lwiringPi -DRaspberryPi -pedantic -Wall
SOURCES=main.cpp lib.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ShutdownButton

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm  *.o

cleanall:
	rm  *.o $(EXECUTABLE)

cstart: all start

start:
	./$(EXECUTABLE)

rebuild: cleanall all