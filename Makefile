# use g++ with C++11 support
CXX=g++
CXXFLAGS?=-Wall -pedantic -g -O0 -std=c++11
SUBMISSIONFILES=graph.o
TESTFILES=GraphTest

all: $(SUBMISSIONFILES) $(TESTFILES)

GraphTest: GraphTest.cpp graph.o
	$(CXX) $(CXXFLAGS) -o GraphTest graph.o GraphTest.cpp

graph.o: Graph.cpp Graph.h
	$(CXX) $(CXXFLAGS) -c -o graph.o Graph.cpp

clean:
	$(RM) $(SUBMISSIONFILES) $(TESTFILES)  *.o

