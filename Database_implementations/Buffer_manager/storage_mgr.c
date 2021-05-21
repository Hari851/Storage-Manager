#include <stdio.h>
#include <stdlib.h>
#include "storage_mgr.h"
#include "dberror.h"
#include <string.h>



void initStorageManager(void)
{
}
/*
 * This method creates a new file with the filename passed as paramter/arg.
 * Initial FileSize is 1 and one page filled with null terminated String 
 * in bytes. 
 * @param fileName
 */
RC createPageFile(char *fileName)
{
	char *pt;
    FILE *filePointer = fopen(fileName, "w+");
    pt = (char *) calloc (PAGE_SIZE,sizeof(char));
    if (filePointer == NULL)
    {
        return RC_FILE_NOT_FOUND;
    }
    else
    {
    
        
        fwrite(pt, 1, PAGE_SIZE, filePointer);
        fclose(filePointer);
        free(pt);      
        return RC_OK;
    }
}

/*
 * This method opens a existing pageFile,returns 1 if not found else.
 * the data members/fields of fileHandle are intialized with the
 * information of the file 
 * @param fileName
 * @param fHandle
 */
RC openPageFile(char *fileName, SM_FileHandle *fHandle)
{

    int no = 0;
    int p_File;

    char str;
    FILE *filePointer = fopen(fileName, "r+"); //open the file in read and write mode.
    if (filePointer == NULL)
    {
        return RC_FILE_NOT_FOUND;
    }
    else
    {
        
        int beg;
        fHandle->mgmtInfo = filePointer;
        fHandle->fileName = fileName;

        fseek(fHandle->mgmtInfo,0,SEEK_SET);
        beg = ftell(fHandle->mgmtInfo);
		fHandle->curPagePos = beg;
        fwrite(fileName,PAGE_SIZE,0,filePointer);
        fHandle->totalNumPages = 1;
        fclose(filePointer);
        return RC_OK;
    }
}

/*
 * This method closes PageFile. 
 * @Param fHandle
 */
RC closePageFile(SM_FileHandle *fHandle)
{
    int d;
    if (fHandle != NULL)
    {

        if (fopen(fHandle->fileName, "r") == NULL)
        {
            printf("\n file not opened");
        }

        else
        {
            d = fclose(fHandle->mgmtInfo);
            if (!d)
                return RC_OK;
        }
    }

    return RC_OK;
}

/*
 * This method removes or deletes PageFile. 
 * @Param fileName
 */
RC destroyPageFile(char *fileName)
{
    if (remove(fileName) == 0)
        return RC_OK;
    else
        return RC_FILE_NOT_FOUND;
}

/*
 * This method reads block at pageNum from a file and stores content.
 * in memory pointed by memPage. If File has less than pageNum then
 * return  4
 * @Param pageNum
 * @Param fHandle
 * @Param memPage
 */
RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){
    FILE *filePointer;
    filePointer = fopen(fHandle->fileName,"r");
    fseek(filePointer,(PAGE_SIZE * pageNum),SEEK_SET);
    if(fread(memPage,1,PAGE_SIZE,filePointer) > PAGE_SIZE){
        return RC_READ_NON_EXISTING_PAGE;
    }
    fHandle->curPagePos = ftell(filePointer);
    fclose(filePointer);
    return RC_OK;
}


// Returns current position in a file.
int getBlockPos(SM_FileHandle *fHandle)
{
    return fHandle->curPagePos;
}

// Reads first block from the page file and returns error if block not found
RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{ 
   FILE *filePointer = fopen(fHandle->fileName,"r+");
	
	int pr =1;
	int br;
    fseek(filePointer, (pr+1)*PAGE_SIZE, SEEK_SET);
	
	//fHandle->curPagePos = 0;
	if( fHandle->totalNumPages < pr)
		return RC_READ_NON_EXISTING_PAGE;

		br = fread(memPage,sizeof(char),PAGE_SIZE,filePointer);
		fHandle->curPagePos = ftell(filePointer);
		fclose(filePointer);
		if(!br){
			printf("\n read unsuccessful");
		}
			
		else{
			return RC_OK;
		}
}

//Reads Pervious block from file.
RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	FILE *filePointer = fopen(fHandle->fileName,"r");
    int br;
	int pr;
	fHandle->curPagePos = ftell(filePointer);
        pr=fHandle->curPagePos - 1;
        fseek(filePointer, (pr-2)*PAGE_SIZE, SEEK_SET);
	
	fHandle->curPagePos = fHandle->curPagePos - 1;
	if( fHandle->totalNumPages < pr)
		return RC_READ_NON_EXISTING_PAGE;

		br = fread(memPage,sizeof(char),PAGE_SIZE,filePointer);
		fHandle->curPagePos = ftell(filePointer);
		fclose(filePointer);
		if(!br){
			
			printf("read not succesfful");
		}
			
		else{
			return RC_OK;
		}
}

