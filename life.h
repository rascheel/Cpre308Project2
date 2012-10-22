#define DEFAULT_GRID_SIZE 8
#define DEFAULT_NUM_THREADS 1
#define DEFAULT_NUM_GENERATIONS 4

struct argsStruct
{
	int gridSize;
	int numThreads;
	int numGenerations;
	FILE* input;
	FILE* output;
};

struct argsStruct processArgs(int argc, char ** argv);

void readInputFile(struct argsStruct * args, char ** inputArray);
void printGrid(struct argsStruct * args, char ** arrayToPrint);
char nextCell(struct argsStruct * args, int i, int j);
