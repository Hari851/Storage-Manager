#include <stdio.h>
#include <stdlib.h>
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include <math.h>

// Initializing
int capacityBuffer = 0;
SM_FileHandle filehandle;

int writes = 0;
int last = 0;
int p_hit = 0;

//one page frame information in buffer pool is stored in this structure with structName framesInfo
typedef struct framesInfo
{
	SM_PageHandle data;
	PageNumber pageNum;
	int least_recent;
	int dirty_Flag;
	int fix_Count;
	int fl;

} FrameInformation;

FrameInformation *fb;
FrameInformation *load;

/*
 * initBufferPoolcreates a new buffer pool withnumPagespage frames  
 * using the page replacement strategy.The pool is used to cache
 * pages from the page file with namepageFileName.
 * @param bm
 * @param pageFileName
 * @param numPages
 * @param strategy
 * @param stratData
 */
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, const int numPages, ReplacementStrategy strategy, void *stratData)
{

	int i;
	fb = malloc(sizeof(FrameInformation) * numPages);
	capacityBuffer = numPages;

	//all page frames should be empty initially. Initializing to 0 or NULL.

	for (i = 0; i < capacityBuffer; i++)
	{
		fb[i].least_recent = 0;
		fb[i].dirty_Flag = 0;
		fb[i].data = NULL;
		fb[i].fix_Count = 0;
		fb[i].pageNum = -1;
	}
	bm->pageFile = (char *)pageFileName;
	bm->numPages = numPages;
	bm->strategy = strategy;
	//store information of all pages in buffer pool to mgmtData for bookkeeping information

	bm->mgmtData = fb;

	writes = 0;

	return 0;
}

//The fuction shutdowns the buffer pool. Frees memory allocation to avoid memory leak and dirty pages are returned before freeing it.
RC shutdownBufferPool(BM_BufferPool *const bm)
{
	int tf = 0;
	int j;

	//Store data in frame.

	fb = (FrameInformation *)bm->mgmtData;

	//Flush all the dirty pages before shutdown and set tf to 1.

	if (forceFlushPool(bm) == RC_OK)
		tf = 1;

	for (j = 0; j < capacityBuffer; j++)
	{
		//Check if disk has been modified.
		if (fb[j].fix_Count != 0)
			return RC_BUF_HAS_DIRTY;
	}

	free(fb);
	bm->mgmtData = NULL;

	if (tf)
		return 0;
}

// The function to write all the dirty pages those have fixCount as 0 to disk
RC forceFlushPool(BM_BufferPool *const bm)
{

	int j;

	for (j = 0; j < capacityBuffer; j++)
	{
		//To check if any page is modified and if pageframe is used.
		if ((0 == fb[j].fix_Count) && (1 == fb[j].dirty_Flag))
		{
			openPageFile(bm->pageFile, &filehandle);
			writeBlock(fb[j].pageNum, &filehandle, fb[j].data);
			fb[j].dirty_Flag = 0;
			writes++;
		}
	}

	return 0;
}

/*
 * markDirty is a Page Management Function that marks a page as dirty.
 * @param bm
 * @param page 
 */
RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page)
{
	int i = 0;
	;
	int tf = 0;

	fb = (FrameInformation *)bm->mgmtData;

	while (i < capacityBuffer)
	{
		if (fb[i].pageNum == page->pageNum)
		{
			//Assign the framePage info to frames variable
			fb[i].dirty_Flag = 1;
			tf = 1;
			return 0;
		}
		i++;
	}
	if (!tf)
		return 860;
}

// The function to remove the page from the memory of the buffer manager.
RC unpinPage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
	int j;
	int tf = 0;

	fb = (FrameInformation *)bm->mgmtData;

	for (j = 0; j < capacityBuffer; j++)
	{
		if (fb[j].pageNum != page->pageNum)
		{
			//printf("\n");
		}
		else
		{
			// If the visited page is the page to be unpined then decrease the fix_count value and exit

			fb[j].fix_Count--;
			tf = 1;
			break;
		}
	}
	if (tf)
		return 0;
}

// This function writes the contents of the modified pages back to the page file on disk
RC forcePage(BM_BufferPool *const bm, BM_PageHandle *const page)
{

	int j;
	if (openPageFile(bm->pageFile, &filehandle) != RC_OK)
		return RC_FILE_NOT_FOUND;

	for (j = 0; j < capacityBuffer; j++)
	{
		if (fb[j].pageNum != page->pageNum)
		{
			//printf("no pagenum");
		}
		else
		{
			// If the page is to be written to disk, then right the page to the disk
			openPageFile(bm->pageFile, &filehandle);
			if (0 != (writeBlock(fb[j].pageNum, &filehandle, fb[j].data)))
				return RC_NOT_OK;
			fb[j].dirty_Flag = 0;
			writes++;
		}
	}

	return 0;
}

