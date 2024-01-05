#pragma once

#include "config.h"

// 盘块
struct Block {
    int next;                           // 下个盘块
    char data[DATA_SIZE];               // 盘块数据
};

// 文件系统
struct MyFileSystem {
    struct Block blocks[BLOCK_NUM];
    int free_blocks;
    int current_dir;
    int free_blocks_size;
};

// 使用一个块
int block_pop(struct MyFileSystem * fs);

// 释放一个块
void block_push(struct MyFileSystem * fs, int blkid);

// 盘块信息
void block_info(struct MyFileSystem * fs);

