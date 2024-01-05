#pragma once

#include "config.h"

#include "block.h"

// 节点类型
enum EntryType {
    ENTRY_FILE,                         // 文件
    ENTRY_DIRECTORY                     // 目录
};

// 节点
struct Entry {
    int block_id;
    char name[MAX_NAME_LENGTH];         // 文件名 / 目录名
    enum EntryType type;                // 类型 文件 / 目录
    off_t size;                         // 文件大小 / 目录文件数量
    time_t creation_time;               // 创建日期
    int first_block;
    int last_block;
};

// 目录
struct Directory {
    struct Entry entry;
    int parent;                         // 父目录所在盘块id
    int child_head;                     // 首个子节点所在盘块id
    int child_tail;                     // 尾个子节点所在盘块id
};

struct Block * to_block(struct MyFileSystem * fs, int block_id);

struct Directory * to_dirctory(struct MyFileSystem * fs, int block_id);

