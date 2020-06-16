OFILES=initialize.o execute.o finalize.o
CXXFLAGS = -I../include

iterate:iterate.o libIterate.a
	g++ -o $@ $< -L. -lIterate

libIterate.a:$(OFILES)
	ar rc $@ $?
clean:
	rm -f *.a *.o *~ iterate
