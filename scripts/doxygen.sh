#!/bin/sh

cd $(dirname $0)/.. || exit

# Update the old configuration file.
doxygen -u

# Generate the documentation.
doxygen
