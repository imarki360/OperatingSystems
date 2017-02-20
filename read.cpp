#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <ext2fs/ext2_fs.h>
#include "read.h"
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <stdlib.h>
#include <iomanip>

int getRealPage(__s32 image_map[], int Page, int diskSize);
//template <class T>
int getData(char* data, int fd, VirtualBox header, __s32 image_map[], int byteStart, int bytes, int arraySize);

int main()
{
	struct VirtualBox header;
	struct BootSector boot;
	char pathname[280] = "Debian1.0.vdi";
	int file = open(pathname,O_RDONLY);
	int length = lseek(file,0,SEEK_END) + 1;

	//printf("%d", length);
	lseek(file, 0, SEEK_SET);
	//char* buf = new char[length];
	read(file, &header, sizeof(header));
	//printf("%x\n%x\n\n", header.offset_data, 4 * (header.disk_size_bytes >> 20));
	lseek(file, header.offset_blocks, SEEK_SET);
	//Read in image map 32-bit entries
	__s32* image_map = new __s32[header.disk_size_bytes >> 20]; //malloc(sizeof(__s32) * (header.disk_size_bytes >> 20));//
	read(file,image_map, (header.disk_size_bytes >> 20));

	int arraySize = 512;//sizeof(512);
	char* data = new char[512];//malloc(sizeof(char) * length);

	getData(data, file, header, image_map, 0, 512, 512);

  for (int i = 0; i < 512; i++)
  {
    printf("%x\t",data[i]);
  }
	memcpy(&boot, data, arraySize);
	printf("%x\n", boot.magic);

	return 0;
}

//template <class T>
int getData(char* data, int fd, VirtualBox header, __s32 image_map[], int byteStart, int bytes, int arraySize)
{

	int byteDiff = bytes - byteStart;
	int virtualPageStart = byteStart >> 20;
	int currentBytes = 0;
	int currentPage = virtualPageStart;

	while(byteDiff - currentBytes > 0)
	{
		int realPage = getRealPage(image_map, currentPage, header.disk_size_bytes);
		int pageOffset = header.offset_data + byteStart;
		int bytesToRead = byteDiff - currentBytes;
		char* dataBuffer = new char[bytesToRead];//malloc(sizeof(char) * bytesToRead);

		if (realPage << 20 >= 0)
		{
			pageOffset += (realPage << 20) + byteStart;
			lseek(fd,pageOffset,SEEK_SET);
			read(fd, dataBuffer, bytesToRead);
		}
		else if (realPage == -1)
		{
			memset(dataBuffer,0, bytesToRead);
		}
		for (int j = 0; j < bytesToRead; j++)
		{
			data[currentBytes + j] = dataBuffer[j];
		}
		currentBytes += bytesToRead;
		currentPage++;
	}

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
