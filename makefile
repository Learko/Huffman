CXX = g++-7
CXXFLAGS = -Wall --pedantic -O3 -std=c++1z
LDFLAGS = -lboost_serialization

all: huffman

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

huffman.o: huffman.cpp
	$(CXX) $(CXXFLAGS) -c huffman.cpp


huffman: main.o huffman.o
	$(CXX) main.o huffman.o $(LDFLAGS) -o huffman

clean:
	rm *.o

clean_enc:
	rm *.dec *.huf