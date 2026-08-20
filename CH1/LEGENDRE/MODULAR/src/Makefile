OFILES=parseInput.o getCanvas.o plotLegendre.o
CXXFLAGS = -I../include -std=c++20

legendre:legendre.o libLegendre.a
	c++ -o $@ $< -L. -lLegendre

libLegendre.a:$(OFILES)
	ar rc $@ $?
clean:
	rm -f *.a *.o *~ legendre
