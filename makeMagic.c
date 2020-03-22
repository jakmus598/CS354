///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
//
////////////////////////////////////////////////////////////////////////////////
// Main File:        makeMagic.c
// This File:        makeMagic.c
// Other Files:      N/A
// Semester:         CS 354 Spring 2020
//
// Author:           Jake Musleh
// Email:            jmusleh@wisc.edu
// CS Login:         musleh
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   Fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:	     N/A
//
// Online sources:   GDB debugger - www.gdbtutorial.com/tutorial/how-install-gdb
////////////////////////////////////////////////////////////////////////////////
   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure that represents a magic square
typedef struct {
    int size;     // dimension of the square
    int **msquare; // pointer to heap allocated magic square
} MSquare;


/*
 * Prompts the user for the magic square's size, reads it,
 * checks if it's an odd number >= 3 (if not display the required
 * error message and exit), and returns the valid number.
 */
int getSize() {
    printf("Enter magic square's size (odd integer >=3)\n");
    int *size = malloc(sizeof(int)); //Allocate a pointer to reference the user's inputted size
    
    scanf("%i", size);
    
    //Dereference size to determine if user entered valid input
    if(*size % 2 == 0)
    {
	//Size must be odd
	printf("Size must be odd.\n");
	exit(1);
    }
    if(*size < 3)
    {
	//Size must be at least three
	printf("Size must be >= 3.\n");
	exit(1);
    }

    //Save size in a temp variable (deallocate the memory used for size)
    int tempSize = *size;

    free(size);
    size = NULL;

    return tempSize;
} 
   
/*
 * Makes a magic square of size n using the Siamese algorithm
 * and returns a pointer to the completed MSquare struct.
 *
 * n the number of rosw and columns
 * board - Points to the msquare board in memory
 */
MSquare *generateMSquare(int n) {
    //Dynamically allocate the board
    MSquare *board = malloc(sizeof(MSquare));

    //Set the size of the board to the one given
    board->size = n;

    //Dynamically allocate the board
    board->msquare = malloc(sizeof(int*) * board->size);
    if(!board->msquare)
    {
       printf("Could not allocate memory for board.\n");
       exit(1);
    }
    
    //Dynamically allocate the rest of the board
    *(board->msquare) = malloc(sizeof(int) * board->size * board->size);
    if(!(*(board->msquare)))
    {
       printf("Could not allocate memory for board.\n");
       exit(1);
    }

    for(int i = 0; i < board->size; i++)
    {
	//Assign a group of squares for each row in the board
       *(board->msquare + i) = *(board->msquare) + i*board->size;
    }

    //Initialize all pieces to -1
    for(int i = 0; i < board->size; i++)
    {
	for(int j = 0; j < board->size; j++)
	{
	 *(*(board->msquare + i) + j) = -1;
	}
    }

    int colNum = n/2; //Keeps track of the column number. Initially n/2 because the first piece goes in the middle column (range: 0 to n - 1)

    int rowNum = 0; //Keeps track of the row number. Initially 0 because the first piece goes in the first row (range: 0 to n - 1)
  
  //Place the first piece  
  *(*(board->msquare + rowNum) + colNum) = 1;

    //Place all other pieces
    for(int i = 2; i <= (n*n); i++)
    {
	//Go up and to the right
	rowNum--;
	colNum++;

	int tempRowNum = rowNum + 2; //Used in case the spot up and to the right is filled (piece would be placed directly below current one)

	//Check to make sure tempRow is in bounds
	if(tempRowNum > n - 1)
	{
	   tempRowNum = 0;
	}

	int tempColNum = colNum - 1; //Keeps track of the old column in case up and to the right is filled (piece would be placed in same column)

	//Check if rowNum and colNum are in bounds
	if(rowNum < 0)
	{
	   rowNum = n - 1;
	}

	if(colNum > n - 1)
	{
	   colNum = 0;
	}

	//Place piece in spot up and to the right if it is open
	if(*(*(board->msquare + rowNum) + colNum) == -1)
	{
	  *(*(board->msquare + rowNum) + colNum) = i;
	}

	else
	{
	  //Move piece directly under current one
	  rowNum = tempRowNum;
	  colNum = tempColNum;
	 *(*(board->msquare + rowNum) + colNum) = i;
	}
    }

    return board;
}

   
/*  
 * Opens a new file (or overwrites the existing file)
 * and writes the square in the specified format.
 *
 * msquare the magic square to output
 * filename the name of the output file
 */
void outputMSquare(MSquare *msquare, char *filename) {
     
      //Open the file and check to make sure it opened correctly
      FILE *boardFile = fopen(filename, "w");
      if(boardFile == NULL)
      {
	 printf("Could not open file\n");
	 exit(1);
      }

      //Print the number at each spot
      for(int i = 0; i < msquare->size; i++)
      {
	  for(int j = 0; j < msquare->size; j++)
	  {
	     //Determine whether or not to include a comma
	     if(j == msquare->size - 1)
	     {
		fprintf(boardFile, "%i", *(*(msquare->msquare + i) + j));
	     }
	     else
	     {		     
	     	fprintf(boardFile, "%i,", *(*(msquare->msquare + i) + j));
	     }
	  }

	  fprintf(boardFile, "\n");
      }

      //Close the board or return an error if it cannot be closed
      if(fclose(boardFile) != 0)
      {
	printf("Could not close file\n");
	exit(1);
      }
}

/*
 * Generates a magic square of the user specified size and
 * output the quare to the output filename
 */
int main(int argc, char *argv[]) {
    // Check to make sure there are the correct number of arguments
    if(argc != 2)
    {
       printf("Incorrect number of arguments\n");
       exit(1);
    }
    
    //Get the file name
    char* fileName = *(argv + 1);    

    //Get the size of the square
    int size = getSize();

    // Generate the magic square
    MSquare *board = generateMSquare(size);

    // Output the magic square
    outputMSquare(board, fileName);

    //Deallocate the board
    free(*board->msquare);
    *board->msquare = NULL;

    free(board->msquare);
    board->msquare = NULL;

    free(board);
    board = NULL;

    return 0;
} 


   
