all: search

search: main.o search.o thread_pool.o
	g++ -std=c++17 -pthread main.o search.o thread_pool.o -o search

main.o: main.cpp
	g++ -std=c++17 -pthread -c main.cpp

search.o: search.cpp
	g++ -std=c++17 -pthread -c search.cpp

thread_pool.o: thread_pool.cpp
	g++ -std=c++17 -pthread -c thread_pool.cpp

clean:
	rm -rf *.o search
