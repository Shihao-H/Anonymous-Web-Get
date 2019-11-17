CC = g++
FLAGS = -std=c++11 -Wall -pthread
EXECS = awget ss

all: $(EXECS)

awget: awget.o
	$(CC) $(FLAGS) awget.o -o awget
	
	
awget.o: awget.h awget.cpp
	$(CC) $(FLAGS) -c awget.cpp

ss: ss.o
	$(CC) $(FLAGS) ss.o -o ss
	
ss.o: awget.h ss.cpp
	$(CC) $(FLAGS) -c ss.cpp	
	
clean:
	rm -f *.o $(EXEC)
	rm -f awget
	rm -f ss

