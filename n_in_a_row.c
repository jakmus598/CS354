///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
//
////////////////////////////////////////////////////////////////////////////////
// Main File:        n_in_a_row.c
// This File:        n_in_a_row.c
// Other Files:      N/A
// Semester:         CS 354 Spring 2020
//
// Author:           Jake Musleh
// Email:            jmusleh@wisc.edu
// CS Login:         musleh
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          N/A
//
// Online sources:   GDB debugger - http://www.gdbtutorial.com/tutorial/how-inst				    all-gdb
////////////////////////////////////////////////////////////////////////////////
   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
     
char *COMMA = ",";  
     
/* COMPLETED:       
 * Retrieves from the first line of the input file,
 * the size of the board (number of rows and columns).
 * 
 * fp: file pointer for input file
 * size: pointer to size
 */
void get_dimensions(FILE *fp, int *size) {      
    char *line = NULL;
    size_t len = 0;
    if (getline(&line, &len, fp) == -1) {
        printf("Error in reading the file.\n");
        exit(1);
    }

    char *token = NULL;
    token = strtok(line, COMMA);
    *size = atoi(token);
}       
 
/**
 * A function that determines if a given piece is in valid winning position
 *
 * piece: Value of either 1 or 2 to represent X and O
 * board: heap allocated 2D array
 * size: The size of the board
 * 
 * Returns 0 if no valid winning position, 1 otherwise
 */

int validWinningPosition(int piece, int** board, int size)
{
   int consecutiveRows; //# of consecutive pieces in a row (range: 0 to size) 
   int consecutiveCols; //# of consecutive pieces in a column (range: 0 to size)
   int consecutiveDiagonal = 0; //# of consecutive pieces in diagonal (0 to size)
   int rowWins = 0; //Keeps track of the number of wins in a row
   int colWins = 0; //Keeps track of the number of wins in a col
   int diagWin = 0; //Used at end to determine if diagonal win occurred

   for(int i = 0; i < size; i++)
   {
      //Reset for every new row/column
      consecutiveRows = 0;
      consecutiveCols = 0;

      for(int j = 0; j < size; j++)
      {
         if(piece == *(*(board + i) + j))
	 {
	   consecutiveRows++;
	 }

	 if(piece == *(*(board + j) + i))
	 {
	   consecutiveCols++;
     	 }

	 if(j == i)
	 {
	   if(piece == *(*(board + i) + j))
	   {
	      consecutiveDiagonal++;
	   }
	   else
	   {
	      consecutiveDiagonal = 0;
	   }
         }
      }

      //Check to see if there are any wins
      if(consecutiveRows == size)
      {
	 rowWins++;
      }
      if(consecutiveCols == size)
      {
	colWins++;
      }
   }
      //Cannot have parallel wins
      if(rowWins > 1 || colWins > 1)
      {
	 return 0;
      }

      if(consecutiveDiagonal == size)
      {
	diagWin++;
      }

      //Can only have a max of two different winning positions
      if(rowWins + colWins + diagWin > 2)
      {
	return 0;
      }
      return rowWins + colWins + diagWin;
  }
  
	
   
  
/*
 * Returns 1 if and only if the board is in a valid state.
 * Otherwise returns 0.
 * 
 * board: heap allocated 2D board
 * size: number of rows and columns
 */
int n_in_a_row(int **board, int size) {
  
   //Check if there are an even number of rows
   if(size % 2 == 0)
   {
       return 0;
   }

   //Count the number of Xs and Os in the board
   int xCounter = 0; //The number of Xs
   int oCounter = 0; //The number of Os

   for(int i = 0; i < size; i++)
   {
      for(int j = 0; j < size; j++)
      {
	 int piece = *(*(board + i) + j);
	 if(piece == 1)
	 {
	   xCounter++;
	 }

	 if(piece == 2)
	 {
	   oCounter++;
	 }
      }
   }

   //Check if the number of Xs and Os is valid
   if(xCounter > oCounter + 1 || oCounter > xCounter)
   {
      return 0;
   }

   //Check to ensure that X and O have valid winning positions
   if(validWinningPosition(1, board, size) &&
      validWinningPosition(2, board, size))
   {
      //Cannot have two winners, return 0
      return 0;
   }
   
   return 1; //All conditions have passed, board is valid
 }


   
/* 
 * This program prints Valid if the input file contains
 * a game board with either 1 or no winners; and where
 * there is at most 1 more X than O.
 * 
 * argc: CLA count
 * argv: CLA value
 */
int main(int argc, char *argv[]) {              
     
    //There must be 2  command line arguments
    if(argc != 2)
    {
       printf("Incorrect number of command line arguments");
       exit(1);
    }

    //Open the file and check if it opened successfully.
    FILE *fp = fopen(*(argv + 1), "r");
    if (fp == NULL) {
        printf("Can't  open file for reading.\n");
        exit(1);
    }

    //Declare local variables and initialize size
    int *size = malloc(sizeof(int));

    //Check if malloc was successful
    if(!size)
    {
 	printf("Malloc was unsuccessful.\n");
	exit(1);
    }

    int **board;

    // Call get_dimensions to retrieve the board size.
    get_dimensions(fp, size);

   //Use a new variable to represent size's dereferenced value
   int derefSize = *size; 

    // Dynamically allocate a 2D array of dimensions retrieved above.
    board = malloc(sizeof(int*) * derefSize);
    if(!board)
    {
      printf("Malloc was unsuccessful.\n");
      exit(1);
    }

    *board = malloc(sizeof(int) * derefSize*derefSize);
    if(!*board)
    {
      printf("Malloc was unsuccessful.\n");
      exit(1);
    }

    for(int i  = 1; i < derefSize; i++)
    {
      *(board + i) = *board + i*derefSize;
    }
    
    //Read the file line by line.
    //Tokenize each line wrt comma to store the values in your 2D array.
   char *line = NULL;
   size_t len = 0;
   char *token = NULL;
  
   for (int i = 0; i < derefSize; i++) {

        if (getline(&line, &len, fp) == -1) {
            printf("Error while reading the file.\n");
            exit(1);
        }
    	
       token = strtok(line, COMMA);
        for (int j = 0; j < derefSize; j++) {
            //Initialize the 2D array
           *(*(board + i) + j) = atoi(token);
            token = strtok(NULL, COMMA);
        }
    }
    
    //Call the function n_in_a_row and print the appropriate
    //output depending on the function's return value.
    int validBoard = n_in_a_row(board, *size);
    if(validBoard)
    {
	printf("valid\n");
    }
    else
    {
	printf("invalid\n");
    }

    // Free all dynamically allocated memory.
    free(*board);
    *board = NULL;

    free(board);
    board = NULL;

    free(size);
    size = NULL;
    //Close the file.
    if (fclose(fp) != 0) {
        printf("Error while closing the file.\n");
        exit(1);
    } 
    
    return 0;       
}       
