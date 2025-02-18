#ifndef VIRTUALBOX_H
#define VIRTUALBOX_H

#include "ext2_fs.h"

class VirtualBoxClass {
private:
  struct __attribute__((packed)) VirtualBoxHeader {
    char description[0x40];
    __u32 image_signiture;
    __u32 version;
    __u32 header_size;
    __u32 image_type;
    char stuff[0x104];
    __u32 offset_blocks;
    __u32 offset_data;
    __u32 cylinders;
    __u32 heads;
    __u32 sectors;
    __u32 sector_size;
    __u32 garbage;
    __u64 disk_size_bytes;
    __u32 block_size;
    __u32 block_extra_data;
    __u32 blocks_in_hdd;
    __u32 blocks_allocated;
    __u32 uuid_vdi[4];
    __u32 uuid_link[4];
    __u32 uuid_parent[4];
  };
  int fd;
  __s32* image_map;

public:
  struct VirtualBoxHeader header;

  VirtualBoxClass(int fd);
  void getBytes(char* data, int byteStart, int bytes);
  int getRealPage(int page);//, int diskSize);
};

#endif
