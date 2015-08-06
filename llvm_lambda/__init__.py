HAS_NUMBA = False
try:
    import numba
except:
    print "numba not found"
    pass

print "numba found"
HAS_NUMBA = True

if HAS_NUMBA:
    from numba import jit
    import sys
    conda_lib = sys.prefix + '/lib/libpython2.7.dylib'
    import llvm_lambda
    llvm_lambda.init_llvm()
    print "llvm lambda activated"
    llvm_lambda.add_shared_library(conda_lib)

    def compile_to_llvm(f, x):
        f2 = jit(nopython=True, nogil=True)(f)
        f2(x)
        return f2.inspect_llvm().values()[0]

    def apply(sa, f):
        llvm_code = compile_to_llvm(f, sa[0])
        print llvm_code
        return llvm_lambda.apply_sa(sa, llvm_code)
