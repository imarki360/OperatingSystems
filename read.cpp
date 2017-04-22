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
	ext2 ext2FileSystem(&vb);

	//close(file);
	return 0;
}
