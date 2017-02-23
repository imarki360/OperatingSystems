#include <ext2fs/ext2_fs.h>
//#include <fstream>
//#include <iostream>

struct __attribute__((packed)) VirtualBox {
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

struct __attribute__((packed)) mark_ext2_super_block {
	__u32	s_inodes_count;		/* Inodes count */
	__u32	s_blocks_count;		/* Blocks count */
	__u32	s_r_blocks_count;	/* Reserved blocks count */
	__u32	s_free_blocks_count;	/* Free blocks count */
	__u32	s_free_inodes_count;	/* Free inodes count */
	__u32	s_first_data_block;	/* First Data Block */
	__u32	s_log_block_size;	/* Block size */
	__u32	s_log_cluster_size;	/* Allocation cluster size */
	__u32	s_blocks_per_group;	/* # Blocks per group */
	__u32	s_clusters_per_group;	/* # Fragments per group */
	__u32	s_inodes_per_group;	/* # Inodes per group */
	__u32	s_mtime;		/* Mount time */
	__u32	s_wtime;		/* Write time */
	__u16	s_mnt_count;		/* Mount count */
	__s16	s_max_mnt_count;	/* Maximal mount count */
	__u16	s_magic;		/* Magic signature */
	__u16	s_state;		/* File system state */
	__u16	s_errors;		/* Behaviour when detecting errors */
	__u16	s_minor_rev_level;	/* minor revision level */
	__u32	s_lastcheck;		/* time of last check */
	__u32	s_checkinterval;	/* max. time between checks */
	__u32	s_creator_os;		/* OS */
	__u32	s_rev_level;		/* Revision level */
	__u16	s_def_resuid;		/* Default uid for reserved blocks */
	__u16	s_def_resgid;		/* Default gid for reserved blocks */
	/*
	 * These fields are for EXT2_DYNAMIC_REV superblocks only.
	 *
	 * Note: the difference between the compatible feature set and
	 * the incompatible feature set is that if there is a bit set
	 * in the incompatible feature set that the kernel doesn't
	 * know about, it should refuse to mount the filesystem.
	 *
	 * e2fsck's requirements are more strict; if it doesn't know
	 * about a feature in either the compatible or incompatible
	 * feature set, it must abort and not try to meddle with
	 * things it doesn't understand...
	 */
	__u32	s_first_ino;		/* First non-reserved inode */
	__u16   s_inode_size;		/* size of inode structure */
	__u16	s_block_group_nr;	/* block group # of this superblock */
	__u32	s_feature_compat;	/* compatible feature set */
	__u32	s_feature_incompat;	/* incompatible feature set */
	__u32	s_feature_ro_compat;	/* readonly-compatible feature set */
	__u8	s_uuid[16];		/* 128-bit uuid for volume */
	char	s_volume_name[16];	/* volume name */
	char	s_last_mounted[64];	/* directory where last mounted */
	__u32	s_algorithm_usage_bitmap; /* For compression */
	/*
	 * Performance hints.  Directory preallocation should only
	 * happen if the EXT2_FEATURE_COMPAT_DIR_PREALLOC flag is on.
	 */
	__u8	s_prealloc_blocks;	/* Nr of blocks to try to preallocate*/
	__u8	s_prealloc_dir_blocks;	/* Nr to preallocate for dirs */
	__u16	s_reserved_gdt_blocks;	/* Per group table for online growth */
	/*
	 * Journaling support valid if EXT2_FEATURE_COMPAT_HAS_JOURNAL set.
	 */
	__u8	s_journal_uuid[16];	/* uuid of journal superblock */
	__u32	s_journal_inum;		/* inode number of journal file */
	__u32	s_journal_dev;		/* device number of journal file */
	__u32	s_last_orphan;		/* start of list of inodes to delete */
	__u32	s_hash_seed[4];		/* HTREE hash seed */
	__u8	s_def_hash_version;	/* Default hash version to use */
	__u8	s_jnl_backup_type; 	/* Default type of journal backup */
	__u16	s_desc_size;		/* Group desc. size: INCOMPAT_64BIT */
	__u32	s_default_mount_opts;
	__u32	s_first_meta_bg;	/* First metablock group */
	__u32	s_mkfs_time;		/* When the filesystem was created */
	__u32	s_jnl_blocks[17]; 	/* Backup of the journal inode */
	__u32	s_blocks_count_hi;	/* Blocks count high 32bits */
	__u32	s_r_blocks_count_hi;	/* Reserved blocks count high 32 bits*/
	__u32	s_free_blocks_hi; 	/* Free blocks count */
	__u16	s_min_extra_isize;	/* All inodes have at least # bytes */
	__u16	s_want_extra_isize; 	/* New inodes should reserve # bytes */
	__u32	s_flags;		/* Miscellaneous flags */
	__u16   s_raid_stride;		/* RAID stride */
	__u16   s_mmp_update_interval;  /* # seconds to wait in MMP checking */
	__u64   s_mmp_block;            /* Block for multi-mount protection */
	__u32   s_raid_stripe_width;    /* blocks on all data disks (N*stride)*/
	__u8	s_log_groups_per_flex;	/* FLEX_BG group size */
	__u8    s_reserved_char_pad;
	__u16	s_reserved_pad;		/* Padding to next 32bits */
	__u64	s_kbytes_written;	/* nr of lifetime kilobytes written */
	__u32	s_snapshot_inum;	/* Inode number of active snapshot */
	__u32	s_snapshot_id;		/* sequential ID of active snapshot */
	__u64	s_snapshot_r_blocks_count; /* reserved blocks for active
					      snapshot's future use */
	__u32	s_snapshot_list;	/* inode number of the head of the on-disk snapshot list */
#define EXT4_S_ERR_START ext4_offsetof(struct ext2_super_block, s_error_count)
	__u32	s_error_count;		/* number of fs errors */
	__u32	s_first_error_time;	/* first time an error happened */
	__u32	s_first_error_ino;	/* inode involved in first error */
	__u64	s_first_error_block;	/* block involved of first error */
	__u8	s_first_error_func[32];	/* function where the error happened */
	__u32	s_first_error_line;	/* line number where error happened */
	__u32	s_last_error_time;	/* most recent time of an error */
	__u32	s_last_error_ino;	/* inode involved in last error */
	__u32	s_last_error_line;	/* line number where error happened */
	__u64	s_last_error_block;	/* block involved of last error */
	__u8	s_last_error_func[32];	/* function where the error happened */
#define EXT4_S_ERR_END ext4_offsetof(struct ext2_super_block, s_mount_opts)
	__u8	s_mount_opts[64];
	__u32	s_usr_quota_inum;	/* inode number of user quota file */
	__u32	s_grp_quota_inum;	/* inode number of group quota file */
	__u32	s_overhead_blocks;	/* overhead blocks/clusters in fs */
	__u32	s_backup_bgs[2];	/* If sparse_super2 enabled */
	__u32   s_reserved[106];        /* Padding to the end of the block */
	__u32	s_checksum;		/* crc32c(superblock) */
};
