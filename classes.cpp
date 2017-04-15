#ifndef VIRTUALBOX
#define VIRTUALBOX

#include "classes.hpp"
#include <cstring>
#include <unistd.h>

VirtualBoxClass::VirtualBoxClass(int fd)
{
  lseek(fd, 0, SEEK_SET);
	read(fd, &header, sizeof(header));

  //Read in image map as 32-bit entries
  image_map = new __s32[header.disk_size_bytes >> 20]; //malloc(sizeof(__s32) * (header.disk_size_bytes >> 20));//
	read(fd, image_map, (header.disk_size_bytes >> 20));
}

void VirtualBoxClass::getBytes(char* data, int byteStart, int bytes)
{
  int byteDiff = bytes;
	int virtualPageStart = byteStart >> 20;
	int currentBytes = 0;
	int currentPage = virtualPageStart;

	while(byteDiff - currentBytes > 0)
	{

		int realPage = getRealPage(currentPage, header.disk_size_bytes);
		int pageOffset = header.offset_data + byteStart + ((realPage - currentPage) << 20);
		int bytesToRead = byteDiff - currentBytes;
		char* dataBuffer = new char[bytesToRead];//malloc(sizeof(char) * bytesToRead);

    //printf("Bytes to Read: 0x%x\nRealPage: 0x%x\npageOffset: 0x%x\nHeaderOffset: 0x%x\n\n",bytesToRead, realPage, pageOffset,header.offset_data);

		if (realPage == -1)
		{
			memset(dataBuffer,0, bytesToRead);
		}
		else
		{
			lseek(fd,pageOffset,SEEK_SET);
			read(fd, dataBuffer, bytesToRead);
		}
		for (int j = 0; j <= bytesToRead; j++)
		{
			data[currentBytes + j] = dataBuffer[j];
		}
		currentBytes += bytesToRead;
		currentPage++;
	}
	//seek to begining again
	lseek(fd, 0, SEEK_SET);
	//return 0;
}

int VirtualBoxClass::getRealPage(int page, int diskSize)
{
	//TODO: add check for out of disksize range
	return image_map[page];
}

#endif