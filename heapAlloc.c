///////////////////////////////////////////////////////////////////////////////
//Main File:	N/A
//This File:	heapAlloc.c
//Other Files:	All files in /tests
//Semester:	CS 354 Spring 2020
//
//Author:	Jake Musleh
//Email:	jmusleh@wisc.edu
//CS Login:	musleh
//
///////////////////////OTHER SOURCES OF HELP////////////////////////////////////
//
//Persons:	N/A
//
//Online sources: GDB debugger - gdbtutorial.com/tutorial/how-install-gdb
//
//
// Copyright 2019-2020 Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
//
///////////////////////////////////////////////////////////////////////////////
 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include "heapAlloc.h"
 
/*
 * This structure serves as the header for each allocated and free block.
 * It also serves as the footer for each free block but only containing size.
 */
typedef struct blockHeader {           
    int size_status;
    /*
    * Size of the block is always a multiple of 8.
    * Size is stored in all block headers and free block footers.
    *
    * Status is stored only in headers using the two least significant bits.
    *   Bit0 => least significant bit, last bit
    *   Bit0 == 0 => free block
    *   Bit0 == 1 => allocated block
    *
    *   Bit1 => second last bit 
    *   Bit1 == 0 => previous block is free
    *   Bit1 == 1 => previous block is allocated
    * 
    * End Mark: 
    *  The end of the available memory is indicated using a size_status of 1.
    * 
    * Examples:
    * 
    * 1. Allocated block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 27
    *      If the previous block is free, size_status should be 25
    * 
    * 2. Free block of size 24 bytes:
    *    Header:
    *      If the previous block is allocated, size_status should be 26
    *      If the previous block is free, size_status should be 24
    *    Footer:
    *      size_status should be 24
    */
} blockHeader;         

/* Global variable - DO NOT CHANGE. It should always point to the first block,
 * i.e., the block at the lowest address.
 */
blockHeader *heapStart = NULL;     

/* Size of heap allocation padded to round to nearest page size.
 */
int allocsize;


//Keep track of the last payload allocated in order to implement next-fit policy
void* lastBlock = NULL;  
 
/* 
 * Function for allocating 'size' bytes of heap memory.
 * Argument size: requested size for the payload
 * Returns address of allocated block on success.
 * Returns NULL on failure.
 * This function should:
 * - Check size - Return NULL if not positive or if larger than heap space.
 * - Determine block size rounding up to a multiple of 8 and possibly adding padding as a result.
 * - Use NEXT-FIT PLACEMENT POLICY to chose a free block
 * - Use SPLITTING to divide the chosen free block into two if it is too large.
 * - Update header(s) and footer as needed.
 * Tips: Be careful with pointer arithmetic and scale factors.
 */
