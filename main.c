// #include <stdio.h>
// #include <stdlib.h>                     
// #include <string.h>                     
// #include <time.h>                       // for restore creation_time
// #include <sys/types.h>                  // for off_t

// #define BLOCK_SIZE 1024                  // 盘块大小
// #define DATA_SIZE  1000                  // 数据块大小 = 盘块大小 - 头部数据
// #define BLOCK_NUM 1024                  // 盘块数量
// #define BLOCK_NULL BLOCK_NUM            // 盘块链表结尾

// #define MAX_NAME_LENGTH 256             // 文件名最大长度
// #define MAX_SUB_ENTRY_SIZE 32           // 最大子节点数量

// // 盘块
// struct Block {
//     int next;                           // 下个盘块
//     char data[DATA_SIZE];               // 盘块数据
// };

// // 文件系统
// struct MyFileSystem {
//     struct Block blocks[BLOCK_NUM];
//     int free_blocks;
//     int current_dir;
// };

// // 文件系统初始化
// void filesystem_erase(struct MyFileSystem * fs) {
//     for (int i = 0; i < BLOCK_NUM; i++) {
//         fs->blocks[i].next = i+1;
//     }
//     fs->free_blocks = 0;
// }

// // 使用一个块
// int block_pop(struct MyFileSystem * fs) {
//     int blkid = fs->free_blocks;                            // 即将弹出的block_id
//     if (blkid != BLOCK_NULL) {                              // 若空 返回BLOCK_NULL
//         fs->free_blocks = fs->blocks[fs->free_blocks].next; // 出栈即将返回的盘块
//         fs->blocks[blkid].next = BLOCK_NULL;                // 清理即将返回的盘块
//     }
//     return blkid;                                           // 返回
// }

// // 释放一个块
// void block_push(struct MyFileSystem * fs, int blkid) {
//     fs->blocks[blkid].next = fs->free_blocks;               // 入栈
//     fs->free_blocks = blkid;                                // 入栈
// }

// // 盘块信息
// void block_info(struct MyFileSystem * fs) {
//     int free_count = 0;
//     for (int i = fs->free_blocks; i != BLOCK_NULL; i = fs->blocks[i].next) {
//         ++ free_count;
//     }
//     printf("******** BLOCKS INFO ********\n");
//     printf("* total block(s): %-9d *\n", BLOCK_NUM);
//     printf("*  free block(s): %-9d *\n", free_count);
//     printf("*  used block(s): %-9d *\n", BLOCK_NUM - free_count);
//     printf("******** BLOCKS INFO ********\n");
// }

// // 节点类型
// enum EntryType {
//     ENTRY_FILE,                         // 文件
//     ENTRY_DIRECTORY                     // 目录
// } EntryType;

// // *****************************************************************************
// // *****************************************************************************
// // *****************************************************************************

// // 文件/路径节点
// struct Entry {
//     int block_id;                       // 当前节点所在盘块号
//     int parent_block_id;                // 父节点
//     int next_block_id;                  // 子节点链表 直到Block.next == BLOCK_NUM
//     char name[MAX_NAME_LENGTH];         // 文件名 / 目录名
//     enum EntryType type;                // 类型 文件 / 目录
//     time_t creation_time;               // 创建日期
//     off_t size;                         // 文件大小 / 目录文件数量
//     int sub_entry_blocks[128];
// };

// // 文件系统初始化
// void filesystem_init(struct MyFileSystem * fs) {
//     int block = block_pop(fs);
//     struct Entry * entry = (struct Entry *)fs->blocks[block].data;
//     strcpy(entry->name, "/");
//     entry->type = ENTRY_DIRECTORY;
//     entry->parent_block_id = block;
//     entry->next_block_id = BLOCK_NULL;
// }

// struct Entry * mkdir(
//     struct MyFileSystem * fs,
//     struct Entry * parent,
//     const char *dirname
// ) {
//     // 获取盘块
//     int block = block_pop(fs);
//     if (block == BLOCK_NULL) {
//         printf("Error: No free blocks available for creating a directory!\n");
//         return NULL;
//     }

//     // 在盘块上创建entry数据
//     struct Entry * entry = (struct Entry *)fs->blocks[block].data;
//     strncpy(entry->name, dirname, MAX_NAME_LENGTH);         // 目录名
//     entry->type = ENTRY_DIRECTORY;                          // 类型是目录
//     entry->block_id = block;                                // 自己所处的盘块
//     entry->parent_block_id = parent->block_id;              // 父块 用于回溯
    
//     // 附加到parent
//     parent->sub_entry_blocks[parent->size++] = block;       // 在父节点上追加子盘块
//     printf("%s new subdir %d %s\n", parent->name, parent->size, entry->name);
//     return entry;
// }

// // 格式待修改
// void entry_print(struct MyFileSystem *fs, struct Entry * entry, int depth) {
//     const char * type = entry->type == ENTRY_FILE ? "FILE" : "DIR";
//     for (int i = 0; i < depth; ++i) { printf("  "); }
//     printf("%s: %s size: %d\n", type, entry->name, entry->size);
// }

// void entry_traverse(
//     struct MyFileSystem * fs,
//     struct Entry * entry,
//     int depth
// ) {
//     entry_print(fs, entry, depth);

//     // 接下来是处理子目录的
//     if (entry->type != ENTRY_DIRECTORY || entry->size == 0) {
//         return;
//     }

//     // 递归处理所有子目录
//     for (int i = 0; i < entry->size; ++ i) {
//         struct Entry * sub_entry = (struct Entry *)fs->blocks[entry->sub_entry_blocks[i]].data;
//         entry_traverse(fs, sub_entry, depth + 1);
//     }
// }

// struct Entry * sub_dir(
//     struct MyFileSystem * fs,
//     struct Entry * entry,
//     const char * sub_dir_name
// ) {

//     for (int i = 0; i < entry->size; ++ i) {
//         struct Entry * sub_entry = (struct Entry *)fs->blocks[entry->sub_entry_blocks[i]].data;
//         if (strcmp(sub_entry->name, sub_dir_name) == 0) {
//             return sub_entry;
//         }
//     }

//     return NULL;
// }

// int main() {

//     struct MyFileSystem fs;
//     filesystem_erase(&fs);
//     filesystem_init(&fs);

//     struct Entry * root = (struct Entry *)fs.blocks[0].data;
//     mkdir(&fs, root, "FUCK YOU");
//     mkdir(&fs, root, "FUCK YOU2");
//     mkdir(&fs, root, "FUCK YOU3");
//     mkdir(&fs, root, "FUCK YOU4");
//     mkdir(&fs, root, "FUCK YOU");

//     struct Entry * fuckyou = sub_dir(&fs, root, "FUCK YOU");
//     if (fuckyou == NULL) {
//         printf("fuck you not found\n");
//     } else {
//         mkdir(&fs, fuckyou, "fuckfuckfuck!!!");
//         mkdir(&fs, fuckyou, "fuckfuckfuck1!!!");
//         mkdir(&fs, fuckyou, "fuckfuckfuck2!!!");
//         mkdir(&fs, fuckyou, "fuckfuckfuck3!!!");
//     }
    
//     block_info(&fs);
//     entry_traverse(&fs, root, 0);
// }
