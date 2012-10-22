#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "life.h"





int main(int argc, char ** argv)
{
	struct argsStruct arguments = processArgs(argc, argv);
	
	printf("grid size = %i\n", arguments.gridSize);
	printf("threads = %i\n", arguments.numThreads);
	printf("generations = %i\n", arguments.numGenerations);
	printf("input = %s\n", arguments.input);
	printf("output = %s\n\n", arguments.output);

	char ** inputArray = malloc(arguments.gridSize*sizeof(char*));
	if(inputArray == NULL)
	{
		printf("Not enough memory!");
		abort();
	}
	int i;
	for(i = 0; i < arguments.gridSize; i++)
	{
		inputArray[i] = malloc(arguments.gridSize*sizeof(char));
		printf("mallocing inputArray[%i] at : %x\n", i, inputArray[i]);
		if(inputArray[i] == NULL)
		{
			printf("Not enough memory!");
			abort();
		}
	}
	
	for(i = 0; i < 8; i++)
	{
		printf("inputArray[%i] at : %x\n", i, inputArray[i]);
	}

	free(inputArray[7]);
	inputArray[7] = (char*)malloc(sizeof(char)*arguments.gridSize);
	printf("%x\twtf?",&inputArray[7][0]);
	inputArray[7][0] = 2;
	//Sanity check?

	readInputFile(&arguments, inputArray);

	printGrid(&arguments, inputArray);
	


	return 0;
}

void readInputFile(struct argsStruct * args, char ** inputArray)
{
	char * inputLine;
	int i, j;
	for(i = 0; i < 8; i++)
	{
		printf("inputArray[%i] at : %x\n", i, inputArray[i]);
	}
	for(i = 0; i < args->gridSize; i++)
	{
		if(fgets(inputLine, args->gridSize*2+1, args->input) == NULL)
		{
			printf("55, %i: Invalid input file, exiting program.\n", i);
			abort();
		}
		for(j = 0; j < args->gridSize*2; j+=2)
		{
				//printf("readInputFile: i=%i   j=%i\n", i, j);
				printf("&inputArray[%i][%i] = %x\n", i, j/2, &(inputArray[i][j/2]));
				if(inputLine[j] == '0' || inputLine[j] == '1')
				{
					//printf("here\n");
					//fflush(stdout);
					inputArray[i][j/2] = inputLine[j];
					//printf("here\n");
					//fflush(stdout);
				}
				else
				{
					
					printf("66: Invalid input file, exiting program.\n");
					printf("%x\n", inputLine[j]);
					abort();
				}
		}
		
	}
}

void printGrid(struct argsStruct * args, char ** arrayToPrint)
{
	int i, j;
	for(i = 0; i < args->gridSize; i++)
	{
		for(j = 0; j < args->gridSize; j++)
		{
			if(j != (args->gridSize - 1))
			{
				fprintf("%c%c", arrayToPrint[i][j], " ");
			}
			else
			{
				fprintf("%c%c", arrayToPrint[i][j], "\n");
			}
			printf("printGrid: i=%i   j=%i\n", i, j);
		}
	}
}

struct argsStruct processArgs(int argc, char ** argv)
{
	int c;
	opterr = 0;

	struct argsStruct returnArgs;
	returnArgs.gridSize = 0;
	returnArgs.numThreads = 0;
	returnArgs.numGenerations = 0;
	returnArgs.input = NULL;
	returnArgs.output = NULL;

	while((c = getopt(argc, argv, "n:t:r:i:o:")) != -1)
	{
		switch(c)
		{
			case 'n':
				returnArgs.gridSize = atoi(optarg);
				break;
			case 't':
				returnArgs.numThreads = atoi(optarg);
				break;
			case 'r':
				returnArgs.numGenerations = atoi(optarg);
				break;
			case 'i':
				returnArgs.input = fopen(optarg, "r");
				if(returnArgs.input == NULL)
				{
					fprintf(stdout, "unable to open file %s\n", optarg);
				}
				break;
			case 'o':
				returnArgs.output = fopen(optarg, "w+");
				if(returnArgs.output == NULL)
				{
					fprintf(stdout, "unable to open file %s\n", optarg);
				}
				break;
			case '?':
				if(optopt == 'n')
				{
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				}
				if(optopt == 't')
				{
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				}
				if(optopt == 'r')
				{
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				}
				if(optopt == 'i')
				{
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				}
				if(optopt == 'o')
				{
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				}
				else if(isprint(optopt))
				{
					fprintf(stderr, "Unknown option '-%c'.\n", optopt);
				}
				else
				{
					fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
				}
			default:
				abort();
		}
	}
	
	if(returnArgs.gridSize == 0)
	{
		returnArgs.gridSize = DEFAULT_GRID_SIZE;
	}
	if(returnArgs.numThreads == 0)
	{
		returnArgs.numThreads = DEFAULT_NUM_THREADS;
	}
	if(returnArgs.numGenerations == 0)
	{
		returnArgs.numGenerations = DEFAULT_NUM_GENERATIONS;
	}
	if(returnArgs.input == NULL)
	{
		returnArgs.input = stdin;
	}
	if(returnArgs.output == NULL)
	{
		returnArgs.output = stdout;
	}
	
	return returnArgs;
}
