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

const int EXT2_SUPER_BLOCK_OFFSET = 1024;

//Function to get "physical" page from the vdi file
int getRealPage(__s32 image_map[], int Page, int diskSize);
//get data from file
//template <class T>
int getData(char* data, int fd, VirtualBox header, __s32 image_map[], int byteStart, int bytes);

int getBlock(int);
int getBlock(int, int);

int main()
{
	struct VirtualBox header;
	struct BootSector boot;
	char pathname[280] = "Debian1.0.vdi";
	int file = open(pathname,O_RDONLY);
	//int length = lseek(file,0,SEEK_END) + 1;

	lseek(file, 0, SEEK_SET);
	read(file, &header, sizeof(header));
	lseek(file, header.offset_blocks, SEEK_SET);
	//Read in image map as 32-bit entries
	__s32* image_map = new __s32[header.disk_size_bytes >> 20]; //malloc(sizeof(__s32) * (header.disk_size_bytes >> 20));//
	read(file, image_map, (header.disk_size_bytes >> 20));

	/*
	 *	from here we can access the disk as it was a physical disk using
	 * getData(char* data[], file descriptor, header,image_map, bytesStart, bytesToRead);
	 * bytesStart is the "virtual" location of the disk to read in at. eg: 0 is begining of the disk
	 */

	//so, lets read in the Boot Sector
	int arraySize = sizeof(BootSector);
	char* data = new char[arraySize];
	getData(data, file, header, image_map, 0, arraySize);
	memcpy(&boot, data, arraySize);
	delete data;

	/*
	 * Now that we have the MBR, we can then figure out where the ext2 filesystem lies.
	*/

	//prepare to read in ext2_super_block
	struct ext2_super_block superblock;
	//read in superblock
	char* data_superblock = new char[sizeof(ext2_super_block)];
	getData(data_superblock, file, header,image_map, (boot.partitionTable[0].firstSector * header.sector_size) + EXT2_SUPER_BLOCK_OFFSET, sizeof(ext2_super_block));
	memcpy(&superblock, data_superblock, sizeof(ext2_super_block));
	delete data_superblock;

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
	 printf("Filesytem type: %i\n", (superblock.s_log_block_size));
	 int bgdtPosition = ((superblock.s_log_block_size >= 2) ? 2 : 3);
	 printf("bgdtPosition: %i\n", bgdtPosition);


	//loop through for each inode and place each inode into an array of structs
	struct ext2_inode *inodes[superblock.s_inodes_count];
	for (size_t i = 0; i < superblock.s_inodes_count; i++)
	{
		/* code */
		//so, we want to read in each inode and put them into the struct array. To do This
		//we need to grab the position of the current inode. then read it into the array.

		//allocate struct
		inodes[i] = new struct ext2_inode;

		//calculate inode position


		//read in inode
		//inodes[i] =
	}

	printf("\n");
	return 0;
}

//template <class T>
int getData(char* data, int fd, VirtualBox header, __s32 image_map[], int byteStart, int bytes)
{

	int byteDiff = bytes;// - byteStart;
	int virtualPageStart = byteStart >> 20;
	int currentBytes = 0;
	int currentPage = virtualPageStart;

	while(byteDiff - currentBytes > 0)
	{

		int realPage = getRealPage(image_map, currentPage, header.disk_size_bytes);
		int pageOffset = header.offset_data + byteStart + ((realPage - currentPage) << 20);
		int bytesToRead = byteDiff - currentBytes;
		char* dataBuffer = new char[bytesToRead];//malloc(sizeof(char) * bytesToRead);

    printf("Bytes to Read: 0x%x\nRealPage: 0x%x\npageOffset: 0x%x\nHeaderOffset: 0x%x\n\n",bytesToRead, realPage, pageOffset,header.offset_data);

		if (realPage == -1)
		{
			memset(dataBuffer,0, bytesToRead);
		}
		else
		{
			lseek(fd,pageOffset,SEEK_SET);
			read(fd, dataBuffer, bytesToRead);
			//printf("pageOffset: %x\nbytesToRead: %x\n\n",pageOffset,bytesToRead);
/*
			for (int i = 0; i < bytesToRead; i++)
			{
				printf("%04x",dataBuffer[i]);
				if (i % 3 == 0)
					printf("\t");
				if (i % 15 == 0)
					printf("\n");

			}*/
			//printf("%02x",dataBuffer[0]);
		}
		//printf("RawData: \n");
		for (int j = 0; j <= bytesToRead; j++)
		{
			data[currentBytes + j] = dataBuffer[j];
			//printf("%x", dataBuffer[j]);

		}
		//printf("\n----\n");
		currentBytes += bytesToRead;
		currentPage++;
	}

	//seek to begining again
	lseek(fd, 0, SEEK_SET);
	return 0;
}


/*
* argv __u32[] image_map
* argv int location = virtual location of page on disk
*
* Return: int - actual location of page on disk.
*/
int getRealPage(__s32 image_map[], int Page, int diskSize)
{
	//add check for out of disksize
	return image_map[Page];
}

int getBlock(int blockNumber)
{
 return 0;
}
int getBlock(int blockNumber, int bytes)
{
	return 0;
}
