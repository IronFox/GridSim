CPP = g++
FLAGS = -Ofast -march=native -flto -std=c++11 -Itbb/include
LIBS = -pthread libtbb.so

all: sim1 sim2 sim3

sim1:
	$(CPP) main.cpp -D DIMENSIONS=1 $(FLAGS) $(LIBS) -o simulate1

sim2:
	$(CPP) main.cpp -D DIMENSIONS=2 $(FLAGS) $(LIBS) -o simulate2

sim3:
	$(CPP) main.cpp -D DIMENSIONS=3 $(FLAGS) $(LIBS) -o simulate3


