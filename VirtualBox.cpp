#ifndef VIRTUALBOX
#define VIRTUALBOX

#include "classes.hpp"
#include <cstring>
#include <unistd.h>

VirtualBoxClass::VirtualBoxClass(int filedescriptor) : fd(filedescriptor)
{
  lseek(fd, 0, SEEK_SET);
	read(fd, &header, sizeof(header));

  //Read in image map as 32-bit entries
  lseek(fd, header.offset_blocks, SEEK_SET);
  printf("Header.offset_blocks: %x\n",header.offset_blocks);
  image_map = new __s32[header.disk_size_bytes >> 20]; //malloc(sizeof(__s32) * (header.disk_size_bytes >> 20));//
	read(fd, image_map, (header.disk_size_bytes >> 20));
}

void VirtualBoxClass::getBytes(char* data, int byteStart, int bytes)
{
  printf("byteStart: 0x%x\n", byteStart);
  int byteDiff = bytes;
	int virtualPageStart = byteStart >> 20;
  printf("virtualPageStart: %i\n", virtualPageStart);
	int currentBytes = 0;
	int currentPage = virtualPageStart;

	while(byteDiff - currentBytes > 0)
	{

		int realPage = getRealPage(currentPage);//, header.disk_size_bytes);
    int bytesToRead = byteDiff - currentBytes;
		char* dataBuffer = new char[bytesToRead];//malloc(sizeof(char) * bytesToRead);


		if (realPage == -1)
		{
			memset(dataBuffer,0, bytesToRead);
		}
		else
		{
      int pageOffset = header.offset_data + byteStart + ((realPage - currentPage) << 20);
      printf("Bytes to Read: 0x%x\nRealPage: 0x%x\ncurrentPage: 0x%x\npageOffset: 0x%x\nHeaderOffset: 0x%x\n\n",bytesToRead, realPage,currentPage, pageOffset,header.offset_data);

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

int VirtualBoxClass::getRealPage(int page)//, int diskSize)
{
	//TODO: add check for out of disksize range
  //printf("image_map[%i]: %i\n", page,image_map[page]);
	return image_map[page];
}

#endif
