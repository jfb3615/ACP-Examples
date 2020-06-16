CXXFLAGS= -I.. -fPIC
OFILES  = zookeeper.o 

../local/bin/zookeeper: $(OFILES) ../local/lib/libAnimal.a
	mkdir -p ../local
	mkdir -p ../local/bin
	mkdir -p ../local/lib
	$(CXX) $(CXXFLAGS) $(OFILES) -o $@ -ldl

../local/lib/libAnimal.a:Animal.o Mammal.o
	mkdir -p ../local/lib
	ar rc ../local/lib/libAnimal.a Animal.o Mammal.o

clean:
	rm -f *~ $(OFILES) ../ZOO/*~ ../local/bin/zookeeper
