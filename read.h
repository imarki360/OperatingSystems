#include <ext2fs/ext2_fs.h>
//#include <fstream>
//#include <iostream>

struct __attribute__((packed)) VirtualBox {
  char description[0x3f];
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

struct __attribute__((packed)) Superblock {

};

const __u16 BOOT_SECTOR_MAGIC = 0xaa55;

// an entry in the partition table. the unused entries really hold information,
// but the information is antiquated and not useful for our purposes here
struct PartitionEntry {
  __u8
    unused0[4],
    type,
    unused1[3];
  __u32
    firstSector,
    nSectors;
};

// the master boot record.
// the __attribute__((packed)) tells GCC to not add hidden padding between
// fields. normally, padding is added to improve data access (it's faster if
// data starts on 4- or 8-byte boundaries). without packing, BootSector is
// 516 bytes, which is 4 too many.
//
// note: you want entry 0 in partitionTable.
struct __attribute__((packed)) BootSector {
  __u8
    unused0[0x1be];
  PartitionEntry
    partitionTable[4];
  __u16
    magic;
};

/*
struct Reader
{
    Reader(std::ostream& os) : m_log(os.rdbuf()) { }

    std::streambuf* reset(std::ostream& os)
    {
        return m_log.rdbuf(os.rdbuf());
    }

    template <typename T> friend Reader& operator<<(Reader& os, const T& t)
    { os.m_log << t; return os; }

    friend Reader& operator<<(Reader& os, std::ostream& ( *pf )(std::ostream&))
    { os.m_log << pf; return os; }

  private:
    std::ostream m_log;
};
*/
