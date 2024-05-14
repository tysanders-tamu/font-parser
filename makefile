clean:
	rm -f core

core: main.cpp
	g++ -I fmt/include -g -o core main.cpp

run: clean core
	./core
