#include "sr.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(_core, m) {
    m.def("cast_rn", &cast_fp_nearest, 
    py::arg("in"), py::arg("man_bits"), py::arg("exp_bits"),
    py::arg("subnormals") = true, py::arg("saturate") = false, 
    R"pbdoc(
        Performs round to nearest even on a given input value.

        Rounds to nearest even a given IEEE-754 binary64 
        floating-point value to the target IEEE-754 compatible 
        format with `man_bits` mantissa width and `exp_bits` 
        exponent width. Subnormals are enabled by default with 
        the `subnormals` parameter, whereas saturation logic is 
        controlled with the saturate parameter.
    )pbdoc");

    m.def("cast_sr", &cast_fp_stochastic, py::arg("in"), 
    py::arg("man_bits"), py::arg("exp_bits"), py::arg("prng_bits"),
    py::arg("subnormals") = true, py::arg("saturate") = false,
    R"pbdoc(
        Performs parameterized stochastic rounding on a given input value.

        Stochastically rounds (mode 1) a given IEEE-754 binary64 
        floating-point value to the target IEEE-754 compatible 
        format with `man_bits` mantissa width and `exp_bits`
        exponent width. The width of the PRNG used to perform
        the rounding is controlled through the `prng_bits` parameter.
        Subnormals are enabled by default with the `subnormals`
        parameter, whereas saturation logic is controlled with
        the saturate parameter.
    )pbdoc");

    m.def("seed", &seed, py::arg("seed"),
     R"pbdoc(
        Sets a seed for the PRNG used for the SR operations.
    )pbdoc");

    m.def("sum_vec_sr", &sum_vec_stochastic, py::arg("in"), py::arg("batch_size"),
    py::arg("man_bits"), py::arg("exp_bits"), py::arg("prng_bits"),
    py::arg("subnormals") = true, py::arg("saturate") = false,
    R"pbdoc(
        TODO
    )pbdoc");

    m.def("sum_vec_rn", &sum_vec_nearest, py::arg("in"), py::arg("batch_size"),
    py::arg("man_bits"), py::arg("exp_bits"),
    py::arg("subnormals") = true, py::arg("saturate") = false,
    R"pbdoc(
        TODO
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}