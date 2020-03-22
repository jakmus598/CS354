////////////////////////////////////////////////////////////////////////////////// This File: cache2Dcols.c
// Other Files: cache1D.c, cache2Drows.c, cache2Dclash.c
// Semester: CS 354 Spring 2020
//
// Author: Jake Musleh
// Email: jmusleh@wisc.edu
// CS Login: musleh
//
// Other sources of help: N/A
////////////////////////////////////////////////////////////////////////////////

//Create a 2D array with 3000 rows and 500 columns
int arr2D[3000][500];

/*
 * Set the values of the array to be the sum of their row and column
 * Returns 0
 * */
int main()
{
   //Iterate over all the columns in the array
   for(int col = 0; col < 500; col++)
   {
      //Iterate over all the rows in the array
      for(int row = 0; row < 3000; row++)
      {
	  arr2D[row][col] = row + col;
      }
   }

   return 0;
}
