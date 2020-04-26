#!/bin/bash
echo "Running test 1 - linear + parallel";
./dpss -file ./resources/bigfile.txt -key testpassword1234 -out ./output/bigfile.enc &&
echo "Running test 2 - linear + parallel verbose" || echo "Test 1 failed";
./dpss -file ./resources/testfile.txt -key testpassword1234 -out ./output/testfile.enc -verbose &&
echo "Running test 3 - OpenMPI" || echo "Test 2 failed";
mpirun -np 2 ./mpidpss -file ./resources/testfile.txt -key testpassword1234 -out ./output/testfile_mpi.enc -openmpi -verbose &&
echo "Tests ran successfully" || echo "Test 3 failed"
