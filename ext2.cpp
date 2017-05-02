#ifndef EXT2
#define EXT2

#include "ext2.hpp"
#include "VirtualBox.cpp"
#include <cstring>
#include <stdio.h>
#include <assert.h>
#include <vector>

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

	blockGroupDescriptorTableLocation = ((superblock.s_log_block_size > 0) ? 1 : 2);

	// printf("Location: %lli\nblock_size: %x\nblocks_count: %u\nblocks_per_group: %u\nsize_groupdesc: %u\n",
	// 			(long long int) blockGroupDescriptorTableLocation,
	// 			(unsigned int) superblock.s_log_block_size,
	// 			(unsigned int) superblock.s_blocks_count,
	// 			(unsigned int) superblock.s_blocks_per_group,
	// 			(unsigned int) sizeof(ext2_group_desc));

	group_count = 1 + (superblock.s_blocks_count-1) / superblock.s_blocks_per_group;
	sizeBlockGDescritorTable = sizeof(ext2_group_desc) * group_count;

	//printf("NumberBlockGroups: %i\nsizeBlockGDescritorTable: %i\n", group_count, sizeBlockGDescritorTable );

	blockGDescriptorTable = new struct ext2_group_desc[group_count];

	data = getBlock(blockGroupDescriptorTableLocation, sizeBlockGDescritorTable);
	std::memcpy(blockGDescriptorTable,data,sizeBlockGDescritorTable);
	delete data;
	//printf("table[0].bg_inode_bitmap: 0x%x\n",blockGDescriptorTable[0].bg_inode_bitmap);
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

struct ext2_inode ext2::getInode(unsigned long long inode)
{
	/*
	 * By now, we have the block group discriptor table.
	 * We can use this to grab any specific inode.
	 * To do so, we need to lookup s_inodes_per_group and then calculate which block group that inode is in.
	 * Then, we lookup the inode in the appropiate bitmap from the bgdt. if it is free, then throw inodeNotAllocated();
	 * if allocated, then return the inode struct. The inode table can be found at blockGDescriptorTable.bg_inode_table + (blockgroup * superblock.s_blocks_per_group);
	 */

	 int blockGroupOfInode = inode / superblock.s_inodes_per_group;
	 unsigned long long inodeInGroup = inode % superblock.s_inodes_per_group;

	//  printf("inodeInGroup: %i\n",inodeInGroup);
	//  printf("blockGroupOfInode: %i\n", blockGroupOfInode);
	//  printf("inodesPerGroup: %u\n", superblock.s_inodes_per_group);

	 bool* inode_map;
	 inode_map = (bool*) getBlock(blockGDescriptorTable[blockGroupOfInode].bg_inode_bitmap, superblock.s_inodes_per_group / 8);

	 /*
	  * Of course, since sizeof(bool) = 1 byte, we now need to to some bit shifting to get the
		* information we want.
		*
	 */

	 int temp = inode_map[inodeInGroup / 8];
	 temp = (temp >> inodeInGroup) & 0x1;

	 if (temp == 0 || inode == 0)
	 {
		 printf("inodeNotAllocated\n");
		 throw inodeNotAllocated();
	 }

	 //TODO: check if inode is actually in use, but is not marked as allocated, or is marked as allocated but not in use.

	 //Get the inode:
	//  printf("Inode Block: 0x%x\n", blockGDescriptorTable[blockGroupOfInode].bg_inode_table);
	//  printf("Inode Offset: 0x%x\n", (superblock.s_inode_size * (inodeInGroup - 1)));

	 char* data = getBlock(blockGDescriptorTable[blockGroupOfInode].bg_inode_table, sizeof(ext2_inode), (superblock.s_inode_size * (inodeInGroup - 1)));

	 struct ext2_inode inodet;
	 std::memcpy(&inodet, data, sizeof(ext2_inode));

	 return inodet;
}

