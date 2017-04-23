#ifndef EXT2
#define EXT2

#include "ext2.hpp"
#include "VirtualBox.cpp"
#include <cstring>
#include <stdio.h>

ext2::ext2(VirtualBoxClass* VirtualBox) : vb(VirtualBox)
{

  //so, lets read in the Boot Sector
	int arraySize = sizeof(BootSector);
	char* data = new char[arraySize];
	//printf("%d", arraySize);
	VirtualBox->getBytes(data, 0, arraySize);
	std::memcpy(&mbr, data, arraySize);
	delete data;

  /*
	 * Now that we have the MBR, we can then figure out where the ext2 filesystem lies.
	 * TODO: Find first valid linux partition and check magic number
	*/

  ext2FirstSector_byte = mbr.partitionTable[0].firstSector * VirtualBox->header.sector_size;
	printf("ext2FirstSector_byte: 0x%x\n",ext2FirstSector_byte);

  /*
   * Now its time to start reading in the ext2 filesystem information. First, lets read in the superblock.
	 * TODO: Check superblock magic number
  */

  data = new char[sizeof(ext2_super_block)];
  VirtualBox->getBytes(data, ext2FirstSector_byte + EXT2_SUPER_BLOCK_OFFSET, sizeof(ext2_super_block));
	std::memcpy(&superblock,data,sizeof(ext2_super_block));
	delete data;

	printf("Magic: 0x%x\nSuperblock_size: %lu\n",superblock.s_magic,sizeof(ext2_super_block));

	/*
	 * Now, lets read in the Block Group descriptor table
	*/

	blockGroupDescriptorTableLocation = ((superblock.s_log_block_size >= 2) ? 2 : 3) - 1;

	printf("Location: %lli\nblock_size: %u\nblocks_count: %u\nblocks_per_group: %u\nsize_groupdesc: %lu\n",
				blockGroupDescriptorTableLocation,
				superblock.s_log_block_size,
				superblock.s_blocks_count,
				superblock.s_blocks_per_group,
				sizeof(ext2_group_desc));

	group_count = 1 + (superblock.s_blocks_count-1) / superblock.s_blocks_per_group;
	sizeBlockGDescritorTable = sizeof(ext2_group_desc) * group_count;

	printf("NumberBlockGroups: %i\nsizeBlockGDescritorTable: %i\n", group_count, sizeBlockGDescritorTable );

	blockGDescriptorTable = new struct ext2_group_desc[group_count];

	//data = new char[sizeBlockGDescritorTable];
	data = getBlock(blockGroupDescriptorTableLocation, sizeBlockGDescritorTable);
	blockGDescriptorTable = (ext2_group_desc *) data;
	//std::memcpy(&blockGDescriptorTable, data, sizeBlockGDescritorTable);
	delete data;
	//printf("table[1].bg_block_bitmap: 0x%x\n",blockGDescriptorTable[1].bg_block_bitmap);

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

struct ext2_inode ext2::getInode(int blockNumber, int inode)
{
	/*
	 * By now, we have the block group discriptor table.
	 * We can use this to grab any specific inode.
	 */
}

//check superblocks and verify their integrity:
int ext2::verify_superblocks()
{
	//take the number of block groups and then read in the superblock copies.
}


#endif
