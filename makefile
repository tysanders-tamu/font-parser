clean:
	rm -f core

core: main.cpp
	g++ -I fmt/include -DFMT_HEADER_ONLY -g -o core main.cpp

run: clean core
	./core
