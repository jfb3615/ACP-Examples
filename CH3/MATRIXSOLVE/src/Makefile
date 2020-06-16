CPPFLAGS=-I.. $(shell pkg-config --cflags eigen3) -g -O0 
target=matrixsolve
../local/bin/$(target):$(target).cpp
	mkdir -p ../local/bin
	$(CXX) $< $(CPPFLAGS) $(LDFLAGS) -o $@ 

clean:
	rm -f *~ ../local/bin/$(target)

