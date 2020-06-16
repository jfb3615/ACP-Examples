targetdir = ../local

TMCPPFILES           =  $(shell find . -name "*.cpp" )
MYCPPFILES           =  $(patsubst ./%,   %,   $(TMCPPFILES))
MYOFILES             =  $(patsubst %.cpp, %.o, $(MYCPPFILES))
DSOFILES             =  $(patsubst %.cpp, $(targetdir)/Animals/%.so, $(MYCPPFILES))


override CXXFLAGS  +=  -I.. -g -O0  -fPIC

%.cpp.%.o:
	$(CXX) -c -g -O0 $< -o $@ $(CXXFLAGS)

$(targetdir)/Animals/%.so : %.o
	mkdir -p $(targetdir)/Animals
	mkdir -p $(targetdir)/Animals/`dirname $<`
	cp $< $(targetdir)/Animals/`dirname $<`
	g++ -shared  -o $@ $<  -L../local/lib -lAnimal
	rm -f $(targetdir)/Animals/$<


all: $(DSOFILES)

$(DSOFILES):$(MYOFILES)

clean:
	rm *.o 



