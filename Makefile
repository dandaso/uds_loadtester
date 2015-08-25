appname := uds_loadtest

CXX := g++
CXXFLAGS := -Wall -g

srcfiles := $(shell find . -maxdepth 2 -name "*.cpp")
objects  := $(patsubst %.cpp, %.o, $(srcfiles))

LDLIBS	 := -lboost_thread-mt -lboost_system -lboost_program_options-mt

all: $(appname)

$(appname): $(objects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname) $(objects) $(LDLIBS)

depend: .depend

.depend: $(srcfiles)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:	
	rm -f $(objects)

dist-clean: clean
	rm -f *~ .depend

include .depend
