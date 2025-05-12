# A MIP-based algorithm to solve the Time-Dependent TSP

This implementation supplements the article

[Hansknecht, C.; Joormann, I.; Stiller, S. "Dynamic Shortest Paths Methods for the Time-Dependent TSP." Algorithms 2021, 14, 21.](https://dx.doi.org/10.3390/a14010021)

## Dependencies

This code depends on the following packages:

- A `C++` compiler supporting `C++ 17`, such as [gcc](https://gcc.gnu.org/) or [clang](https://clang.llvm.org/)
- [CMake](https://cmake.org/) `>= 3.10`
- [Boost](https://www.boost.org/) `>= 1.70`
- [GoogleTest](https://google.github.io/googletest/)
- [SCIP](https://scipopt.org/) `>= 7.0.0`

## Build

The following commands build and test the code when executed in the
root folder of the project:

- `mkdir build`
- `cd build`
- `cmake ..`
- `make`

## Tests

To build and run the unit tests following the build, execute the following
commands in the `build/` folder:

- `make build_tests`
- `make test`

## Benchmarks

This code includes several benchmarks used to obtain the results in
the article mentioned above. To build the benchmarks following the
build, execute the following commands in the `build/` folder:

- `make build_benchmarks`

The benchmarks are stored as executables in the `src/perf` subfolder
of the `build/` folder. Following the build of the benchmarks, you can
run all benchmarks and collect the results by executing

- `make collect`

The results will be stored as `.csv` files in the `src/perf` subfolder
of the `build` folder. Alternatively, you can collect the results from
individual benchmarks using

- `make collect_<benchmark>_<size>`

where size is either `medium` or `large` (again, see the article for
details). This will produce a file `<benchmark>_<size>.csv` in the
same folder as mentioned above.

Alternatively, you can run the benchmarks on their own in order to
study the behavior of the solvers during the execution.

**Beware:** To obtain correct (i.e. comparable) timing results, ensure
the following:

- Instruct `cmake` to compile optimized code by setting an appropriate
  [build type](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html).
- Use [Gurobi](https://gurobi.com/) as a back end for SCIP.
- Ensure that SCIP itself is compiled in release mode (this *should* be the case by default)

## Instances

The instances were generated as mentioned in the article, using a
deterministic random number generator. Thus far, they are not
persisted. If you need access to instance files, please contact us.
