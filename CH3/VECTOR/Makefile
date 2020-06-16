CXXFLAGS = $(shell pkg-config --cflags eigen3)
target:vectEx

../local/bin/%:$%.cpp
	mkdir -p ../local/bin
	$(CXX) $< $(CPPFLAGS) $(LDFLAGS) -o $@ 

clean:
	rm -f *~ ../local/bin/vectEx1 ../local/bin/vectEx2

