#!/bin/bash
echo "Running test 1 - linear + parallel" &&
./dpss -file ./resources/bigfile.txt -key testpassword1234 -out ./output/bigfile.enc &&
echo "Running test 2 - linear + parallel verbose"
./dpss -file ./resources/bigfile.txt -key testpassword1234 -out ./output/bigfile.enc -verbose &&
echo "Running test 3 - OpenMPI"
./dpss -file ./resources/bigfile.txt -key testpassword1234 -out ./output/bigfile.enc -openmpi &&
echo "Tests ran successfully"
