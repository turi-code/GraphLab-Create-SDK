all: example1.so sgraph_example.so

example1.so: example1.cpp
	g++  -std=c++11 $^ -I../ -I/home/haijieg/graphlab/graphlab-dev-gerrit/deps/local/include -shared -fPIC -o $@

sgraph_example.so: sgraph_example.cpp
	g++  -std=c++11 $^ -I../ -shared -fPIC -o $@
