#! /bin/bash

# Create dirs
mkdir $(dirname "$0")/BUILD &> /dev/null
cd $(dirname "$0")/BUILD

# Configure
cmake .. &&

# Build
cmake --build . -- -j 8
