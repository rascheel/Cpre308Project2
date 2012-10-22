/* Cpr E 308: Handling 2D arrays */
#include <stdio.h>
#include <stdlib.h>


void copy_array (int ** dest, int ** source, const int rows, const int columns)
{
    int i,j;
 
    printf("\nDest after copying : \n");
    for ( i = 0; i < rows; i++ )
    {
        for ( j = 0; j < columns; j++ )
        {
            dest[i][j] = source[i][j];
            printf("%d ", dest[i][j]);

        }
        
        printf ("\n");
    }
 
}

int main(int argc, char * argv[])
{
    int n = 0;
    
    if ( argc != 2 )  {
        
        printf("Please enter the size of the array.\n");
        scanf("%d",&n);
    }
    else  {
        
        n = atoi(argv[1]);
    }
    
    /* printf("Size is %d.\n\n",n); */
    
    int i; /* loop index */
    int j; /* loop index */

    int **source;     /* source array */
    int **dest;       /* destination array */
    int rows;         /* row dimension */
    int columns;      /* column dimension */

    /* Here both dimentions are same here */
    rows = n;
    columns = n;

    /*  allocate storage for an array of pointers */
    source = malloc ( rows * sizeof(int *) );
    dest = malloc ( rows * sizeof(int *) );
    
    if ( (source == NULL) || (dest == NULL) ) {
        
        printf("Error! Not enough memory!");
        exit(1);
    }

    /* for each pointer, allocate storage for an array of ints */
    for (i = 0; i < rows; i++) {
        
        source[i] = malloc ( columns * sizeof(int) );
        dest[i] = malloc ( columns * sizeof(int) );
        
        if ( (source[i] == NULL) || (dest[i] == NULL) ) {
            
            printf("Error! Not enough memory!");
            exit(1);
        }
    }

    /* initialize */
    for (i = 0; i < rows; i++) {
        
        for (j = 0; j < columns; j++) {
    
            source[i][j] = i+j; /* Source initialization */
            dest[i][j] = i*j;   /* Destination initialization */
        }
    }

    /* Display the arrays */
    printf("\nSource:\n");
    for (i = 0; i < rows; i++) {
        
        for (j = 0; j < columns; j++) {
            
            printf("%d ", source[i][j]);
        }
        
        printf ("\n");
    }
    
    printf("\nDest:\n");
    
    for (i = 0; i < rows; i++) {
        
        for (j = 0; j < columns; j++) {
            
            printf("%d ", dest[i][j]);
        }
        
        printf ("\n");
    }
    
    /* Pass the arrays to the function for copying */
    copy_array(dest, source, n, n);
    

    /* for each pointer, free the array of ints */
    for (i = 0; i < rows; i++) {
        
        free(source[i]);
        free(dest[i]);
    }
    
    /* free the array of pointers */
    free(source);
    free(dest);
    
    printf("\n");

    return 0;
}