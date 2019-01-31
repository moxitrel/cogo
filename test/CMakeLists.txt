cmake_minimum_required(VERSION 3.13)

# generate exe file named <name>
add_executable(c_benchmark_test)

# exit with 0 to pass and non-zero to fail.
add_test(NAME    c_benchmark_test
         COMMAND c_benchmark_test)

###
### target
###

# sources files for given target.
# path relative to the directory where the add_library() call is made
# PRIVATE   : only add to self                 , e.g. sources, private headers
# INTERFACE : only add to dependencies         , e.g. headers
# PUBLIC    : both add to self and dependencies, e.g. headers
target_sources(c_benchmark_test
  PRIVATE
        await_case_benchmark.c
        benchmark.c
        gen_case_benchmark.c
        gen_label_value_benchmark.c
        gen_lineno_benchmark.c)

target_compile_options(c_benchmark_test
        PRIVATE
            -O)
