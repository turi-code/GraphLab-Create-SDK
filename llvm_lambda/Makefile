CXX := g++
CXXFLAGS := -std=c++11 -I..
SHARED_LIBRARY_FLAGS := -shared -fPIC

LLVM_HOME := /Users/haijieg/local/llvm-3.6.2-release
LLVMFLAGS := $(shell $(LLVM_HOME)/bin/llvm-config  --cxxflags --ldflags --libs all) -lncurses -lz

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	CXXFLAGS += --stdlib=libc++ -undefined dynamic_lookup
endif

# LLVM_LAMBDA extension
llvm_lambda.so: llvm_lambda_extension.cpp llvm_lambda.cpp
	$(CXX) -o $@ $(CXXFLAGS) $(LLVMFLAGS) $(SHARED_LIBRARY_FLAGS) -O2 $^

llvm_lambda_test: llvm_lambda_test.cpp llvm_lambda.cpp
	$(CXX) -o $@ $(CXXFLAGS) $(LLVMFLAGS) $^

# JIT example
jit_example: jit_example.cpp
	$(CXX) -o $@ $(CXXFLAGS) $(LLVMFLAGS) $^
