#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "btree_mgr.h"
#include "tables.h"
#include "buffer_mgr.h"
#include "storage_mgr.h"

typedef struct DataBTree
{
    struct InfoBTree **ext;
    int cp_nd;
    int ch_nd;
    int val_nd;
    int in_nd;
    int en_nd;
    int lf;
    int inc_nd;
    int et;
    int ex;
    int pa_no;
    int sl_no;
    int *tp;
    int vr_nd;
    int ex_no;
    RID *di;
} InfoBTree;

int oc = 0;
int zero = 0;
int one = 1;
int six = 6;

typedef struct Fram
{
    BM_BufferPool *pbuff;
    BM_PageHandle *pHandle;
} Fr_b;

InfoBTree treeb;
InfoBTree *root;
SM_FileHandle h_f;
SM_PageHandle h_p;
InfoBTree *redundant;
Fr_b *frame;

/*
 * initIndexManager is used to initialize the index manager
 * return 0 on success.
 * @param mgmtData
 */
RC initIndexManager(void *mgmtData)
{
    //Allocate memory using malloc considering size of struct InfoBtree
    root = (InfoBTree *)malloc(sizeof(InfoBTree));
    h_p = malloc(sizeof(int) * 4096);
    if (!root)
    {
        return RC_FILE_NOT_FOUND;
    }
    return RC_OK;
}

RC shutdownIndexManager()
{
    free(root);
    free(h_p);
    return RC_OK;
}

RC createBtree(char *idxId, DataType keyType, int num)
{
    if (root == NULL)
    {
        return RC_ERR;
    }
    else
    {
        (*root).tp = malloc(sizeof(int) + sizeof(char));
        (*root).di = malloc(sizeof(int) + sizeof(char));
        (*root).ext = malloc(sizeof(int) + sizeof(int));
        treeb.val_nd = 0;
        while (treeb.val_nd < num + sizeof(char))
        {
            (*root).ext[treeb.val_nd] = '\0';
            treeb.cp_nd = sizeof(bool);
            treeb.val_nd = treeb.val_nd + sizeof(bool);
        }
        if (keyType == DT_INT)
        {
            if (createPageFile(idxId) == 0)
            {
                return RC_OK;
            }
            else
            {
                return RC_NOT_OK;
            }
            if (openPageFile(idxId, &h_f) == 0)
            {
                return RC_OK;
            }
            else
            {
                return RC_NOT_OK;
            }
        }
        else
        {
            return RC_ERR;
        }
        oc++;
    }
    return RC_OK;
}

RC openBtree(BTreeHandle **tree, char *idxId)
{
    if (oc == 1)
    {
        if (frame == NULL)
        {
            printf("Frame is not null!");
        }
        else
        {

            (*frame).pHandle = ((BM_PageHandle *)malloc(sizeof(BM_PageHandle)));

            (*frame).pbuff = ((BM_BufferPool *)malloc(sizeof(BM_BufferPool)));

            int res = initBufferPool((*frame).pbuff, idxId, 3, RS_LRU, NULL);

            if (res == 0)
            {
                printf("ok");
            }
            else
            {
                return RC_NO_BUFFERED_DATA;
            }
        }
        pinPage((*frame).pbuff, (*frame).pHandle, 1);

        unpinPage((*frame).pbuff, (*frame).pHandle);
    }
    return 0;
}

/* When closing a b-tree (closeBtree), the index manager 
*should ensure that all new or modified pages of the index are flushed back to disk (use the buffer manager function for that).
* @param tree
*/
RC closeBtree(BTreeHandle *tree)
{
    if (NULL != tree)
    {
        printf("tree not null");
    }
    else
    {
        return RC_OK;
    }
    free(root);
    free(tree);
    
    closePageFile(&h_f);

    if (NULL != tree)
    {
        printf("tree not null");
    }
    else
        return RC_OK;
}

RC deleteBtree(char *idxId)
{
    remove(idxId);
    return RC_OK;
}

