clean:
	rm -f core

core: main.cpp
	g++ -o core main.cpp

run: clean core
	./core
