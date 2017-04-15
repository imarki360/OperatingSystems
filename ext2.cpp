#ifndef EXT2
#define EXT2

#include "ext2.hpp"
#include "classes.cpp"
#include <cstring>

ext2::ext2(VirtualBoxClass* vb) : vb(vb)
{
  //so, lets read in the Boot Sector
	int arraySize = sizeof(BootSector);
	char* data = new char[arraySize];
	vb->getBytes(data, 0, arraySize);
	std::memcpy(&mbr, data, arraySize);
	delete data;

  /*
	 * Now that we have the MBR, we can then figure out where the ext2 filesystem lies.
	 * TODO: Find first valid linux partition and check magic number
	*/

  ext2FirstSector_byte = mbr.partitionTable[0].firstSector * vb->header.sector_size + EXT2_SUPER_BLOCK_OFFSET;

  /*
   * Now its time to start reading in the ext2 filesystem information. First, lets read in the superblock.
  */

  data = new char[sizeof(ext2_super_block)];
  vb->getBytes(data, ext2FirstSector_byte, sizeof(ext2_super_block));
	std::memcpy(&superblock,data,sizeof(ext2_super_block));
	delete data;

	/*
	 * Now, lets read in the Block Group descriptor table
	*/

	blockGroupDescriptorTableLocation = ((superblock.s_log_block_size >= 2) ? 2 : 3);

	data = getBlock(blockGroupDescriptorTableLocation,sizeof(ext2_group_desc));
	memcpy(&blockGDescriptor, data, sizeof(ext2_group_desc));
	delete data;

}

char* ext2::getBlock(int blockNumber)
{
	char* data = new char[1024 << superblock.s_log_block_size];

	vb->getBytes(data, (blockNumber * ( 1024 << superblock.s_log_block_size)) + ext2FirstSector_byte, (1024 << superblock.s_log_block_size));

	return data;
}

char* ext2::getBlock(int blockNumber, int bytes)
{
	char* data = new char[bytes];

	vb->getBytes(data, (blockNumber * ( 1024 << superblock.s_log_block_size)) + ext2FirstSector_byte, bytes);

	return data;
}

char* ext2::getBlock(int blockNumber, int bytes, int offsetBytes)
{
	char* data = new char[bytes];

	vb->getBytes(data, (blockNumber * ( 1024 << superblock.s_log_block_size)) + ext2FirstSector_byte + offsetBytes, bytes);

	return data;
}

struct ext2_inode ext2::getInode(int inode)
{
	
}

#endif
