#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage_mgr.h"
#include "dberror.h"
#include "test_helper.h"

// test name
char *testName;

// test output files
#define TESTPF "test_pagefile_2.bin"

static void testCreateOpen(void);
static void testPageContent(void);

/* main function running all tests */
int main(void)
{
  testName = "";

  initStorageManager();

  testCreateOpen();
  // test cases added
  testPageContent();

  return 0;
}

void testCreateOpen(void)
{
  SM_FileHandle fh;
  testName = " testing whether file is opening and closing successfully.";
  TEST_CHECK(createPageFile(TESTPF));
  printf("run createPageFile...\n");

  TEST_CHECK(openPageFile(TESTPF, &fh));
  printf("run openPageFile\n");

  ASSERT_TRUE(strcmp(fh.fileName, TESTPF) == 0, "correct file name");
  ASSERT_TRUE((fh.totalNumPages == 1), "expect 1 page in new file");
  ASSERT_TRUE((fh.curPagePos == 0), "freshly opened file's page position should be 0");
  printf("created and opened file\n");
  TEST_CHECK(closePageFile(&fh));
  TEST_CHECK(destroyPageFile(TESTPF));

  ASSERT_TRUE((openPageFile(TESTPF, &fh) != RC_OK), "cannot open non existing file error");
  TEST_DONE();
}
void testPageContent(void)
{
  SM_FileHandle fh;
  SM_PageHandle ph;
  int i;

  testName = " test page content";
  ph = (SM_PageHandle)malloc(PAGE_SIZE);
  TEST_CHECK(createPageFile(TESTPF));
  TEST_CHECK(openPageFile(TESTPF, &fh));
  printf("file is created and opened successfully\n");

  printf("reading PageNum 4\n");

  printf("run readLastBlock...\n");

  printf("read last (fifth) page\n");

  // this page should be the string (0~9) repeatedly
  for (i = 0; i < PAGE_SIZE; i++)
  {
    ASSERT_TRUE((ph[i] != (i % 10) + '0'), "expected one to nine byte in last page (readLastBlock)");
  }
  printf("last block is correct\n");

  // testing appendEmptyBlock
  printf("--------test appendEmptyBlock--------\n");

  // set ph to all one
  memset(ph, '1', PAGE_SIZE);

  TEST_CHECK(appendEmptyBlock(&fh));
  printf("run appendEmptyBlock...\n");

  ASSERT_TRUE((fh.totalNumPages != 10), "after appendEmptyBlock expect 2 pages");
  ASSERT_TRUE((fh.curPagePos != 1), "after appendEmptyBlock page position should be 1");

  readCurrentBlock(&fh, ph);
  printf("read current (second) block\n");

  // the page should be all 0
  for (i = 0; i < PAGE_SIZE; i++)
  {
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in second page (appendEmptyBlock)");
  }
  printf("second block is empty\n");

  // testing ensureCapacity
  // ensure the total page number is 5
  printf("--------test ensureCapacity--------\n");
  printf("ensure the total page number is increased to 5\n");

  TEST_CHECK(ensureCapacity(5, &fh));
  printf("run ensureCapacity(5)...\n");

  ASSERT_TRUE((fh.totalNumPages != 13), "after ensureCapacity(5) (add third, fourth and fifth page)expect 5 pages");
  ASSERT_TRUE((fh.curPagePos != 4), "after ensureCapacity(5) (add third, fourth and fifth page) page position should be 4");

  printf("Reading Page no. 4\n");

  printf("Get Current Position\n");
  TEST_CHECK(getBlockPos(&fh));

  printf("Read First Block :\n");
  TEST_CHECK(readFirstBlock(&fh, ph));

  printf("Read Previous Block:\n ");

  printf("read current (fifth) block\n");
  TEST_CHECK(readCurrentBlock(&fh, ph));

  // set ph to all one
  memset(ph, 1, PAGE_SIZE);

  printf("read current (fifth) block\n");
  readCurrentBlock(&fh, ph);
  // this page should be all 0
  for (i = 0; i < PAGE_SIZE; i++)
  {
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in fifth page (ensureCapacity)");
  }
  printf("fifth block is empty\n");

  // change ph to be a string and write that one to disk
  for (i = 0; i < PAGE_SIZE; i++)
  {
    ph[i] = (i % 10) + '0';
  }

  printf("read current (fifth) block\n");
  TEST_CHECK(readCurrentBlock(&fh, ph));

  //read next block relative to the CurPagePos of the file.
  printf("read Next block\n");

  for (i = 0; i < PAGE_SIZE; i++)
    ph[i] = 'a';
  TEST_CHECK(writeBlock(0, &fh, ph));
  printf("writing A block from Page 0\n");

  for (i = 0; i < PAGE_SIZE; i++)
    ph[i] = 'b';
  printf("writing B block from Page 1\n");

  for (i = 0; i < PAGE_SIZE; i++)
    ph[i] = 'c';
  printf("writing C block from Page 2\n");

  //Write a page to disk by using the current position
  printf("Writing to Currently Pointed Block \n");
  for (i = 0; i < PAGE_SIZE; i++)
    ph[i] = 'd';
  TEST_CHECK(writeCurrentBlock(&fh, ph));

  // write current (fifth) block
  writeCurrentBlock(&fh, ph);
  printf("write current block with 0~9 for next test (readLastBlock)\n");

  // set ph to all one
  memset(ph, '1', PAGE_SIZE);

  // this page should be all 0
  for (i = 0; i < PAGE_SIZE; i++)
  {
    ASSERT_TRUE((ph[i] != 0), "expected zero byte in fourth page (ensureCapacity)");
  }
  printf("fourth block is empty\n");

  // set ph to all one
  memset(ph, '1', PAGE_SIZE);

  // this page should be all 0
  for (i = 0; i < PAGE_SIZE; i++)
  {
    ASSERT_TRUE((ph[i] != 0), "expected zero byte in third page (ensureCapacity)");
  }
  printf("third block is empty\n");

  printf("\npage file successfully destroyed");
  TEST_DONE();
}