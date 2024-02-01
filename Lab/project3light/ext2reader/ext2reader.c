// Copyright 2009 Steve Gribble (gribble [at] cs.washington.edu).
// May 22nd, 2009.
//
// This is the "main" file for project 3 light.  We've written
// all of the code for you in this file; you don't need to make
// any changes here.  The code in this file invokes functions that
// you'll implement in extaccess.c, as well as the test code
// we've provided in testcode.c.
//
// It's worth reading through this file to understand what's happening:
// it's pretty straightforward.

#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "inc/types.h"
#include "inc/blockgroup_descriptor.h"
#include "inc/directoryentry.h"
#include "inc/inode.h"
#include "inc/superblock.h"
#include "inc/ext2access.h"
#include "inc/testcode.h"

// Usage:  ext2reader diskfile_name
int main(int argc, char **argv) {
  // open up the disk file
  if (argc != 2) {
    printf("usage:  ext2reader diskfile_name\n");
    return -1;
  }
  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    printf("couldn't open file \"%s\"\n", argv[1]);
    return -1;
  }

  // read the superblock off the disk and into memory.  you get
  // to implement this function in file ext2_access.c.
  struct os_superblock_t *superblock = read_superblock(fd);
  check_superblock(superblock);  // our test code

  // calculate filesystem metadata.  you get to implement this
  // function in file ext2_access.c.
  struct os_fs_metadata_t *metadata = calc_metadata(fd, superblock);
  check_metadata(metadata, superblock);  // our test code

  // allocate and populate the blockgroup descriptor table.  you get
  // to implement this function in file ext2_access.c.
  struct os_blockgroup_descriptor_t *bgdt = read_bgdt(fd, metadata);
  check_bgdt(metadata, bgdt);  // our test code

  // check that your inode retrieval function works
  check_fetch_inode(fd, metadata, bgdt);

  // check that your indirection index calculation function works
  check_calculate_offsets();

  // check that file_blockread() works correctly
  check_file_blockread(fd, metadata);

  // check that file_read() works correctly
  check_file_read(fd, metadata);

  // check that pop_dir_component() works correctly
  check_pop_dir_component();

  // check that scan_dir() works correctly
  check_scan_dir(fd, metadata);

  // check that ls_dir() works correctly
  check_ls_dir(fd, metadata);

  // check that path_read() works correctly
  check_path_read(fd, metadata);

  // all done; quit!
  close(fd);
  print_grade();
  return 0;
}
