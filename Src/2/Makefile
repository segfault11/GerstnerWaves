FLAGS= -Wall -std=c++11
LIBS= -lCGK -lSDL2 -lGL -lfreetype -lGLEW
TARGET= Wave4
OBJ= Wave.o

$(TARGET): Main.cpp $(OBJ)
	g++ -o $(TARGET) Main.cpp $(OBJ) $(FLAGS) $(LIBS)

%.o: %.cpp
	g++ -c $<
