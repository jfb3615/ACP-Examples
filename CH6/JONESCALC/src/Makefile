CPPFLAGS=-I.. -g -O0
INCLUDES=../JONESCALC/JonesVector.h ../JONESCALC/JonesVector.icc \
         ../JONESCALC/JonesMatrix.h ../JONESCALC/JonesMatrix.icc

../local/bin/polarization:polarization.cpp $(INCLUDES)
	mkdir -p ../local/bin
	$(CXX) $< $(CPPFLAGS) $(LDFLAGS) -o $@ 

clean:
	rm -rf *~ ../JONESCALC/*~ ../local