RC getNumEntries(BTreeHandle *tree, int *result)
{
    treeb.en_nd = zero;
    treeb.val_nd = zero;
    treeb.cp_nd = sizeof(bool);
    while (treeb.val_nd < treeb.cp_nd + sizeof(int))
    {
        treeb.en_nd = treeb.en_nd + 1;
        treeb.val_nd++;
    }
    *result = treeb.en_nd;
    return RC_OK;
}

/*
*check the datatype of the keys in the btree whether they are int, float, bool or string and store them in the datatype parameter
*@param tree
*@param result
*/
RC getKeyType(BTreeHandle *tree, DataType *result)
{
    if (NULL != tree)
    {
        printf("tree not null");
    }
    else
        return RC_TREE_EMPTY;
    if (NULL != tree)
    {
        printf("tree not null");
    }
    else
        return RC_NOT_OK;
    return RC_OK;
}

RC findKey(BTreeHandle *tree, Value *key, RID *result)
{
    treeb.lf = 0;
    redundant = (InfoBTree *)malloc(sizeof(InfoBTree));
    redundant = root;
    while (redundant != NULL)
    {
        treeb.val_nd = 0;
        while (treeb.val_nd < sizeof(int))
        {

            if ((redundant)->tp[treeb.val_nd] == (key)->v.intV)
            {
                int slotNum = (redundant)->di[treeb.val_nd].slot;
                int pageNum = (redundant)->di[treeb.val_nd].page;
                (result)->slot = slotNum;
                (result)->page = pageNum;
                treeb.val_nd = one;
                treeb.lf = one;
                break;
            }
            treeb.val_nd++;
        }
        if (treeb.lf == 1)
            break;

        redundant = (*redundant).ext[sizeof(bool)];
    }
    if(treeb.lf == 1)
	    return RC_OK;
	else
	    return RC_IM_KEY_NOT_FOUND;
}

RC getNumNodes(BTreeHandle *tree, int *result)
{
    treeb.en_nd = zero;
    treeb.val_nd = zero;
    while (treeb.val_nd < treeb.cp_nd + sizeof(bool))
    {
        treeb.in_nd = treeb.in_nd + 1;
        treeb.val_nd++;
    }
    *result = treeb.in_nd;
    return RC_OK;
}

RC insertKey (BTreeHandle *tree, Value *key, RID rid)
	{
	    int st =sizeof(InfoBTree);
        int ts;
	    treeb.ch_nd = zero;

	    redundant = (InfoBTree*)malloc(sizeof(InfoBTree));
	    redundant = root;
        while (checkIfNull(redundant))
	    {
	        InfoBTree *info = (InfoBTree*)malloc(sizeof(InfoBTree));    
	        (info)->tp = malloc(sizeof(int) + sizeof(char));             
	        (info)->di = malloc(sizeof(int) + sizeof(char));               
	        (info)->ext = malloc(sizeof(int) + (sizeof(int)));
	        
	        for(treeb.val_nd=zero;treeb.val_nd<sizeof(bool);treeb.val_nd++)
	        {
	
	            if ((redundant)->tp[treeb.val_nd] == zero)
	            {
	                (redundant)->di[treeb.val_nd].page = rid.page;
	                (redundant)->di[treeb.val_nd].slot = rid.slot;
	                (redundant)->tp[treeb.val_nd] = (key)->v.intV;
	                treeb.ch_nd = treeb.ch_nd + sizeof(bool);
	                break;
	            }
	        }
	        if (treeb.ch_nd == zero)
	        {
	            if((redundant)->ext[treeb.cp_nd] == NULL)
            	{
	                (redundant)->ext[treeb.cp_nd] = info;
	            }
	        }
	        redundant = (redundant)->ext[sizeof(bool)];
        }	
	    return RC_OK;
	}

    int checkIfNull(InfoBTree *ibt) {
        return (ibt != NULL)? 1:0;
    }
    
    RC deleteKey (BTreeHandle *tree, Value *key)
	{
		int sizeTemp = sizeof(InfoBTree);
		int sh = sizeTemp;
		if(sh>=one)
		{
			treeb.lf = zero;
			redundant = (InfoBTree*)malloc(sizeof(InfoBTree));
			redundant = root;
			while (checkIfNull(redundant))
			{
				for(treeb.val_nd = zero;treeb.val_nd < sizeof(int);treeb.val_nd ++)
				{
					if ((redundant)->tp[treeb.val_nd] == (key)->v.intV)
					{
						(redundant)->tp[treeb.val_nd] = zero;
						(redundant)->di[treeb.val_nd].slot = zero;
						(redundant)->di[treeb.val_nd].page = zero;
						treeb.lf = one;
						break;
					}
				}
				if (treeb.lf == 1)
					break;
				redundant = (redundant)->ext[sizeof(bool)];
			}
		}
		return RC_OK; 
	}

