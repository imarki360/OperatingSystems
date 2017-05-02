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

  struct ext2_inode getInode(unsigned long long inode);
  struct ext2_dir_entry_2 getDir(unsigned int block, unsigned int offsetDir);

  //verification and output:
  int verify_superblocks();
  int dump_blockgrouptables();
  int verify_inodes(unsigned long long = 2);
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
bool isPowerof357(unsigned int);

//Defines:
#define EXT2_S_IFSOCK	0xC000	//socket
#define EXT2_S_IFLNK	0xA000	//symbolic link
#define EXT2_S_IFREG	0x8000	//regular file
#define EXT2_S_IFBLK	0x6000	//block device
#define EXT2_S_IFDIR	0x4000	//directory
#define EXT2_S_IFCHR	0x2000	//character device
#define EXT2_S_IFIFO	0x1000	//fifo
//-- process execution user/group override --
#define EXT2_S_ISUID	0x0800	//Set process User ID
#define EXT2_S_ISGID	0x0400	//Set process Group ID
#define EXT2_S_ISVTX	0x0200	//sticky bit
//-- access rights --
#define EXT2_S_IRUSR	0x0100	//user read
#define EXT2_S_IWUSR	0x0080	//user write
#define EXT2_S_IXUSR	0x0040	//user execute
#define EXT2_S_IRGRP	0x0020	//group read
#define EXT2_S_IWGRP	0x0010	//group write
#define EXT2_S_IXGRP	0x0008	//group execute
#define EXT2_S_IROTH	0x0004	//others read
#define EXT2_S_IWOTH	0x0002	//others write
#define EXT2_S_IXOTH	0x0001	//others execute

#endif
