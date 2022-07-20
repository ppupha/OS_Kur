#ifndef MYFS_H
#define MYFS_H

#define MYFS_MAGIC 0xDEADCELL

#define MYFS_SB_BLOCK_NR 0

#define MYFS_BLOCK_SIZE (1 << 12) /* 4 KiB */
#define MYFS_MAX_EXTENTS \
    MYFS_BLOCK_SIZE / sizeof(struct myfs_extent)
#define MYFS_MAX_BLOCKS_PER_EXTENT 8 /* It can be ~(uint32) 0 */
#define MYFS_MAX_FILESIZE                                      \
    (uint64_t) MYFS_MAX_BLOCKS_PER_EXTENT *MYFS_BLOCK_SIZE \
        *MYFS_MAX_EXTENTS
#define MYFS_FILENAME_LEN 28
#define MYFS_MAX_SUBFILES 128


struct myfs_inode {
    uint32_t i_mode;   /* File mode */
    uint32_t i_uid;    /* Owner id */
    uint32_t i_gid;    /* Group id */
    uint32_t i_size;   /* Size in bytes */
    uint32_t i_ctime;  /* Inode change time */
    uint32_t i_atime;  /* Access time */
    uint32_t i_mtime;  /* Modification time */
    uint32_t i_blocks; /* Block count */
    uint32_t i_nlink;  /* Hard links count */
    union {
        uint32_t ei_block;  /* Block with list of extents for this file */
        uint32_t dir_block; /* Block with list of files for this directory */
    };
    char i_data[32]; /* store symlink content */
};

#define MYFS_INODES_PER_BLOCK (MYFS_BLOCK_SIZE / sizeof(struct myfs_inode))

struct myfs_sb_info {
    uint32_t magic; /* Magic number */

    uint32_t nr_blocks; /* Total number of blocks (incl sb & inodes) */
    uint32_t nr_inodes; /* Total number of inodes */

    uint32_t nr_istore_blocks; /* Number of inode store blocks */
    uint32_t nr_ifree_blocks;  /* Number of inode free bitmap blocks */
    uint32_t nr_bfree_blocks;  /* Number of block free bitmap blocks */

    uint32_t nr_free_inodes; /* Number of free inodes */
    uint32_t nr_free_blocks; /* Number of free blocks */

#ifdef __KERNEL__
    unsigned long *ifree_bitmap; /* In-memory free inodes bitmap */
    unsigned long *bfree_bitmap; /* In-memory free blocks bitmap */
#endif
};

#ifdef __KERNEL__

struct myfs_inode_info {
    union {
        uint32_t ei_block;  /* Block with list of extents for this file */
        uint32_t dir_block; /* Block with list of files for this directory */
    };
    char i_data[32];
    struct inode vfs_inode;
};

struct myfs_extent {
    uint32_t ee_block; /* first logical block extent covers */
    uint32_t ee_len;   /* number of blocks covered by extent */
    uint32_t ee_start; /* first physical block extent covers */
};

struct myfs_file_ei_block {
    struct myfs_extent extents[MYFS_MAX_EXTENTS];
};

struct myfs_dir_block {
    struct myfs_file {
        uint32_t inode;
        char filename[MYFS_FILENAME_LEN];
    } files[MYFS_MAX_SUBFILES];
};

/* superblock functions */
int myfs_fill_super(struct super_block *sb, void *data, int silent);

/* inode functions */
int myfs_init_inode_cache(void);
void myfs_destroy_inode_cache(void);
struct inode *myfs_iget(struct super_block *sb, unsigned long ino);

/* file functions */
extern const struct file_operations myfs_file_ops;
extern const struct file_operations myfs_dir_ops;
extern const struct address_space_operations myfs_aops;

/* extent functions */
extern uint32_t myfs_ext_search(struct myfs_file_ei_block *index,
                                    uint32_t iblock);

/* Getters for superbock and inode */
#define MYFS_SB(sb) (sb->s_fs_info)
#define MYFS_INODE(inode) \
    (container_of(inode, struct myfs_inode_info, vfs_inode))

#endif /* __KERNEL__ */

#endif /* MYFS_H */
