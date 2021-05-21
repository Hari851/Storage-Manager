Group 12 - Assignment 2  

Team Members

Mayank Rajesh Lodha -->Implemented functions -->shutdownBufferPool, unpinPage, getDirtyFlags along with LRU page replacement strategy.
Srushti Ajay Patil -->Implemented functions -->forceFlushPool,forcePage, getFixCount and LRU page replacement strategy. Also added Readme file.
Ajay Kasturi --> Implemented functions --> getNumReadIO, getNumWriteIO and pinPage with Hari.
Hariharan Janakiraman --> functions implemented--> initBufferPool,markDirty,getFrameContents,First In First Out Page 
replacement Strategy and FirstInFirstOutpinPage and LeastRecentlyUsedpp. Also added the Make file.
--------------------------------------------------------------------------------------------------------------------------------
Running the Make
mingw32-make -f Make test_case1
mingw32-make -f Make test_case2
--------------------------------------------------------------------------------------------------------------------------------

Buffer Manager
04/04/2021

Project Modules

C source files: 
1.dberror.c 
2.test_assign2_1.c
3.buffer_mgr_stat.c
4.test_assign2_1.c 

Header files: 
1.storage_mgr.h
2.dberror.h
3.test_helper.h
4.buffer_mgr.h
5.buffer_mgr_stat.h
6.dt.h

--------------------------------------------------------------------------------------------------------------

Steps for Running

1.Create folder and Clone git repository at the location.
2.Navigate to assign2 folder.
3.Use make command to compile FIFO and LRU strategies.
    $make
4.Execute make command to compile Clock page replaceent strategy
    $make
5.To clean the buffer manager execute clean command
    $make clean

--------------------------------------------------------------------------------------------------------------
Method Descriptions

-------------------------------------------------------------------------
* Buffer Pool Functions *
-------------------------------------------------------------------------
1.initBufferPool() : initBufferPoolcreates a new buffer pool withnumPagespage frames  
 using the page replacement strategy.The pool is used to cache
 pages from the page file with namepageFileName.

2.shutdownBufferPool:-
    a. This method frees all the memory allocated for page frames to avoid memory leak.
    b.Calls forceFlushPool() that writes all the dirty pages from buffer pool to the pageFile before freeing up the memory.

3.forceFlushPool:-causes all dirty pages having fix count as 0 from the buffer pool that is to be written to pageFile.

------------------------------------------------------------------------
* Page Management Functions *

------------------------------------------------------------------------
1. markDirty() :- markDirty is a Page Management Function that marks a page as dirty.

2.unpinPage():-

    unpins the page does following.
    a.The pageNum field is used to check which page should be unpined.
    b.Fixcount is decreased once the page is not used anymore.

3.forcePage():-
    a.This function writes the contents of the modified pages back to the page file on disk
    b.resets a page as dirty or not dirty identified by pageNum of the frame in the buffer pool.

4.pinPage() -

This function is used for pinning the page in the buffer pool that holds the page number (it fetches pages from the buffer pool's pagefile). 
Until pinning a page, it tests to see if the buffer pool is empty, and if it is not, we  use a Page Replacement Strategy to insert our new page into the buffer pool.
We have used LRU, FIFO, and Clock strategies to do so.

------------------------------------------------------------------------
Statistics Functions
------------------------------------------------------------------------
1. *getFrameContents() :- *getFrameContents is a Statistics Function that returns an array of pageNumbers (of
   size numPages) where the ith element is the number of the page stored in the ith
   page frame. Return -1 or NO_PAGE for empty page frame

2.getDirtyFlags():-function is used to return array of booleans where each element depicts the dirtybit of the particular page.

3.getFixCounts():-return an array of int (size of numPages) where the ith element acts as the fixcount of the page which is stored in the ith page frame

4.getNumReadIO()- 
The total number of IO reads performed by the buffer pool (the number of pages read from the disk) is returned by this function.
The final_Index variable is used to keep track of this information.

5.getNumWriteIO()-
The total number of IO writes performed by the buffer pool (the number of pages written to disk) is returned by this function.
The wordCount variable is used to keep track of this information. 
When the buffer pool is formed, we set writeCount to 0 and increment it whenever a page frame is written to disk.

------------------------------------------------------------------------
Page Replacement strategies
------------------------------------------------------------------------
1. FirstIn_FirstOut() : FIFO is page replacement algorithm using queue where the first page in is replaced if a
PAGE FAULT occurs else if the page exists it means there is a pageHit indicating page is present in the set.

2.Least_Recently_Used: this function is used to remove the page from the buffer pool that has not been used for long time.
So by using the LRU function we just find the position of the frame with lowest hit and remove them from the buffer pool.