// Read Current block.
RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    FILE *filePointer = fopen(fHandle->fileName,"r");
    int br;
	int pr ;
        pr= getBlockPos(fHandle) ;
	fHandle->curPagePos = ftell(filePointer);
        fseek(filePointer, (fHandle->curPagePos+1)*PAGE_SIZE, SEEK_SET);
	
	fHandle->curPagePos = ftell(filePointer);
	if( fHandle->totalNumPages < pr)
		return RC_READ_NON_EXISTING_PAGE;

		br = fread(memPage,sizeof(char),PAGE_SIZE,filePointer);
		fHandle->curPagePos = ftell(filePointer);
		fclose(filePointer);
		if(!br)
			printf("read not success");
		else
			return RC_OK;
}

// Read next block from file.
RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    FILE *filePointer = fopen(fHandle->fileName,"r");
    int pr;
    int br;
        fHandle->curPagePos = ftell(filePointer);
	fHandle->curPagePos = fHandle->curPagePos + 1;
        fseek(filePointer, PAGE_SIZE*(fHandle->curPagePos+1), pr);
	
	if( fHandle->totalNumPages < pr)
		return RC_READ_NON_EXISTING_PAGE;

		br = fread(memPage,sizeof(char),PAGE_SIZE,filePointer);
		fHandle->curPagePos = ftell(filePointer);
		fclose(filePointer);
		if(!br)
			printf("read not success");
		else
			return RC_OK;
}

/*
 * This method reads the Last Block from the page into memory.
 * It returns an error if there are no pages.
 */
RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    int pr;
    int br;
    FILE *filePointer = fopen(fHandle->fileName,"r");
        pr =fHandle->totalNumPages - 1;
        fseek(filePointer, PAGE_SIZE*(pr+1), SEEK_SET);
	if( fHandle->totalNumPages < pr)
		return RC_READ_NON_EXISTING_PAGE;
	
		br = fread(memPage,sizeof(char),PAGE_SIZE,filePointer);
		fHandle->curPagePos = ftell(filePointer);
		fclose(filePointer);
	
		if(!br)
			printf("\n not success");
		else
			return RC_OK;
}

// This function writes in the block
RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    FILE *filePointer = fopen(fHandle->fileName, "r+");
	int po_c = (pageNum)*PAGE_SIZE;
	 
	if(pageNum == 0)
	{ 
    int i=0;    
	fseek(filePointer,po_c,SEEK_SET);  
	while(i<PAGE_SIZE)
	{
	fputc(memPage[i],filePointer);
	i++;
	}
	fHandle->curPagePos = ftell(filePointer);
	fclose(filePointer);  
	}
	else
	{ 
    fHandle->curPagePos = po_c;
	fclose(filePointer);
	writeCurrentBlock(fHandle,memPage); 
	} 
	return RC_OK;
}

// This function writes in the current block.
RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	int curPos;
    FILE *fp;
	fp = fopen(fHandle->fileName, "r+");
	curPos = fHandle->curPagePos;
	fseek(fp,curPos,SEEK_SET);
	fwrite(memPage,1,strlen(memPage),fp);
	fHandle->curPagePos = ftell(fp);
	fclose(fp); 
	return RC_OK;
}

/*
 * This method appends a new Empty block into the page.
 * The empty block will contain '\0' bytes.
 */
RC appendEmptyBlock(SM_FileHandle *fHandle)
{
  	int check_ap;
	FILE *filePointer = fopen(fHandle->fileName,"a+");
	fHandle->totalNumPages = fHandle->totalNumPages + 1;
	char *newP = (char *) calloc (PAGE_SIZE,sizeof(char));
	
        fseek(filePointer, PAGE_SIZE*(fHandle->totalNumPages+1), SEEK_END); 
	
        check_ap = fwrite(newP, PAGE_SIZE, sizeof(char), filePointer);
	    fclose(filePointer);
        if(check_ap != 1)
            {
		
		return RC_WRITE_FAILED;
	    }
	else 
		return RC_OK;
	free(newP);
}

/*
 * This method ensures the capacity of pageFile.
 * The pageFile must have numberOfPages,
 * if not then add Page to get required capacity
 */
RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle)
{
    //Checking capacity
    if(numberOfPages > fHandle->totalNumPages){
        int i ;
        i = numberOfPages - fHandle->totalNumPages;
        fHandle->totalNumPages = fHandle->totalNumPages + numberOfPages;
        while (i < fHandle->totalNumPages)
        {
            appendEmptyBlock(fHandle);
            i++;
        }
       
    }
     return RC_OK;
}