//check superblocks and verify their integrity:
int ext2::verify_superblocks()
{
	//take the number of block groups and then read in the superblock copies.

	printf("Block    Inodes    Blocks    Reserved     First     Log Block   Log Frag   Blocks Per\n"
				 "Group    Count     Count     Blocks     Data Block     Size         Size        Group\n"
			   "-------------------------------------------------------------------------------------\n");


	for (unsigned int i = 0; i < group_count; i++)
	{
		if (isPowerof357(i) || i == 0)
		{
			ext2_super_block *copy;
			unsigned int offset = 0;
			if (i == 0 || blockGroupDescriptorTableLocation - 1)
				offset = EXT2_SUPER_BLOCK_OFFSET;

			copy = (ext2_super_block *) getBlock(i * superblock.s_blocks_per_group, sizeof(ext2_super_block), offset);

			printf("%5u %9u %9u %11u %12u %11u %10u %12u\n",
						i,
						copy->s_inodes_count,
						copy->s_blocks_count,
						copy->s_r_blocks_count,
						copy->s_first_data_block,
						copy->s_log_block_size,
						copy->s_log_cluster_size,
						copy->s_blocks_per_group);
			delete copy;
		}
	}

	printf("\n\n"
				 "Block    Frags Per    Magic     Minor Revision    Creator    Revision    First    Inode\n"
				 "Group    Group                  Level             OS         Level       Inode    Size \n"
			 	 "---------------------------------------------------------------------------------------\n");

	for (unsigned int i = 0; i < group_count; i++)
	{
		if (isPowerof357(i) || i == 0)
		{
			ext2_super_block *copy;
			unsigned int offset = 0;
			if (i == 0 || blockGroupDescriptorTableLocation - 1)
				offset = EXT2_SUPER_BLOCK_OFFSET;

			copy = (ext2_super_block *) getBlock(i*superblock.s_blocks_per_group, sizeof(ext2_super_block), offset);

			printf("%5u %12u %8u %18u %10u %11u %8u %8u\n",
						i,
						copy->s_clusters_per_group,
						copy->s_magic,
						copy->s_minor_rev_level,
						copy->s_creator_os,
						copy->s_rev_level,
						copy->s_first_ino,
						copy->s_inode_size);

			delete copy;
		}
	}
	printf("\n\n");
	return 0;
}

int ext2::dump_blockgrouptables()
{
	printf("Group    Block     Inode      Inode    Free      Free        Used\n"
         "         Bitmap    Bitmap     Table    Blocks    Inodes      Dirs\n"
         "-----------------------------------------------------------------\n");
	for (unsigned int i = 0; i < group_count; i++)
	{
		printf("%5u %9u %9u %9u %9u %9u %9u\n",
					i,
					blockGDescriptorTable[i].bg_block_bitmap,
					blockGDescriptorTable[i].bg_inode_bitmap,
					blockGDescriptorTable[i].bg_inode_table,
					blockGDescriptorTable[i].bg_free_blocks_count,
					blockGDescriptorTable[i].bg_free_inodes_count,
					blockGDescriptorTable[i].bg_used_dirs_count);
	}
	printf("\n\n");
	return 0;
}

int ext2::verify_inodes(unsigned long long inodeNumber)
{
	/*
	 * Starting at inode 2 (root), read through each inode, recursively
	*/
	try {
		struct ext2_inode inodet = this->getInode(inodeNumber);
		printf("here!");

		//if file, return 0
		if (inodet.i_mode & EXT2_S_IFREG)
		{
			printf("inode: %u\n"
						"Size: %u\n"
						"links: %u\n",
						inodet.i_mode,
						inodet.i_size,
						inodet.i_links_count);
			return 0;
		}

		//if DIR, recurse through inodes in ext2_dir_entry_2
		if (inodet.i_mode & EXT2_S_IFDIR)
		{
			for (size_t i = 0; i < EXT2_NDIR_BLOCKS; i++)
			{
				if(inodet.i_block[i] != 0)
				{
					this->getDir(inodet.i_block[i], 0);
				}
				//printf("i: 0x%x\n", inodet.i_block[i]);
				//this->verify_inodes(inodet.i_block[i]); //WRONG
				//read in DIR entry
			}
		}
	}
	catch(inodeNotAllocated)
	{
			printf("Inode is not allocated");
			return 1;
	}

	return 0;
}

struct ext2_dir_entry_2 ext2::getDir(unsigned long block, unsigned long offsetDir)
{
	std::vector<struct ext2_dir_entry_2> dirEntries;
	struct ext2_dir_entry_2 temp;
	unsigned int totalRecordLength = 0;

	std::memcpy(&temp,this->getBlock(block, *(this->getBlock(block,2,4)), 0), *(this->getBlock(block,2,4)));

	printf("Block: 0x%x\nlength: %x\nFileType: %x\nName: %s\n", block, temp.name_len,temp.file_type, temp.name);

	totalRecordLength += temp.rec_len;

	dirEntries.push_back(temp);
	//if (offsetDir == 0)
		//return temp;
	bool moreEntries = totalRecordLength - (1024 << superblock.s_log_block_size);
	while(moreEntries)
	{
		char* length = this->getBlock(block,2,4 + totalRecordLength);
		unsigned int length2 = (unsigned int) *length;
		if (length2 >= (1024 << superblock.s_log_block_size))
			break;
		printf("length2: %u\n", length2);
		delete length;
		std::memcpy(&temp, this->getBlock(block, length2, totalRecordLength), length2);
		printf("%u\n", totalRecordLength);
		totalRecordLength += temp.rec_len;
		moreEntries = totalRecordLength - (1024 << superblock.s_log_block_size);
		dirEntries.push_back(temp);
	}

	return temp;

}

bool isPowerof357(unsigned int number)
{
	//this was a fun one
	//3^19, 5^19, and 7^19 respectively. Avoids the pesky while loop by seeing if max unsigned int is divisible evenly.
	return (number != 0 && 3486784401u % number == 0) || (number != 0 && 95367431640625u % number == 0) || (number != 0 && 79792266297612001u % number == 0);
}

#endif
