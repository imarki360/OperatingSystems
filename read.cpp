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

#include "VirtualBox.cpp"
#include "ext2.cpp"

const int EXT2_SUPER_BLOCK_OFFSET = 1024;

int main(int argc, char** argv)
{
	try{
		/*
		 * Command line parser.
		*/

		if (argc == 1)
		{
			printf("You must specify a filename\n"
						 "Usage: read <filename>\n");
			exit(1);
		}
		char* pathname = argv[argc - 1];


		//char pathname[280] = "Debian1.0.vdi";
		//char pathname[280] = "Test-fixed-4k.vdi";
		//char pathname[280] = "Test-dynamic-1k.vdi";
		int file = open(pathname,O_RDONLY);
		if (file == -1)
			throw CouldNotReadFile();
		//int length = lseek(file,0,SEEK_END) + 1;

		VirtualBoxClass vb(file);
		ext2 ext2FileSystem(&vb);
		ext2FileSystem.verify_superblocks();
		ext2FileSystem.dump_blockgrouptables();
		ext2FileSystem.verify_inodes();
	}
	catch (CouldNotReadFile)
	{
		printf("Could not read in file, please check the filename entered.\n");
	}
	std::cout << std::endl;
	//close(file);
	return 0;
}
