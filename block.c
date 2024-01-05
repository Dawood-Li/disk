#include "block.h"

// 使用一个块
int block_pop(struct MyFileSystem * fs) {
    int blkid = fs->free_blocks;                            // 即将弹出的block_id
    if (blkid != BLOCK_NULL) {                              // 若空 返回BLOCK_NULL
        fs->free_blocks = fs->blocks[fs->free_blocks].next; // 出栈即将返回的盘块
        fs->blocks[blkid].next = BLOCK_NULL;                // 清理即将返回的盘块
        -- fs->free_blocks_size;                            // 计数
    }
    return blkid;                                           // 返回
}

// 释放一个块
void block_push(struct MyFileSystem * fs, int blkid) {
    fs->blocks[blkid].next = fs->free_blocks;               // 入栈
    fs->free_blocks = blkid;                                // 入栈
    ++ fs->free_blocks_size;                                // 计数
}

// 盘块信息
void block_info(struct MyFileSystem * fs) {
    printf("******** BLOCKS INFO ********\n");
    printf("* total block(s): %-9d *\n", BLOCK_NUM);
    printf("*  free block(s): %-9d *\n", fs->free_blocks_size);
    printf("*  used block(s): %-9d *\n", BLOCK_NUM - fs->free_blocks_size);
    printf("******** BLOCKS INFO ********\n");
}
