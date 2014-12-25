typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long  uint32_t;

struct ext2_inode {
	uint16_t	i_mode;		/* File mode */
	uint16_t	i_uid;		/* Owner Uid */
	uint32_t	i_size;		/* Size in bytes */
	uint32_t	i_atime;	/* Access time */
	uint32_t	i_ctime;	/* Creation time */
	uint32_t	i_mtime;	/* Modification time */
	uint32_t	i_dtime;	/* Deletion Time */
	uint16_t	i_gid;		/* Group Id */
	uint16_t	i_links_count;	/* Links count */
	uint32_t	i_blocks;	/* Blocks count */
	uint32_t	i_flags;	/* File flags */
        uint32_t	dummy;
	uint32_t	i_block[15];    /* Pointers to blocks */
        uint32_t	pad[7];         /* inode size MUST be 128 bytes */
};

struct ext2_group_desc {
	uint32_t	bg_block_bitmap;	/* Blocks bitmap block */
	uint32_t	bg_inode_bitmap;	/* Inodes bitmap block */
	uint32_t	bg_inode_table;		/* Inodes table block */
	uint16_t	bg_free_blocks_count;	/* Free blocks count */
	uint16_t	bg_free_inodes_count;	/* Free inodes count */
	uint16_t	bg_used_dirs_count;	/* Directories count */
	uint16_t	bg_pad;
	uint32_t	bg_reserved[3];
};

struct ext2_dir_entry_2 {
	uint32_t	inode;			/* Inode number */
	uint16_t	rec_len;		/* Directory entry length */
	uint8_t		name_len;		/* Name length */
	uint8_t		file_type;
	uint8_t		name[255];      	/* File name */
};
