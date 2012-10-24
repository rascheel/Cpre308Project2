#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include "life.h"


char ** currGrid;
char ** nextGrid; 
int cellsLeft;
int generationsDone;
pthread_mutex_t mutex;
pthread_cond_t oneGenDone;

int main(int argc, char ** argv)
{
	struct argsStruct arguments = processArgs(argc, argv);
	pthread_t * threads;


	printf("grid size = %i\n", arguments.gridSize);
	printf("threads = %i\n", arguments.numThreads);
	printf("generations = %i\n", arguments.numGenerations);
	printf("input = %s\n", arguments.input);
	printf("output = %s\n\n", arguments.output);

	currGrid = malloc(arguments.gridSize*sizeof(char*));
	if(currGrid == NULL)
	{
		printf("Not enough memory!");
		abort();
	}
	int i;
	for(i = 0; i < arguments.gridSize; i++)
	{
		currGrid[i] = malloc(arguments.gridSize*sizeof(char));
		if(currGrid[i] == NULL)
		{
			printf("Not enough memory!");
			abort();
		}
	}
	readInputFile(&arguments, currGrid);

	if(arguments.numThreads < 2)
	{
		printf("Not enough threads. Minimum is 2.");
		abort();
	}


	nextGrid = malloc(arguments.gridSize*sizeof(char*));
	if(nextGrid == NULL)
	{
		printf("Not enough memory");
		abort();
	}
	for(i = 0; i < arguments.gridSize; i++)
	{
		nextGrid[i] = malloc(arguments.gridSize*sizeof(char));
		if(nextGrid[i] == NULL)
		{
			printf("Not enough memory!");
			abort();
		}
	}

    
	cellsLeft = arguments.gridSize*arguments.gridSize;
	generationsDone = 0;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&oneGenDone, NULL);
    struct threadArgs * tArgs = malloc(sizeof(struct threadArgs)*arguments.numThreads);
    threads = malloc(sizeof(pthread_t)*arguments.numThreads-1);
    for(i = 0; i < arguments.numThreads-1; i++)
    {
		tArgs[i].gridSize = arguments.gridSize;
		tArgs[i].numGenerations = arguments.numGenerations;
        tArgs[i].startRow = i*(arguments.gridSize/(arguments.numThreads-1));
		tArgs[i].numOfRow = arguments.gridSize/(arguments.numThreads-1);
		printf("before Thread: startRow = %i, numOfRow = %i\n", tArgs[i].startRow, tArgs[i].numOfRow);
		fflush(stdout);
        pthread_create(&threads[i], NULL, (void *)&workerThread, &tArgs[i]);
	}
	
	for(i = 0; i < arguments.numThreads-1; i++)
	{
		pthread_join(threads[i], NULL);
	}




	printf("before printing \n");
	fflush(stdout);
	printGrid(&arguments, currGrid);



/*
	printf("nextCell[%i][%i] = %c\n", 0, 0, nextCell(&arguments, 0, 0));
	printf("nextCell[%i][%i] = %c\n", 7, 0, nextCell(&arguments, 7, 0));
	printf("nextCell[%i][%i] = %c\n", 0, 7, nextCell(&arguments, 0, 7));
	printf("nextCell[%i][%i] = %c\n", 7, 7, nextCell(&arguments, 7, 7));
	printf("nextCell[%i][%i] = %c\n", 0, 1, nextCell(&arguments, 0, 1));
	printf("nextCell[%i][%i] = %c\n", 7, 1, nextCell(&arguments, 7, 1));
	printf("nextCell[%i][%i] = %c\n", 1, 0, nextCell(&arguments, 1, 0));
	printf("nextCell[%i][%i] = %c\n", 1, 7, nextCell(&arguments, 1, 7));
	printf("nextCell[%i][%i] = %c\n", 1, 1, nextCell(&arguments, 1, 1));
	printf("nextCell[%i][%i] = %c\n", 2, 5, nextCell(&arguments, 2, 5));
*/	


	return 0;
}

void workerThread(void * arg)
{
	struct threadArgs * tArgs = (struct threadArgs*)arg;
	printf("startRow = %i, numOfRow = %i\n", tArgs->startRow, tArgs->numOfRow);
	fflush(stdout);
	char lastThread = 0;
	while(generationsDone != tArgs->numGenerations)
	{
			int i;
			int j;
			int counter = 0;
			for(i = tArgs->startRow; i < tArgs->numOfRow+tArgs->startRow; i++)
			{
				for(j = 0; j < tArgs->gridSize; j++)
				{
					counter++;
					nextGrid[i][j] = nextCell(tArgs->gridSize, i, j);
				}
			}

			pthread_mutex_lock(&mutex);
			printf("cellsleft = %i\n", cellsLeft);
			cellsLeft -= counter;
			printf("I removed %i cells.\n", counter);
			
			if(cellsLeft == 0)
			{
				printf("I'm the last thread 1.\n");
				fflush(stdout);

				lastThread = 1;
				cellsLeft = tArgs->gridSize*tArgs->gridSize;
				free(currGrid);
				currGrid = nextGrid;
				nextGrid = malloc(tArgs->gridSize*sizeof(char*));
				if(nextGrid == NULL)
				{
					printf("Not enough memory");
					abort();
				}
				for(i = 0; i < tArgs->gridSize; i++)
				{
					nextGrid[i] = malloc(tArgs->gridSize*sizeof(char));
					if(nextGrid[i] == NULL)
					{
						printf("Not enough memory!");
						abort();
					}
				}				
				generationsDone++;
				pthread_cond_broadcast(&oneGenDone);
			} 
			if(!lastThread)
			{
				printf("waiting\n");
				fflush(stdout);
				pthread_cond_wait(&oneGenDone, &mutex);
				printf("done waiting\n");
				fflush(stdout);
			}
			else
			{
				printf("I'm the last thread.\n");
				fflush(stdout);
				lastThread = 0;
			}
			pthread_mutex_unlock(&mutex);
	}
}


