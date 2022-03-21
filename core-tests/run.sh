#!/bin/bash
export GTEST_OUTPUT="xml:./test-result.xml"
cmake .
make -j8
ipcs -q
./bin/core_tests
