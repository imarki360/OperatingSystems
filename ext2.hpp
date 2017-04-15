#ifndef EXT2_H
#define EXT2_H

#include "ext2_fs.h"
#include "classes.hpp"
#include "mbr.h"

class ext2 {
private:
  VirtualBoxClass* vb;
  const static int EXT2_SUPER_BLOCK_OFFSET = 1024;
  struct BootSector mbr;
  int ext2FirstSector_byte;
  struct ext2_super_block superblock;
  struct ext2_group_desc blockGDescriptor;
  long long blockGroupDescriptorTableLocation;

public:
  ext2(VirtualBoxClass* vb);

  char* getBlock(int blockNumber);
  char* getBlock(int blockNumber, int bytes);
  char* getBlock(int blockNumber, int bytes, int offsetBytes);

  struct ext2_inode getInode(int inode);
};
#endif
