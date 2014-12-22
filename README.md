GraphLab Create 1.x SDK (BETA)
==============================

The GraphLab Create SDK aims to provide 3rd party extensibility to GraphLab Create. The SDK provides:

- A really simple C++ to GraphLab Create to Python FFI interface.
High level native access to the GraphLab Create datastructures (SArray, SFrame and SGraph)

- It also provides a way for certain SFrame / SGraph Python operators which take functions (like SArray.apply, SFrame.apply or SGraph.triple_apply) to use native functions instead of Python functions.

Minimum Requirements
--------------------
*Linux*:

- GraphLab Create >= 1.1 installation
- gcc 4.8 and above

*Mac*:

- GraphLab Create >= 1.1 installation
- XCode 6.1 Command Line Tools. (Apple LLVM version 6.0 (clang-600.0.54))

The sdk also depends on boost 1.55 or above (headers only). We've included a subset of dependent boost 1.56 header files under boost/.

Download SDK
-------------
```
git clone https://github.com/graphlab-code/GraphLab-Create-SDK.git graphlab-sdk
```

Build Examples
--------------
```
cd graphlab-sdk && make
```

Use the Example Extensions
--------------------------
```
graphlab-sdk$ ipython
>>> import graphlab
>>> import sdk_example.example1 as example1
>>> example1.add(2, 5)
7
```

Documentation
-------------
[http://graphlab.com/products/create/sdk/docs](http://graphlab.com/products/create/sdk/docs/index.html)

Alternatively, you can type `make doc` to build local documentations (requires Doxygen).

License
-------
This SDK is provided under the 3-clause BSD [license](LICENSE).
