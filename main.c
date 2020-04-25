#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#if __has_include(<mpi.h>)
    #define _IS_MPI 1
    #include <mpi.h>
#endif

#define ECB 1

#include "aes.h"

#define CRLF "\r\n"
#define TAB "\t"
#define CORES 4

int parseArgs(int argc, char* argv[], char ** fileName, char ** key, char ** outFileName, int * verbose, int * openmpi) {
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-file") == 0) {
			if ((argv[i + 1] != NULL) && strlen(argv[i + 1]) > 0) {
				*fileName = (char*)malloc((strlen(argv[i + 1]) + 1) * sizeof(char));
				strcpy(*fileName, argv[i + 1]);
			}
		} else if (strcmp(argv[i], "-key") == 0) {
			if ((argv[i + 1] != NULL) && strlen(argv[i + 1]) > 0) {
				*key = (char*)malloc((strlen(argv[i + 1]) + 1) * sizeof(char));
				strcpy(*key, argv[i + 1]);
			}
		} else if (strcmp(argv[i], "-out") == 0) {
			if ((argv[i + 1] != NULL) && strlen(argv[i + 1]) > 0) {
				*outFileName = (char*)malloc((strlen(argv[i + 1]) + 1) * sizeof(char));
				strcpy(*outFileName, argv[i + 1]);
			}
		} else if (strcmp(argv[i], "-verbose") == 0) {
			*verbose = 1;
		} else if (strcmp(argv[i], "-openmpi") == 0){
                        *openmpi = 1;
                }
	}
	if (*fileName != NULL && *key != NULL) {
		return 0;
	}
	return -1;
}


void zeroArray(uint8_t * arr, int size) {
	for (int i = 0; i < size; i++) {
		arr[i] = (uint8_t)0x00;
	}
}


void printArray(uint8_t* arr, int size, char * format) {
	for (int i = 0; i < size; i++) {
		printf(format, arr[i]);
	}
	printf(CRLF);
}

uint8_t* makeKey(char* string) {
	int size = strlen(string);
	uint8_t* result = (uint8_t*)malloc(size * sizeof(uint8_t));
	for (int i = 0; i < size + 1; i++) {
		result[i] = (uint8_t)string[i];
	}
	return result;
}

uint8_t* readFile(char * fileName, int * size) {
	FILE* file = fopen(fileName, "r");
	uint8_t* result = NULL;
	if (file == NULL){
		printf("Error! Could not open file\n");
		return result;
	}
	fseek(file, 0, SEEK_END);
	*size = ftell(file);
	fseek(file, 0, SEEK_SET);
	int padding = (*size) % 16;
	result = (uint8_t*)malloc((*size + padding) * sizeof(uint8_t));
	for (int i = 0; i < *size + 1; i++) {
		fscanf(file, "%c", &(result[i]));
	}
	for (int i = *size; i < *size + padding; i++) {
		result[i] = (uint8_t)0x00;
	}
	*size += padding;
	fclose(file);
	return result;
}

void writeFile(char * fileName, uint8_t* input, int size, char * format) {
	FILE* file = fopen(fileName, "w");
	if (file == NULL) {
		printf("Error! Could not open file\n");
		return;
	}
	for (int i = 0; i < size + 1; i++) {
		fprintf(file, format, input[i]);
	}
	fclose(file);
}


//
// Mandatory arguments:
// -key <password> - the password used to encrypt the text file
// -file <in file name> - input file name, has to be a text file
// -out <out file name> - output file name
//
// Optional arguments:
// -verbose - see additional output
// -openmpi - run using openMpi
//
int main(int argc, char * argv[]) {

	char * fileName = NULL;
	char * charKey = NULL;
	char * outFileName = NULL;
	int verbose = 0;
        int openmpi = 0;

	int success = 
            parseArgs(argc, argv, &fileName, &charKey, &outFileName, &verbose, &openmpi);

	if (success < 0) {
		printf("Invalid parameters passed.");
		return -1;
	}

	int size = 0;
	uint8_t* input = readFile(fileName, &size);

	if (input != NULL) {

	    clock_t start, end, delta;

            if (verbose == 1) {
			printf("Size=%d", size); printf(CRLF);
			printf("Input"); printf(CRLF);
			printArray(input, size, "%c");
            }

	    uint8_t* key = makeKey(charKey);

	    if (verbose == 1) {
		printf("Key"); printf(CRLF);
		printArray(key, 16, "%c");
            }

	    uint8_t* output = (uint8_t*)malloc(size  * sizeof(uint8_t));
	    uint8_t* output2 = (uint8_t*)malloc(size * sizeof(uint8_t));
	    zeroArray(output, size);
	    zeroArray(output2, size);

            if(openmpi == 0){
		    // Sequential
		    start = clock();
		    AES_ECB_encrypt(input, key, output, size);
		    end = clock();
		    delta = end - start;
		    printf("Sequential took %ld ms.", delta); printf(CRLF);

		    zeroArray(output, size);
		    zeroArray(output2, size);

		    // Parallel
		    int chunkSize = size / CORES;
		    if (chunkSize < 16) {
			chunkSize = 16;
		    }
		    start = clock();
		    #pragma omp parallel
		    #pragma omp for
		    for (int i = 0; i < CORES; i++){
			if (i * chunkSize <= size) {
			    AES_ECB_encrypt(input + (i * chunkSize), key, output + (i * chunkSize), chunkSize);
			}
		    }
		    end = clock();
		    delta = end - start;
		    printf("Parallel took %ld ms.", delta); printf(CRLF);
		}
		if(openmpi == 1){
                    // Open MPI
		    #ifndef _IS_MPI
                    printf("OpenMpi not supported."); printf(CRLF);
                    #else
                    MPI_Init(NULL, NULL);
                    int wSize, rank;
                    MPI_Comm_size(MPI_COMM_WORLD, &wSize);
                    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                    int elementsPerProc = size / wSize;
                    uint8_t * sub = (uint8_t *)malloc(sizeof(uint8_t) * elementsPerProc);
                    MPI_Scatter(input, elementsPerProc, MPI_INT, sub, elementsPerProc, MPI_INT, 0, MPI_COMM_WORLD);
                    start = clock();
		    AES_ECB_encrypt(input + (rank * elementsPerProc), key, sub, elementsPerProc);
                    MPI_Gather(&sub, elementsPerProc, MPI_INT, output + (rank * elementsPerProc), elementsPerProc, MPI_INT, 0, MPI_COMM_WORLD);
                    end = clock();
		    delta = end - start;
		    printf("OpenMpi took %ld ms.", delta); printf(CRLF);
                    MPI_Finalize();
                    #endif
               }

		// Logging and verification
		if (verbose == 1) {
			printf("Output"); printf(CRLF);
			printArray(output, size, "%.2X ");
		}
		start = clock();
		if (outFileName != NULL) {
			writeFile(outFileName, output, size, "%.2X");
		}
		end = clock();
		delta = end - start;
		printf("Writing took %ld ms.", delta); printf(CRLF);

		AES_ECB_decrypt(output, key, output2, size);

		if (verbose == 1) {
			printf("Output 2"); printf(CRLF);
			printArray(output2, size, "%c");
		}
		return 0;
	}
}

