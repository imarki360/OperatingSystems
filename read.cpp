//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "ext2_fs.h"
#include "read.h"
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <stdlib.h>
#include <iomanip>

#include "classes.cpp"
#include "ext2.cpp"


const int EXT2_SUPER_BLOCK_OFFSET = 1024;

int main()
{
	char pathname[280] = "Debian1.0.vdi";
	int file = open(pathname,O_RDONLY);
	//int length = lseek(file,0,SEEK_END) + 1;

	VirtualBoxClass vb(file);

	/*
	 *	from here we can access the disk as it was a physical disk using
	 * getData(char* data[], file descriptor, header,image_map, bytesStart, bytesToRead);
	 * bytesStart is the "virtual" location of the disk to read in at. eg: 0 is begining of the disk
	 */


	/*
	 * Now that we have the MBR, we can then figure out where the ext2 filesystem lies.
	*/

	ext2 ext2FileSystem(&vb);

	/* //Debug:
	printf("inode_count: %x\n",superblock.s_inodes_count);
	printf("firstSector: %x\n",boot.partitionTable[0].firstSector);
	printf("sector_size: %x\n",header.sector_size);
	*/
	/*
	 * Okay, using the superblock, we can now go thorugh the inodes and work thorugh the directory structure
	 */

	 //first lets find position of block group descriptor table:


	 /*
	  * The block group discripor table is located in:
		* third block on a 1KiB file system
		* or 2nd block on a >= 2KiB file system
		* file system size is superblock.s_log_block_size (block size = 1024 << s_log_block_size;)
		*
	 */
	 /*

	 struct ext2_group_desc blockDescriptor;
	 char *data2 = new char[sizeof(ext2_group_desc)];

	 getData(data2, file, header, image_map, (1024 << superblock.s_log_block_size) * bgdtPosition, superblock.s_blocks_count / superblock.s_blocks_per_group);


	//loop through for each inode and place each inode into an array of structs
	struct ext2_inode *inodes[superblock.s_inodes_count];
	*/
	//for (size_t i = 0; i < superblock.s_inodes_count; i++)
//	{
		/* code */
		//so, we want to read in each inode and put them into the struct array. To do This
		//we need to grab the position of the current inode. then read it into the array.

		//allocate struct
		//inodes[i] = new struct ext2_inode;

		//calculate inode position


		//read in inode
		//inodes[i] =
//	}

	printf("\n");
	return 0;
}
