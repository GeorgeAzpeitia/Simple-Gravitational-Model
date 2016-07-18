# Makefile for Simple Gravitational Model
# George Azpeitia

all: gravcpp gravHS

gravcpp:
	g++ -std=c++11 grav.cpp -lGL -lGLU -lGLEW -lglut -o gravcpp
gravHS:
	ghc --make gravHS.hs
clean:
	$(RM) *~ *.o

spotless: clean
	$(RM) $(PROGS)