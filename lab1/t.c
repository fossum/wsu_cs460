/********** t.c file ******************/

#include "ext2.h"

#define BLK 1024
#define INODE_BLOCK 5

typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

// printd (done)
// Prints unsigned int
/*void printd(uint16_t num) {
    if (num / 10) {
        printd(num / 10);
        putc((num % 10) + '0');
    } else {
        putc(num + '0');
    }
}*/

// Prints string (done)
void prints(uint8_t *s)
{
    while (*s)
    {
        putc(*s++);
    }
}

// getblk (done)
// Gets 1024 size block and places it in the buffer
uint8_t *getblk(uint16_t blk, uint8_t buf[])
{
    // convert blk to CHS; then call readfd(c,h,s, buf);
    //blk *= 2;
    //prints("Block:");
    //printd(buf);
    //getc();
    readfd(blk / 18, (blk / 9) % 2, (2 * blk) % 18, buf);
    return buf;
}

// gets (done)
// Get string from user
void gets(uint8_t s[])
{
    while(1) {
        *s = getc();
        putc(*s);
        if ( *s == '\r')
        {
            putc('\n');
            *s = '\0';
            break;
        }
        s++;
    }
}

// Loads inode number into buffer
INODE* load_inode(uint16_t inode, uint8_t buf[]) {
    getblk((--inode / 8) + INODE_BLOCK, buf);
    return ((INODE*)buf + (inode % 8));
}

// pathleft = remaining filepath string, inode = inode of directory
uint16_t find_file(uint8_t* pathleft, uint16_t inode)
{
    uint8_t buf2[BLK];
    uint16_t i;
    INODE *ip;
    uint8_t* cur_dir;

    cur_dir = (*pathleft == '/' ? ++pathleft : pathleft);
    
    // Increment next directory/file from pathleft
    while( (*pathleft != '/') && (*pathleft != '\0') ) {
        pathleft++;
    }
    if (*pathleft != '\0') { *(pathleft++) = '\0'; }

    // Load initial inode
    ip = load_inode(inode, buf2);
    
    i = 0;
    while (i < 13)
    {
        DIR* dir; // dir entry pointer
        uint16_t blockleft = BLK;
        uint16_t datablock = ip->i_block[i];
            
        // load block
        dir = getblk(datablock, buf2);
        
        // iterate 
        while(blockleft > 0 && dir->rec_len > 0) {
            // compare name to nextdir
            uint8_t* sp = dir->name;
            uint8_t* np = cur_dir;
            
            while (*sp == *np && dir->name_len > 0) {
                // Look for a match
                if (*(np+1) == '\0') {
                    if (*pathleft == '\0') {
                        return dir->inode;
                    } else {
                        return find_file(pathleft, dir->inode);
                    }
                }
                sp++;
                np++;
            }
                        
            // increment
            blockleft -= dir->rec_len;
            dir = (uint8_t*)dir + dir->rec_len;
        }
        i++;
    }
    return 0;
}

main()
{
    INODE *ip;
    uint32_t *blk_ptr;
    uint8_t i;
    uint32_t *blk_ptr;
    uint8_t main_buf[BLK], indirect_buf[BLK];

    // Get path
    prints("Boot>");
    gets(main_buf);

    // Find inode and load it
    ip = load_inode(find_file(main_buf, 2), main_buf);

    // Must save indirect block before jump
    getblk((uint16_t)ip->i_block[12], indirect_buf);
    blk_ptr = indirect_buf;
    
    // Move ES and begin loading data
    setes(0x1000);
    
    // Load direct blocks
    i = 0;
    while (i < 12){// && ip->i_block[i] != 0) {
        // Debugging
	//printd((uint16_t)ip->i_block[i]);
	//putc(' ');

        getblk((uint16_t)ip->i_block[i], 0);
        inces();
        i++;
    }
    
    // Load indirect blocks
    while (*blk_ptr != 0) {
        // Debugging
	    //printd((uint16_t)*blk_ptr);
	    //putc(' ');

        getblk((uint16_t)*blk_ptr, 0);
        inces();
        blk_ptr++;
    }
    
    return 1;
}

