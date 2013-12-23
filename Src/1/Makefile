CC = g++
FLAGS = -Wall -std=c++11
LIBS = -lCGK -lSDL2 -lGL -lfreetype -lGLEW
TARGET = Ocean
OBJ = Ocean.o

$(TARGET): Main.cpp $(OBJ)
	$(CC) -o $(TARGET) Main.cpp $(OBJ) $(LIBS) $(FRAMEWORKS) $(FLAGS)

%.o : %.cpp
	$(CC) -c $(FLAGS) $<

.PHONY:
clean:
	rm *.o
