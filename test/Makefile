
FLAGS = -O3 -g3 -fPIC
CFLAGS = $(FLAGS) -std=c11
CXXFLAGS = $(FLAGS) -fvisibility-inlines-hidden -std=c++14

all:
	gcc ../golay-24-systematic.c $(CFLAGS) -c
	g++ test.cpp $(CXXFLAGS) -c
	g++ golay-24-systematic.o test.o $(CXXFLAGS) -o test_golay


