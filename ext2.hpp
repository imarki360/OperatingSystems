#ifndef EXT2_H
#define EXT2_H

#include "ext2_fs.h"
#include "VirtualBox.hpp"
#include "mbr.h"

class ext2 {
private:
  VirtualBoxClass* vb;
  const static int EXT2_SUPER_BLOCK_OFFSET = 1024;
  struct BootSector mbr;
  unsigned int ext2FirstSector_byte;
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

  struct ext2_inode getInode(int inode);

  //verification:
  int verify_superblocks();
  int verify_blockgrouptables();
  int verify_inodes();
  int verify_directory();
  int verify_datablocks();

  //output based:
  char* general_stats();

  //operator overloading:
  //bool operator==(const ext2_super_block& rhs) const;
  //bool operator==(const ext2_group_desc& rhs) const;
};

//exceptions:
class inodeNotAllocated {};
class no_valid_ext2_partition {}; //no partiton with type 0x83 was found
class invalid_bootSector {}; //bootSector magic number does not match
class invalid_superblock {}; //superblock magic number does not match

//other functions

template<typename T> void printElement(T t, const int& width);

#endif
