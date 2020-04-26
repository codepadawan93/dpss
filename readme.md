# DPSS Project

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

## Prerequisites

GNU GCC should be available on the system. With a bit of labour this can also be compiled with Visual Studio.

## Building

Build the tool using the shell script provided:

```
./build.sh
```
If this fails due to privileges, make the script executable and run again:

```
sudo chmod 777 build.sh
```

## Running

Make sure the ``` output ``` directory exists:

```
mkdir output
```

The default name of the normal executable is ``` dpss ```. Run it with the following command:

```
./dpss -file ./resources/bigfile.txt -key testpassword1234 -out ./output/bigfile.enc
```

The default name of the MPI executable is ``` mpidpss ```. Run it with the following command:

```
mpirun -np 2 ./mpidpss -file ./resources/testfile.txt -key testpassword1234 -out ./output/testfile_mpi.enc -openmpi
```

## Running tests

Run the (very basic) tests like this:
```
./test.sh
```

The following options can be specified:

- ```-file <filename>``` - the name of the input file (mandatory)
- ```-key <password>``` - the encryption key (mandatory)
- ```-out <filenme>``` - the name of the output file
- ```-verbose``` - show more output
- ```-openmpi``` - run the OpenMpi code
