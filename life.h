#define DEFAULT_GRID_SIZE 8
#define DEFAULT_NUM_THREADS 2
#define DEFAULT_NUM_GENERATIONS 1

struct argsStruct
{
	int gridSize;
	int numThreads;
	int numGenerations;
	FILE* input;
	FILE* output;
};

struct threadArgs
{
    int gridSize;
	int numGenerations;
    int startRow;
    int numOfRow;
};

struct argsStruct processArgs(int argc, char ** argv);

void readInputFile(struct argsStruct * args, char ** inputArray);
void printGrid(struct argsStruct * args, char ** arrayToPrint);
char nextCell(int gridSize, int i, int j);
void workerThread(void * arg);
