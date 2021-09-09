run:
	g++ -O3 -std=c++11 UniqueWordsCounter.cpp UniqueWordsCounterManager.cpp main.cpp -o run -pthread

clean:
	rm -f run