Group 12 - Programming Assignment 1 
Members :
Srushti Ajay Patil
Mayank Rajesh Lodha
Hariharan Janakiraman
Ajay Kasthuri

Storage Manager
02/27/2021

Files included
_________________________________________________________________________________________________________________________________

Makefile
README.txt
dberror.c
dberror.h
storage_mgr.c
storage_mgr.h
test_assign1_1.c
test_assign1_2.c
test_helper.h



DESCRIPTION
____________________________________________________________________________________________________________________________________

Implementation of the interface of a storage manager that allows reading and writing of blocks to and from a file on disk to memory.


METHODS DESCRIPTION
_____________________________________________________________________________________________________________________________________

createPageFile -
This method creates a new file with the filename passed as paramter/arg.Initial FileSize is 1 and one page filled with null terminated String in bytes.

openPageFile -
This method opens a existing pageFile,returns 1 if not found else,the data members/fields of fileHandle are intialized with the information of the file.

closePageFile -
This method closes PageFile.

destroyPageFile -
This method removes or deletes PageFile.

readBlock -
This method reads block at pageNum from a file and stores content in memory pointed by memPage. If File has less than pageNum then return  4

getBlockPos -
This method returns the current position in a pagefile.

readFirstBlock, readLastBlock -
Used to read the first and last pages of a file by sending the pageNum information. This is done by calling readBlock method.

readNextBlock-
Reads the next block relative to the CurPagePos of the file as it sends the pageNumth information while calling the readBlock method.If there is no next block and user tries to still read it the method returns RC_READ_NON_EXISTING_PAGE error.

readCurrentBlock- 
This method reads current block of a file. This is done by calling readBlock method.

appendEmptyBlock -
The page file is appended by a page which is the empty block. The values of headerPage, totalNumPage are incremented by 1. This last page is filled with zero bytes and the value of CurPagePos is updated to the appended page in the file. It return RC_WRITE_FAILED if the write operation fails.


ensureCapacity -
The size of the file is increased to numberOfPages by calling the appendEmptyBlock method. This adds the remaining empty pages to the existing pages in the file.

writeBlock, writeCurrentBlock -
Write a page to disk by using either the current position or an absolute position. For writeBlock()  if absolute position is beyond range then it returns RC_WRITE_FAILED error and incase of failure of write operations on any page of file it then returns RC_WRITE_FAILED error.


TEST CASES
_______________________________________________________________________________________________________________________________________
Test Cases in test_assign1_1.c and test_assign1_2.c are run along with assertions to check the working of the main code.

