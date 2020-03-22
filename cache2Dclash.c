///////////////////////////////////////////////////////////////////////////////
// This File: cache2Dclash.c
// Other Files: cache1D.c, cache2Drows.c, cache2Dcols.c
// Semester: CS 354 Spring 2020
//
// Author: Jake Musleh
// Email: jmusleh@wisc.edu
// CS Login: musleh
//
// Other sources of help: N/A
////////////////////////////////////////////////////////////////////////////////

//Declare a 2D array with 128 rows and 8 columns
int arr2D[128][8];

/*
 * Sets the values in the array to be the sum of their row, column, and iteration
 * Returns 0
 * */
int main()
{
   //Repeat the iteration 100 times
   for(int iteration = 0; iteration < 100; iteration++)
   {
      //Iterate over all the rows in the array
      for(int row = 0; row < 128; row += 64)
      {
	 //Iterate over all the columns in the array
	 for(int col = 0; col < 8; col++)
	 {
	    arr2D[row][col] = iteration + row + col;
	 }
      }
   }

   return 0;
}
