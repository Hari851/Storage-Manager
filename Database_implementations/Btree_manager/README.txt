Group 12 - Assignment 3(Optional)  

Team Members

Mayank Rajesh Lodha -->Implemented functions -->findKey,shutdownIndexManager
Srushti Ajay Patil -->Implemented functions -->createBtree, getNumNodes.
Hariharan Janakiraman --> Implemented functions --> initIndexManager,closeBtree, getKeyType, openTreeScan
Tanya Sinha--> Implemented functions -->openBtree,deleteKey,getNumEntries.
Naveen Kumar Gavara--> Implemented functions --> deleteBtree, next entry
Manoj Reddy Kichaiah Gari--> Implemented functions --> insert key, close tree scan

--------------------------------------------------------------------------------------------------------------------------------
Running the Make
mingw32-make -f Make test_case1
mingw32-make -f Make test_case2
--------------------------------------------------------------------------------------------------------------------------------

BTree Manager

Submission-27th April 2021

Project Modules

C Source Files
1.btree_mgr.c
2.btree_exec.c
3.buffer_mgr.c
4.buffer_mgr_stat.c
5.dberror.c
6.test_assign4_1.c
8.Storage_mgr.c

Header Files
1.storage_mgr.h
2.dberror.h
3.test_helper.h
4.buffer_mgr.h
5.buffer_mgr_stat.h
6.dt.h
7.btree_mgr.h
8.expr.h
9.tables.h
--------------------------------------------------------------------------------------------------------------------------------

Steps for Running

1.Create folder and Clone git repository at the location.
2.Navigate to optional folder.
3.Use make command to compile.
    $make
4.To clean the BTree manager execute clean command.
    $make clean
    
------------------------------------------------------------------------------------------------------------------------------

Method Description

shutdownIndexManager: Shut downs the index manager.

createBtree: Creates a new node manager for btree manager. Checks if the manager is empty. If the manager is not empty, then creates a new page file
by using createpagefile method with the btree index id. If it is not then uses the openPagefile method to open the btree index manager for the particular index id.

getNumNodes: Travels through the nodes in btree for all search entries in the node manager.Increments the count of nodes and Store it in the result variable.

findKey: Travels through the BTree to  search for a particular key.Once the key is found, it makes a note of the index where the key was found.
It then sets the flag to 1 and return RC_OK if flag is 1 else RC_KEY_NOT_FOUND error message.

initIndexManager: These functions are used to initialize the index manager and shut it down, freeing up all acquired resources.

closeBtree: When closing a b-tree (closeBtree), the index manager should ensure that all new or modified pages of the index are flushed back to disk (use the buffer manager function for that).

getKeyType: check the datatype of the keys in the btree whether they are int, float, bool or string and store them in the datatype parameter. Returns true. Also checks if tree is empty.

openTreeScan : Clients can scan through all entries of a B+ tree in sort order using theopenTreeScan.

openBtree: This function initializes a buffer pool handle for the btree index. FIFO replacement strategy is used to pinPage the buffer to the index. Returns RC_OK if the operation is successful.

deleteKey: This function is used to remove a key and its corresponding pointers from the index. 
It returns RC_IM_KEY_NOT_FOUND if the key is not in the index. First, we are traversing the tree to find the key to be deleted.
If the leaf node is empty after deletion of the key, free it or else if it contains one key, merge it with other nodes using insertKey logic. Returns RC_OK if the operations are successful.

getNumEntries: This function is used to return number of keys in the btree index manager. Here, we are traversing the btree and 
increasing count by one and storing the result in *result parameter and returning it. Returns RC_OK if successful.

deleteBtree: The function will return the btree index and then returns true

next entry: This function will check if the node capcity is zero. If not, check for the next value in the tree from the index.
It will then store the value in the variable declared in the function. It'll then UnPinPage from the btree buffer pool handle
and will return true.

insert key: This function will insert the value, key and rid for the key to be inserted. It'll then traverse through the btree
to check where to insert the key. If the leaf node is full, it'll split it and insert the key to the right nide. If not
it'll insert directly and then returns true.

close tree scan: It'll free the buffer pool data. It'll will then free the btree handle and then returns true.