//Implementing FIFO Function
void FirstinFirstout(BM_BufferPool *const bm, FrameInformation *buff)
{
	int k;
	int j = last % capacityBuffer;

	fb = (FrameInformation *)bm->mgmtData;

	for (k = 0; k < capacityBuffer; k++)
	{
		if (fb[j].fix_Count != 0)
		{
			j++;
			if (j % capacityBuffer == 0)
				j = 0;
		}
		else
		{
			if (fb[j].dirty_Flag != 1)
			{
				//printf("not modified");
			}
			else
			{
				openPageFile(bm->pageFile, &filehandle);
				writeBlock(fb[j].pageNum, &filehandle, fb[j].data);
				writes++;
			}
			fb[j].dirty_Flag = buff->dirty_Flag;
			fb[j].pageNum = buff->pageNum;
			fb[j].data = buff->data;
			fb[j].fix_Count = buff->fix_Count;
			break;
		}
	}
}

RC FirstinFirstoutpinpage(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
{

	fb = (FrameInformation *)bm->mgmtData;
	if (fb[0].pageNum != -1)
	{
		int i;
		int sc = 0;
		for (i = 0; i < capacityBuffer; i++)
		{
			if (fb[i].pageNum == -1)
			{
				if (0 != openPageFile(bm->pageFile, &filehandle))
				{
					return RC_NOT_OK;
				}
				fb[i].data = (SM_PageHandle)malloc(PAGE_SIZE);
				if (0 != readBlock(pageNum, &filehandle, fb[i].data))
				{
					return RC_ERR;
				}

				fb[i].least_recent = 0;
				fb[i].fix_Count = 1;
				page->data = fb[i].data;

				sc = 1;
				p_hit++;

				last++;

				page->pageNum = pageNum;
				fb[i].pageNum = pageNum;

				break;
			}
			else
			{
				if (fb[i].pageNum != pageNum)
				{
					//PRINT
				}
				else
				{
					fb[i].fix_Count++;
					page->data = fb[i].data;

					sc = 1;
					p_hit++;

					page->pageNum = pageNum;

					break;
				}
			}
		} //endFor
		if (sc != 0)
		{
			//print
		}
		else
		{

			load = (FrameInformation *)malloc(sizeof(FrameInformation));
			if (0 != openPageFile(bm->pageFile, &filehandle))
			{
				return RC_NOT_OK;
			}
			load->data = (SM_PageHandle)malloc(PAGE_SIZE);
			if (0 != readBlock(pageNum, &filehandle, load->data))
			{
				return RC_ERR;
			}
			load->least_recent = 0;
			load->fix_Count = 1;
			load->dirty_Flag = 0;

			page->pageNum = pageNum;
			page->data = load->data;

			load->pageNum = pageNum;
			last++;
			p_hit++;

			FirstinFirstout(bm, load);
		}

		return 0;
	}

	else
	{

		fb[0].data = (SM_PageHandle)malloc(PAGE_SIZE);
		if (0 != openPageFile(bm->pageFile, &filehandle))
		{
			return RC_NOT_OK;
		}

		if (0 != ensureCapacity(pageNum, &filehandle))
		{
			return 880;
		}
		if (0 != readBlock(pageNum, &filehandle, fb[0].data))
		{
			return 500;
		}
		fb[0].least_recent = 0;
		page->data = fb[0].data;

		p_hit = 0;
		last = 0;
		fb[0].fix_Count++;

		page->pageNum = pageNum;

		fb[0].pageNum = pageNum;

		return 0;
	}
}

void LeastRecentlyUsed(BM_BufferPool *const bm, FrameInformation *buff)
{

	int j;
	int k;
	int i;
	fb = (FrameInformation *)bm->mgmtData;

	for (i = 0; i < capacityBuffer; i++)
	{
		if (fb[i].fix_Count != 0)
		{
			//print
		}
		else
		{

			j = i;
			k = fb[i].fl;
			break;
		}
	}
	for (i = j + 1; i < capacityBuffer; i++)
	{
		if (fb[i].fl > k)
		{
			//print
		}
		else
		{
			j = i;
			k = fb[i].fl;
		}
	}

	if (fb[j].dirty_Flag != 1)
	{
		//print
	}
	else
	{
		openPageFile(bm->pageFile, &filehandle);
		writeBlock(fb[j].pageNum, &filehandle, fb[j].data);
		writes++;
	}
	fb[j].fix_Count = buff->fix_Count;
	fb[j].dirty_Flag = buff->dirty_Flag;
	fb[j].data = buff->data;
	fb[j].fl = buff->fl;

	fb[j].pageNum = buff->pageNum;
}

RC LeastRecentlyUsedpp(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
{

	fb = (FrameInformation *)bm->mgmtData;
	if (fb[0].pageNum != -1)
	{
		int i;
		int sc = 0;
		for (i = 0; i < capacityBuffer; i++)
		{
			if (fb[i].pageNum == -1)
			{
				fb[i].data = (SM_PageHandle)malloc(PAGE_SIZE);
				if (0 != openPageFile(bm->pageFile, &filehandle))
				{
					return RC_NOT_OK;
				}

				if (0 != readBlock(pageNum, &filehandle, fb[i].data))
				{
					return RC_ERR;
				}

				fb[i].fix_Count = 1;

				page->pageNum = pageNum;

				fb[i].least_recent = 0;
				last++;
				sc = 1;
				p_hit++;
				fb[i].fl = p_hit;

				page->data = fb[i].data;

				fb[i].pageNum = pageNum;

				break;
			}
			else
			{
				if (fb[i].pageNum != pageNum)
				{
					//print
				}
				else
				{
					{
						fb[i].fix_Count++;
						sc = 1;
						p_hit++;
						fb[i].fl = p_hit;

						page->data = fb[i].data;

						page->pageNum = pageNum;

						break;
					}
				}
			}
		} //endfor
		if (sc == 0)
		{
			load = (FrameInformation *)malloc(sizeof(FrameInformation));
			if (0 != openPageFile(bm->pageFile, &filehandle))
			{
				return RC_NOT_OK;
			}
			load->data = (SM_PageHandle)malloc(PAGE_SIZE);
			if (0 != readBlock(pageNum, &filehandle, load->data))
			{
				return RC_ERR;
			}

			load->least_recent = 0;
			last++;
			p_hit++;

			load->dirty_Flag = 0;
			load->fix_Count = 1;

			page->data = load->data;

			load->fl = p_hit;

			page->pageNum = pageNum;

			load->pageNum = pageNum;

			LeastRecentlyUsed(bm, load);
		}
		return 0;
	}
	else
	{
		fb[0].data = (SM_PageHandle)malloc(PAGE_SIZE);
		if (0 != openPageFile(bm->pageFile, &filehandle))
		{
			return RC_NOT_OK;
		}

		if (0 != ensureCapacity(pageNum, &filehandle))
		{
			return RC_SIZE_NOT_OK;
		}
		if (0 != readBlock(pageNum, &filehandle, fb[0].data))
		{
			return RC_ERR;
		}

		last = 0;
		fb[0].least_recent = 0;

		fb[0].fix_Count++;
		page->data = fb[0].data;

		page->pageNum = pageNum;
		fb[0].pageNum = pageNum;

		fb[0].fl = p_hit;

		p_hit = 0;

		return 0;
	}
}

RC pinPage(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
{

	if (RS_FIFO == bm->strategy)
	{
		return FirstinFirstoutpinpage(bm, page, pageNum);
	}
	else if (RS_LRU == bm->strategy)
	{
		return LeastRecentlyUsedpp(bm, page, pageNum);
	}
}

/*
 * *getFrameContents is a Statistics Function that returns an array of pageNumbers (of
 * size numPages) where the ith element is the number of the page stored in the ith
 * page frame. Return -1 or NO_PAGE for empty page frame
 * @param bm
 */
PageNumber *getFrameContents(BM_BufferPool *const bm)
{
	int j;
	//allocate memory to contents_frame variable of type PageNumber.
	PageNumber *return_page_number = malloc(sizeof(PageNumber) * capacityBuffer);
	//Assign the framePage info to frames variable.
	fb = (FrameInformation *)bm->mgmtData;

	//pages in buffer pool are looped and pageNumber is set to framecontents value.
	for (j = 0; j < capacityBuffer; j++)
	{
		return_page_number[j] = fb[j].pageNum;
	}
	return return_page_number;
}

// Function to return  array of bools where each element depicts the dirtyBit of the particular page.
bool *getDirtyFlags(BM_BufferPool *const bm)
{
	int j = 0;
	bool *fd = malloc(sizeof(bool) * capacityBuffer);

	fb = (FrameInformation *)bm->mgmtData;

	while (j < capacityBuffer)
	{

		fd[j] = (fb[j].dirty_Flag != 1) ? false : true;
		j++;
	}
	return fd;
}

// Function to return an array of int (size of numPages) where the ith element acts as the fixcount of the page which is stored in the ith page frame.
int *getFixCounts(BM_BufferPool *const bm)
{
	int j = 0;
	int *cf = malloc(sizeof(int) * capacityBuffer);

	fb = (FrameInformation *)bm->mgmtData;

	while (j < capacityBuffer)
	{
		cf[j] = fb[j].fix_Count;
		j++;
	}
	return cf;
}

// This function returns the number of pages that have been read from disk since a buffer pool has been initialized.
int getNumReadIO(BM_BufferPool *const bm)
{
	return last + 1;
}

// This function returns the number of pages written to the page file since the buffer pool has been initialized.
int getNumWriteIO(BM_BufferPool *const bm)
{
	return writes;
}