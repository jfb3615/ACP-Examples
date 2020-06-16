CXXFLAGS=$(shell pkg-config --cflags --libs QatGenericFunctions)  -std=c++17


gauss:gauss.cpp
	$(CXX) $< $(CXXFLAGS) -o $@
clean:
	rm -f gauss *~ 
