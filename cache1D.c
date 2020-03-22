////////////////////////////////////////////////////////////////////////////////
// This File: cache1D.c
// Other Files: cache2Drows.c, cache2Dcols.c, cache2Dclash.c 
// Semester: CS 354 Spring 2020
//
// Author: Jake Musleh
// Email: jmusleh@wisc.edu
// CS Login: musleh
//
// Other sources of help: N/A
////////////////////////////////////////////////////////////////////////////////


//Declare a global array of integers of size 100000
int array[100000];

/*
 * Set the values in the array to their indices
 * Returns 0
 * */
int main()
{
   //Iterate over the entire array
   for(int i = 0; i < 100000; i++)
   {
      array[i] = i;
   }
  
  return 0;
} 