void* allocHeap(int size) {

   //Check size to determine if size is valid
   if(size <= 0)
   {
     return NULL;
   }


   //Set the min block size needed to store the data (range: 4 to 4088 --> anything bigger will cause the method to return NULL shortly)
   //The 4 refers to the size of the header
   int minBlockSize = 4 + size;

   //The amount of padding that the block should have (range: 0 to 7)
   int padding;
   
   //Check to see if minBlockSize is already a multiple of 8
   if(minBlockSize % 8 == 0)
   {
      padding = 0;
   }

   else
   {
      padding = 8 - (minBlockSize % 8);
   }

   //Appropriately adjust minBlockSize to account for padding   
   minBlockSize = minBlockSize + padding;

   //Ensure that the block can fit in the allocated memory
   if(minBlockSize > allocsize)
   {
	return NULL;
   }
   //ptr variable to store the payload address of the block to allocate (range: heapStart to heapStart + allocsize/4 - 1)
   void *payload = NULL;

   //The size of the block to be used before it is split (range: 8 to 4088)
   int blockSize;

   //Keeps track of whether the last block was allocated (either 0 or 2)
   int prevBlockStatus; 

   //Check to determine if a block has been allocated yet
   if(lastBlock == NULL)
   {
      //Set payload to be heapStart
      payload = heapStart + 1;

      //Set prevBlockStatus = 2 (always the case when first block in heap)
      prevBlockStatus = 2;
      heapStart->size_status = minBlockSize + 1 + prevBlockStatus;
      blockSize = allocsize; //The only block we have is the entire block of memory
      
   }

   else
   {
      //Get the header and size of the previous block
      blockHeader *lastHeader = lastBlock - 4; //4 because lastBlock is void*
      blockSize = lastHeader->size_status;

      //Check to determine if block has been already freed
      if(blockSize % 8 == 0)
      {
	  prevBlockStatus = 0;
	  payload = lastBlock;
      }

      else if(blockSize % 8 == 2)
      {
	  blockSize = blockSize - 2;
	  prevBlockStatus = 2;
	  payload = lastBlock;
      }

      else
      {
	  //Go through all the blocks, starting at the block after lastHeader
	  blockSize = blockSize - blockSize % 8; //Get size of block itself
	  lastHeader = lastHeader + blockSize/4;
	  prevBlockStatus = 2; //The previous block is allocated

	  //Look through entire heap (if necessary) until reaching starting point
	  while(lastHeader != lastBlock - 4)
	  {
	      blockSize = lastHeader->size_status;
	      
	      //Check if block is freed
	      if(blockSize % 8 == 0 || blockSize % 8 == 2)
	      {
		  blockSize = blockSize - blockSize % 8;
		  if(blockSize >= minBlockSize)
		  {
		     payload = lastHeader + 1;
		     break;
		  }
		  prevBlockStatus = 0; //The previous block is free but is not big enough
	      }
	      else
	      {
		  prevBlockStatus = 2;
	      }

	      lastHeader += (blockSize - blockSize % 8)/4;

	      //Check to see if we have arrived at the end of the heap
	      if(lastHeader->size_status == 1)
	      {
		  lastHeader = heapStart;
	      }
	  }

	  //There was not a big enough space to allocate
	  if(payload == NULL)
	  {
	     return NULL;
	  }
      }
   }
      //Get the header of the current block to allocate
      blockHeader *currHeader = payload - 4;

      //Determine if the block needs to be split
      if(blockSize - minBlockSize >= 8)
      {
	   blockHeader *nextHeader = currHeader + minBlockSize/4;
	   nextHeader->size_status = blockSize - minBlockSize + 2;
	   currHeader->size_status = minBlockSize + 1 + prevBlockStatus;
	   
      }
      
      else
      {
	   currHeader->size_status = blockSize + 1 + prevBlockStatus;
      }
  
   //Update lastBlock
   lastBlock = payload;
   return payload;
}
 
 
/* 
 * Function for freeing up a previously allocated block.
 * Argument ptr: address of the block to be freed up.
 * Returns 0 on success.
 * Returns -1 on failure.
 * This function should:
 * - Return -1 if ptr is NULL.
 * - Return -1 if ptr is not a multiple of 8.
 * - Return -1 if ptr is outside of the heap space.
 * - Return -1 if ptr block is already freed.
 * - USE IMMEDIATE COALESCING if one or both of the adjacent neighbors are free.
 * - Update header(s) and footer as needed.
 */                    
int freeHeap(void *ptr) {
   
    if(ptr == NULL)
    {
	//Invalid argument
	return -1;
    }

    if((int)ptr % 8 != 0)
    {
	//ptr must represent the start of a payload
	return -1;
    }

    //Check if ptr is outside of heap space
    if((int)ptr > (int)(heapStart + allocsize/4 - 1))
    {
	return -1;
    }

    //Get the header of the ptr (where headerToFree represents the address of its header)
    blockHeader *headerToFree = ptr - 4;

    //Check if block has already been freed
    if(headerToFree->size_status % 8 == 2 || headerToFree->size_status % 8 == 0)
    {
	return -1;
    }

    //Change the size status of headerToFree to account for its deallocation
    headerToFree->size_status = headerToFree->size_status - 1;

    //Denote blockSize to refer to headerToFree->size_status (makes for more readable code)
    //Range: 8 to 4091
    int blockSize = headerToFree->size_status;
    
    int prevBlockSize; //The size of the previous block (0 if block is allocated), range: 8 to allocsize - blockSize - blockSize % 8
    int nextBlockSize; //The size of the next block (0 if the block is allocated), range: same as prevBlockSize 

    //Use footerAddr as an address for the footer
    blockHeader *footerAddr = NULL;
    
    //Use newHeader for the header of the coalesced blocks
    blockHeader *newHeader = NULL;

    //Check if the adjacent blocks are open
    if(blockSize % 8 == 2)
    {
	//Preceeding block is not open
	prevBlockSize = 0;
	newHeader = headerToFree;
    }

    else if(blockSize % 8 == 0)
    {
	//Go back two bytes to address of previous block
	//prevFoot is the address of the previous footer
	blockHeader *prevFoot = ptr - 8;
	prevBlockSize = prevFoot->size_status;
	newHeader = headerToFree - prevBlockSize/4;
    }
    else
    {
	//Error has occurred
	return - 1;
    }

    //Check if succeeding block is open
    blockSize = blockSize - (blockSize % 8);
    
    //nextHeader used to refer to the header of the next block
    blockHeader *nextHeader = ptr + blockSize - 4; //Subtract 4 to arrive at header

    //Check if next block is open
    if(nextHeader->size_status % 8 != 2)
    {
	 nextBlockSize = 0;
	 footerAddr = nextHeader - 1; //Set it to be the start of the last byte end of the previous payload
    }

    else
    {
	nextBlockSize = nextHeader->size_status - 2;
	footerAddr = nextHeader + nextBlockSize/4 - 1;
    }

    //Set the value of the footer
    footerAddr->size_status = prevBlockSize + blockSize + nextBlockSize;

    //Change the p-bit for the block following footerAddr
    //nextAllocatedBlock refers to the nextAllocatedBlock following freed mem
    blockHeader *nextAllocatedBlock = footerAddr + 1;
    if(nextAllocatedBlock->size_status % 8 == 3)
    {    
	nextAllocatedBlock->size_status = nextAllocatedBlock->size_status - 2;
    }

    newHeader->size_status = footerAddr->size_status + 2; //previous block should be allocated

    return 0;

}

 
/*
 * Function used to initialize the memory allocator.
 * Intended to be called ONLY once by a program.
 * Argument sizeOfRegion: the size of the heap space to be allocated.
 * Returns 0 on success.
 * Returns -1 on failure.
 */                    