char nextCell(int gridSize, int row, int col)
{

	int numOfNeighbors = 0;
	if(row != 0 && row != gridSize-1)
	{
		if(col != 0 && col != gridSize-1)//Normal tile with 8 neighbors
		{
			if(currGrid[row-1][col-1] == '1') numOfNeighbors++;
			if(currGrid[row-1][col] == '1') numOfNeighbors++;
			if(currGrid[row-1][col+1] == '1') numOfNeighbors++;
			if(currGrid[row][col-1] == '1') numOfNeighbors++;
			if(currGrid[row][col+1] == '1') numOfNeighbors++;
			if(currGrid[row+1][col-1] == '1') numOfNeighbors++;
			if(currGrid[row+1][col] == '1') numOfNeighbors++;
			if(currGrid[row+1][col+1] == '1') numOfNeighbors++;
		}
		else if(col == 0)//tile on left border of grid
		{
			if(currGrid[row-1][col] == '1') numOfNeighbors++;
			if(currGrid[row-1][col+1] == '1') numOfNeighbors++;
			if(currGrid[row][col+1] == '1') numOfNeighbors++;
			if(currGrid[row+1][col] == '1') numOfNeighbors++;
			if(currGrid[row+1][col+1] == '1') numOfNeighbors++;
		}
		else if(col == gridSize-1)//tile on right border of grid
		{
			if(currGrid[row-1][col] == '1') numOfNeighbors++;
			if(currGrid[row-1][col-1] == '1') numOfNeighbors++;
			if(currGrid[row][col-1] == '1') numOfNeighbors++;
			if(currGrid[row+1][col] == '1') numOfNeighbors++;
			if(currGrid[row+1][col-1] == '1') numOfNeighbors++;
		}
	}
	else if(col != 0 && col != gridSize-1)
	{
		if(row == 0)//tile on top border of grid
		{
			if(currGrid[row][col-1] == '1') numOfNeighbors++;
			if(currGrid[row][col+1] == '1') numOfNeighbors++;
			if(currGrid[row+1][col-1] == '1') numOfNeighbors++;
			if(currGrid[row+1][col] == '1') numOfNeighbors++;
			if(currGrid[row+1][col+1] == '1') numOfNeighbors++;
		}
		else if(row == gridSize-1)//tile on bottom border of grid
		{
			if(currGrid[row][col-1] == '1') numOfNeighbors++;
			if(currGrid[row][col+1] == '1') numOfNeighbors++;
			if(currGrid[row-1][col-1] == '1') numOfNeighbors++;
			if(currGrid[row-1][col] == '1') numOfNeighbors++;
			if(currGrid[row-1][col+1] == '1') numOfNeighbors++;
		}
	}
	else//it's a corner
	{
		if(row == 0 && col == 0)//top left corner
		{
			if(currGrid[row][col+1] == '1') numOfNeighbors++;
			if(currGrid[row+1][col] == '1') numOfNeighbors++;
			if(currGrid[row+1][col+1] == '1') numOfNeighbors++;
		}
		else if(row == gridSize-1 && col == gridSize-1)//bottom right corner
		{
			if(currGrid[row][col-1] == '1') numOfNeighbors++;
			if(currGrid[row-1][col] == '1') numOfNeighbors++;
			if(currGrid[row-1][col-1] == '1') numOfNeighbors++;
		}
		else if(row == gridSize-1) //bottom left corner
		{
			if(currGrid[row][col+1] == '1') numOfNeighbors++;
			if(currGrid[row-1][col] == '1') numOfNeighbors++;
			if(currGrid[row-1][col+1] == '1') numOfNeighbors++;
		}
		else //top right corner
		{
			if(currGrid[row][col-1] == '1') numOfNeighbors++;
			if(currGrid[row+1][col] == '1') numOfNeighbors++;
			if(currGrid[row+1][col-1] == '1') numOfNeighbors++;
		}
	}


	if(currGrid[row][col] == '0')
	{
		if(numOfNeighbors == 3)
		{
			return '1';
		}
		else
		{
			return '0';
		}
	}
	else
	{
		if(numOfNeighbors <= 1)
		{
			return '0'; //die due to loneliness
		}
		else if(numOfNeighbors >= 4)
		{
			return '0'; //die due to overcrowding
		}
		else
		{
			return '1'; //Cell survives... for now
		}
	}
}

void readInputFile(struct argsStruct * args, char ** inputArray)
{
	char * inputLine = (char *)malloc(args->gridSize*2*sizeof(char)+1);//needs the +1 because fgets Null terminates.
	int i, j;
	for(i = 0; i < args->gridSize; i++)
	{
		if(fgets(inputLine, args->gridSize*2+1, args->input) == NULL)
		{
			printf("55, %i: Invalid input file, exiting program.\n", i);
			abort();
		}
		for(j = 0; j < args->gridSize*2; j+=2)
		{
				if(inputLine[j] == '0' || inputLine[j] == '1')
				{
					inputArray[i][j/2] = inputLine[j];
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
				fprintf(args->output, "%c ", arrayToPrint[i][j]);
			}
			else
			{
				fprintf(args->output, "%c\n", arrayToPrint[i][j]);
			}
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
