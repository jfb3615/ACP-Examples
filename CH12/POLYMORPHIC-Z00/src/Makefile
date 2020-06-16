CXXFLAGS= -I.. -std=c++17
OFILES  = zookeeper.o Animal.o Mammal.o Fish.o Rabbit.o Whale.o

../local/bin/zookeeper: $(OFILES)
	mkdir -p ../local
	mkdir -p ../local/bin
	mkdir -p ../local/lib
	$(CXX) $(CXXFLAGS) $(OFILES) -o $@
clean:
	rm -f *~ $(OFILES) ../ZOO/*~ ../local/bin/zookeeper
