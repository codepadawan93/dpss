#!/bin/bash

gcc -fopenmp main.c aes.h aes.c -o dpss &&
echo "Normal version successfully built" || echo "Normal version build failed";
mpicc -fopenmp main.c aes.h aes.c -o mpidpss &&
echo "Mpi version successfully built" || echo "Mpi version build failed";
