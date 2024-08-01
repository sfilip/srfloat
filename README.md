# `srfloat` parameterizable stochastic rounding library for floating-point arithmetic
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

This is a proof of concept library for experimenting with limited-precision stochastic rounding-based arithmetic.

### Prerequisites:
You should have `cmake` on your system in order to build the project. For the Python bindings, `pybind11` should
be available on your system.

### Build:
From the root project directory, building the executable(s) is as easy as running the following commands:
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```
By default, an executable called `example` is generated that shows the major capabilities available in the library. You can take a look at `tests/example.cpp` for what exactly is being done.

### Python bindings:
A minimal Python interface is provided as well. To build it and install the `srfloat` module in your current Python environment, just run 
```
pip install -e .
```
from inside the project parent directory. For the moment, two functions are exposed to the user `cast_rn` and `cast_sr` for casting a `binary64` floating-point value to a lower precision floating-point value using round to nearest ties to even or stochastic rounding, respectively. Setting the seed of the PRNG used in the stochastic rounding calls can be done through the `seed` method. An example script can be found in `tests/srfloat_script.ipynb` (it assumes `mpmath` is installed in order to do multiple precision computations in Python). 

### Usage:
#### C++

The user can use one of the provided quantization functions directly on binary64 data (`cast_fp_nearest` or `cast_fp_stochastic` from `sr.h`) or the object-oriented interface that exposes custom precision floating-point data types with stochastic rounding computations (addition and multiplication).

An example program of how this could look like is the following:
```cpp
#include "srfloat.h"
#include "sr.h"
#include <iostream>

// configuration for the low precision format (M, E, R)
const int man_bits = 2;
const int exp_bits = 5;
const int r_bits = 10;
// typedefs for SR-enabled floating-point formats
// this is just for convenience to more easily declare
// and initialize values afterwards
typedef srfloat<exp_bits, man_bits, r_bits> fp8e5m2_r10;

int main() {
    fp8e5m2_r10 x{0.2};
    fp8e5m2_r10 y{1.75};
    std::cout << "x = " << x << std::endl;
    std::cout << "y = " << y << std::endl;

    fp8e5m2_r10 z = x + y;
    std::cout << "SR(x + y) = " << z << std::endl;

    return 0;    
}
```

#### Python
For the moment, the Python interface exposes only the quantization functions from the C++ interface. An equivalent Python script to the C++ one would look like the following:

```python
import srfloat as sr

def main():
    man_bits, exp_bits, r_bits = 2, 5, 10

    cast_fp8e5m2_r10 = lambda x: sr.cast_sr(x, man_bits, exp_bits, r_bits, subnormals=True, saturate=False)
    x = cast_fp8e5m2_r10(0.2)
    y = cast_fp8e5m2_r10(1.75)
    print(f'x = {x}')
    print(f'y = {y}')

    z = cast_fp8e5m2_r10(x + y)
    print(f'SR(x + y) = {z}')

if __name__ == '__main__':
    main()
```