RC nextEntry(BT_ScanHandle *handle, RID *result)
{
    redundant = (InfoBTree *)malloc(sizeof(InfoBTree));
    if (treeb.cp_nd != zero)
        return RC_ERR;
    else if (treeb.cp_nd == zero)
    {
        redundant = (redundant)->ext[treeb.cp_nd];
        (result)->page = (redundant)->di[treeb.et].page;
        (result)->page = (result)->page + six;
        (result)->slot = (redundant)->di[treeb.et].slot;
        (result)->slot = (result)->slot + six;
        treeb.et = treeb.et + sizeof(bool);
        return zero;
    }
    else
    {
        int tot = 303;
        return tot;
    }
}

/*
* Clients can scan through all entries of a B+ tree in sort order using theopenTreeScan.
* @param tree
* @param handle
*/
RC openTreeScan(BTreeHandle *tree, BT_ScanHandle **handle)
{
    treeb.et = 0;
    treeb.en_nd = 0;
    redundant = (InfoBTree *)malloc(sizeof(InfoBTree));
    redundant = root;
    InfoBTree *info = (InfoBTree *)malloc(sizeof(InfoBTree));

    for (info = root; info != NULL; info = (*info).ext[treeb.cp_nd])
    {
        treeb.val_nd = 0;
        while (treeb.val_nd < sizeof(int))
        {
            if ((*info).tp[treeb.val_nd] == 0)
            {
                printf("\n");
            }
            else
            {
                treeb.en_nd = treeb.en_nd + 1;
            }
            treeb.val_nd++;
        }
    }
    scanTree(tree, handle);
    return RC_OK;
}

RC scanTree(BTreeHandle *tree, BT_ScanHandle **handle)
{
    int tpp[treeb.en_nd];
    int le[treeb.cp_nd][treeb.en_nd];
    treeb.inc_nd = 0;
    InfoBTree *info = (InfoBTree *)malloc(sizeof(InfoBTree));
    info = root;
    while (info != NULL)
    {
        treeb.val_nd = 0;
        while (treeb.val_nd < sizeof(int))
        {
            tpp[treeb.inc_nd] = (info)->tp[treeb.val_nd];
            int pageNum = (info)->di[treeb.val_nd].page;
            le[zero][treeb.inc_nd] = pageNum;
            int slotNum = (info)->di[treeb.val_nd].slot;
            le[one][treeb.inc_nd] = slotNum;
            treeb.inc_nd = treeb.inc_nd + 1;
            treeb.val_nd = treeb.val_nd + 1;
        }
        info = (info)->ext[treeb.cp_nd];
    }
}

RC closeTreeScan(BT_ScanHandle *handle)
{
    if (handle == NULL)
        return RC_NOT_OK;
    free(handle);
    return RC_OK;
}

char *printTree(BTreeHandle *tree)
{
    char *treeInfo;

    printf("B-Tree as follows:%s", treeInfo);
    treeInfo = printTree(tree);

    return treeInfo;
}
