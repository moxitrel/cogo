cmake_minimum_required(VERSION 3.13)

# generate exe file named <name>
add_executable(c_chan)

###
### target
###

# sources files for given target.
# path relative to the directory where the add_library() call is made
# PRIVATE   : only add to self                 , e.g. sources, private headers
# INTERFACE : only add to dependencies         , e.g. headers
# PUBLIC    : both add to self and dependencies, e.g. headers
target_sources(c_chan
  PRIVATE
        chan_example.c)

target_compile_options(c_chan
        PRIVATE
            -O)
