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
	struct mark_ext2_super_block superblock;
	printf("size: %i\nsize: %i",sizeof(mark_ext2_super_block),sizeof(ext2_super_block));
	//read in superblock
	char* data_superblock = new char[sizeof(ext2_super_block)];
	getData(data_superblock, file, header,image_map, (boot.partitionTable[0].firstSector * header.sector_size) + EXT2_SUPER_BLOCK_OFFSET, sizeof(ext2_super_block));
	memcpy(&superblock, data_superblock, sizeof(ext2_super_block));
	delete data_superblock;
	printf("inode_count: %02x",superblock.s_inodes_count);

	/*
	 * Okay, using the superblock, we can now go thorugh the inodes and work thorugh the directory structure
	 */

	//loop through for each inode and place each inode into an array of structs
	struct ext2_inode *inodes[superblock.s_inodes_count];
	for (size_t i = 0; i < superblock.s_inodes_count; i++)
	{
		/* code */

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
		int pageOffset = header.offset_data + byteStart + (realPage - (currentPage << 20));
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
