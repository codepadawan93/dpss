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

The default name of the executable is ``` dpss ```. Run it with the following command:

```
mkdir output
./dpss -file ./resources/bigfile.txt -key testpassword1234 -out ./output/bigfile.enc
```

The following options can be specified:

- ```-file <filename>``` - the name of the input file (mandatory)
- ```-key <password>``` - the encryption key (mandatory)
- ```-out <filenme>``` - the name of the output file
- ```-verbose``` - show more output

