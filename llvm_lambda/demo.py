
#### Demo ####

import graphlab as gl
import llvm_lambda


def add_one(x):
    return x + 1

code = llvm_lambda.compile_to_llvm(add_one, 0)
print code[:800]



sa = gl.SArray(range(1, 1000001))  # 1M
%time llvm_lambda.apply(sa, add_one).__materialize__()
%time sa.apply(add_one).__materialize__()
%time (sa + 1).__materialize__()


# Loop optimization
def add_thousand(x):
    for _ in xrange(1000):
        x = x + 1
    return x

print llvm_lambda.compile_to_llvm(add_thousand, 0)[:800]

sa = gl.SArray(range(1, 50001))  # 50k
%time llvm_lambda.apply(sa, add_thousand).__materialize__()
%time sa.apply(add_thousand).__materialize__()


# Capturing global variable BY VALUE
y = 10
def add_y(x):
    return x + y

print llvm_lambda.compile_to_llvm(add_y, 0)[:800]
llvm_lambda.apply(sa, add_y)


def fib(n):
    a, b = 1, 1
    for i in xrange(n - 1):
        a, b = b, a + b
    return min(2 ** 32, a)

sa = gl.SArray(range(1, 10001))
print llvm_lambda.compile_to_llvm(fib, 0)[:1000]

%time llvm_lambda.apply(sa, fib)
%time sa.apply(fib).__materialize__()


# More examples
%time llvm_lambda.apply(sa, lambda x: x % 13).__materialize__()
%time sa.apply(lambda x: x % 13).__materialize__()


%time llvm_lambda.apply(sa, lambda x: abs(-x)).__materialize__()
%time sa.apply(lambda x: abs(-x)).__materialize__()


###### Failure cases #######

# Front end limitation, numba cannot compile python -> llvm native #
# Backend limitation, llvm cannot execute numba compiled llvm #

# 1. only supports numerical types
def to_str(x):
    return str(x)
llvm_lambda.compile_to_llvm(to_str, 0)

def from_str(x):
    return int(x)
llvm_lambda.compile_to_llvm(from_str, '1')


# 2. no function capturing
def add_two(x):
    return add_one(add_one(x))
llvm_lambda.compile_to_llvm(add_two, '1')


# 3. native library not supported SAD!!!!
import random
def add_random(x):
    return x + random.random()
print llvm_lambda.compile_to_llvm(add_random, 1)[:1500]
llvm_lambda.apply(sa, add_random)


# 4. using non-standard llvm functions!!!!
def square(x):
    return x ** 2
print llvm_lambda.compile_to_llvm(square, 1)[:1000]
llvm_lambda.apply(sa, square)
