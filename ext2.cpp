#ifndef EXT2
#define EXT2

#include "ext2.hpp"
#include "VirtualBox.cpp"
#include <cstring>
#include <stdio.h>
#include <assert.h>
#include <iomanip>
#include <iostream>

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
	 * Find first valid linux partition and check magic number
	*/
	int ext2_partition = -1;
	for (int partNum = 0; partNum <= MBR_NUM_PARTITIONS; partNum++)
	{
		if (mbr.partitionTable[partNum].type == 0x83)
		{
			ext2_partition = partNum;
		}
	}
	if (ext2_partition == -1)
		throw no_valid_ext2_partition();
	assert(ext2_partition >= 0 && ext2_partition <= 4);

	if (mbr.magic != BOOT_SECTOR_MAGIC)
		throw invalid_bootSector();

  ext2FirstSector_byte = mbr.partitionTable[ext2_partition].firstSector * VirtualBox->header.sector_size;
	//printf("ext2FirstSector_byte: 0x%x\n",ext2FirstSector_byte);

  /*
   * Now its time to start reading in the ext2 filesystem information. First, lets read in the superblock.
	 * Checks superblock magic number
  */

  data = new char[sizeof(ext2_super_block)];
  VirtualBox->getBytes(data, ext2FirstSector_byte + EXT2_SUPER_BLOCK_OFFSET, sizeof(ext2_super_block));
	std::memcpy(&superblock,data,sizeof(ext2_super_block));
	delete data;

	if (superblock.s_magic != EXT2_SUPER_MAGIC)
		throw invalid_superblock();

	//printf("Magic: 0x%x\nSuperblock_size: %lu\n",superblock.s_magic,sizeof(ext2_super_block));

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

	//printf("NumberBlockGroups: %i\nsizeBlockGDescritorTable: %i\n", group_count, sizeBlockGDescritorTable );

	blockGDescriptorTable = new struct ext2_group_desc[group_count];

	data = getBlock(blockGroupDescriptorTableLocation, sizeBlockGDescritorTable);
	blockGDescriptorTable = (ext2_group_desc *) data;
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

struct ext2_inode ext2::getInode(int inode)
{
	/*
	 * By now, we have the block group discriptor table.
	 * We can use this to grab any specific inode.
	 * To do so, we need to lookup s_inodes_per_group and then calculate which block group that inode is in.
	 * Then, we lookup the inode in the appropiate bitmap from the bgdt. if it is free, then throw inodeNotAllocated();
	 * if allocated, then return the inode struct
	 */

	 int blockGroupOfInode = inode / superblock.s_inodes_per_group;
	 int inodeInGroup = inode % superblock.s_inodes_per_group;
	 int* inode_map = new int[superblock.s_inodes_per_group];
	 char* data = getBlock(blockGDescriptorTable[blockGroupOfInode].bg_inode_bitmap, sizeof(__u32) * superblock.s_inodes_per_group, sizeof(__u32) * superblock.s_blocks_per_group);

	 inode_map = (int*) data;
 	 //TODO: check if allocated

	 //TODO: check if inode is actually in use, but is not marked as allocated, or is marked as allocated but not in use.

	 struct ext2_inode hi;
	 return hi;
}

//check superblocks and verify their integrity:
int ext2::verify_superblocks()
{
	//take the number of block groups and then read in the superblock copies.
	const int width = 17;

	printElement("block Group",width);
	printElement("inodes count", width);
	printElement("blocks count", width);
	printElement("r blocks count", width);
	printElement("first data block", width);
	printElement("log block size", width);
	printElement("log frag size", width);
	printElement("blocks per group", width);

	std::cout << std::endl;

	printElement("Superblock",width);
	printElement(superblock.s_inodes_count, width);
	printElement(superblock.s_blocks_count, width);
	printElement(superblock.s_r_blocks_count, width);
	printElement(superblock.s_first_data_block, width);
	printElement(superblock.s_log_block_size, width);
	printElement(superblock.s_log_cluster_size, width);
	printElement(superblock.s_blocks_per_group, width);

	std::cout << std::endl;

	for (unsigned int i = 0; i < group_count; i++)
	{
		if (isPowerof357(i))
		{
			ext2_super_block *copy;
			copy = (ext2_super_block *) getBlock(i*superblock.s_blocks_per_group,( 1024 << superblock.s_log_block_size), superblock.s_first_data_block);

			//print current block group
			printElement(i,width);
			//test the important bits;
			printElement(copy->s_inodes_count, width);
			printElement(copy->s_blocks_count, width);
			printElement(copy->s_r_blocks_count, width);
			printElement(copy->s_first_data_block, width);
			printElement(copy->s_log_block_size, width);
			printElement(copy->s_log_cluster_size, width);
			printElement(copy->s_blocks_per_group, width);

			std::cout << std::endl;

			delete copy;
		}
	}
	std::cout << std::endl;

	printElement("block Group",width);
	printElement("frags per group", width);
	printElement("magic", width);
	printElement("minor rev level", width);
	printElement("creator os", width);
	printElement("rev level", width);
	printElement("first ino", width);
	printElement("inode size", width);

	std::cout << std::endl;
	printElement("Superblock",width);
	printElement(superblock.s_clusters_per_group, width);
	printElement(superblock.s_magic, width);
	printElement(superblock.s_minor_rev_level, width);
	printElement(superblock.s_creator_os, width);
	printElement(superblock.s_rev_level, width);
	printElement(superblock.s_first_ino, width);
	printElement(superblock.s_inode_size, width);

	std::cout<<std::endl;

	for (unsigned int i = 1; i < group_count; i++)
	{
		if (isPowerof357(i))
		{
			ext2_super_block *copy; // = new ext2_super_block[sizeof(ext2_super_block)];
			copy = (ext2_super_block *) getBlock(i*superblock.s_blocks_per_group,( 1024 << superblock.s_log_block_size), superblock.s_first_data_block);

			//print current block group
			printElement(i,width);
			//test the important bits;
			printElement(copy->s_clusters_per_group, width);
			printElement(copy->s_magic, width);
			printElement(copy->s_minor_rev_level, width);
			printElement(copy->s_creator_os, width);
			printElement(copy->s_rev_level, width);
			printElement(copy->s_first_ino, width);
			printElement(copy->s_inode_size, width);

			delete copy;

			std::cout << std::endl;
		}
	}
	return 0;
}

int ext2::verify_blockgrouptables()
{
	for (unsigned int i = 0; i < group_count; i++)
	{
		if (isPowerof357(i))
		{

		}
	}
	return 0;
}

template<typename T> void printElement(T t, const int& width)
{
    std::cout << std::left << std::setw(width) << std::setfill(' ') << t;
}

bool isPowerof357(unsigned int number)
{
	//this was a fun one
	return (number != 0 && 3486784401u % number == 0) || (number != 0 && 95367431640625u % number == 0) || (number != 0 && 79792266297612001u % number == 0);
}

#endif