int initHeap(int sizeOfRegion) {    
 
    static int allocated_once = 0; //prevent multiple initHeap calls
 
    int pagesize;  // page size
    int padsize;   // size of padding when heap size not a multiple of page size
    void* mmap_ptr; // pointer to memory mapped area
    int fd;

    blockHeader* endMark;
  
    if (0 != allocated_once) {
        fprintf(stderr, 
        "Error:mem.c: InitHeap has allocated space during a previous call\n");
        return -1;
    }
    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    allocsize = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    mmap_ptr = mmap(NULL, allocsize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (MAP_FAILED == mmap_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }
  
    allocated_once = 1;

    // for double word alignment and end mark
    allocsize -= 8;

    // Initially there is only one big free block in the heap.
    // Skip first 4 bytes for double word alignment requirement.
    heapStart = (blockHeader*) mmap_ptr + 1;
  

    // Set the end mark
    endMark = (blockHeader*)((void*)heapStart + allocsize);
    endMark->size_status = 1;

    // Set size in header
    heapStart->size_status = allocsize;

    // Set p-bit as allocated in header
    // note a-bit left at 0 for free
    heapStart->size_status += 2;

    // Set the footer
    blockHeader *footer = (blockHeader*) ((void*)heapStart + allocsize - 4);
    footer->size_status = allocsize;
  
    return 0;
} 
                  
/* 
 * Function to be used for DEBUGGING to help you visualize your heap structure.
 * Prints out a list of all the blocks including this information:
 * No.      : serial number of the block 
 * Status   : free/used (allocated)
 * Prev     : status of previous block free/used (allocated)
 * t_Begin  : address of the first byte in the block (where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block as stored in the block header
 */                     
void dumpMem() {     
 
    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end   = NULL;
    int t_size;

    blockHeader *current = heapStart;
    counter = 1;

    int used_size = 0;
    int free_size = 0;
    int is_used   = -1;

    fprintf(stdout, "************************************Block list***\
                    ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
                    --------------------------------\n");
  
    while (current->size_status != 1) {
        t_begin = (char*)current;
	//printf("current: %p\n", current);
        t_size = current->size_status;
	//printf("current->size)status: %i\n", t_size);
    
        if (t_size & 1) {
            // LSB = 1 => used block
            strcpy(status, "used");
            is_used = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_used = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "used");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }

        if (is_used) 
            used_size += t_size;
        else 
   	   free_size += t_size;

	printf("t_size: %i\n", t_size);

        t_end = t_begin + t_size - 1;
	//printf("t_end: %p\n", t_end);
    
        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status, 
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
    
        current = (blockHeader*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, "---------------------------------------------------\
                    ------------------------------\n");
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fprintf(stdout, "Total used size = %d\n", used_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", used_size + free_size);
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fflush(stdout);

    return;  
} 
