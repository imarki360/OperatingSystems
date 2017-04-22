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
  struct ext2_group_desc* blockGDescriptorTable;
  unsigned int group_count;
  unsigned int sizeBlockGDescritorTable;
  long long blockGroupDescriptorTableLocation;

public:
  ext2(VirtualBoxClass* VirtualBox);

  char* getBlock(int blockNumber);
  char* getBlock(int blockNumber, int bytes);
  char* getBlock(int blockNumber, int bytes, int offsetBytes);

  struct ext2_inode getInode(int blockNumber, int inode);
  int verify_superblocks();
  int verify_blockgrouptables();
  int verify_inodes();
  int verify_directory();
  int verify_datablocks();

  //output based:
  char* general_stats();
};
#endif
