CC := gcc
CXX := g++
CFLAGS := -std=c++11 -I . -shared -fPIC

ifeq ($(UNAME),Darwin)
	CFLAGS += --stdlib=libc++ -undefined dynamic_lookup
endif

all: sdk_example 

clean:
	rm sdk_example/*.so

#### SDK Examples ##### 
EXAMPLE_SRCS := $(wildcard sdk_example/*.cpp)
EXAMPLE_TARGETS := $(EXAMPLE_SRCS:%.cpp=%.so)

sdk_example : $(EXAMPLE_TARGETS)

sdk_example/%.so: sdk_example/%.cpp
	$(CXX) -o $@ $(CFLAGS) $^
