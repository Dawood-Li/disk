#include "commands.h"
#include "entry.h"

#include <time.h>

// 选择子目录 辅助函数
int select_dir(
    struct MyFileSystem * fs,
    struct Directory * current_dir,
    const char * dir_name
) {
    if (strcmp(dir_name, "/") == 0) {
        return 0;
    }

    if (strcmp(dir_name, ".") == 0) {
        return current_dir->entry.block_id;
    }

    if (strcmp(dir_name, "..") == 0) {
        return current_dir->parent;
    }

    for (
        int block = current_dir->child_head;
        block != BLOCK_NULL;
        block = to_block(fs, block)->next
    ) {
        struct Directory * dir = to_dirctory(fs, block);
        if (strcmp(dir->entry.name, dir_name) == 0) {
            return block;
        }
        if (block == current_dir->child_tail) {
            // return BLOCK_NULL;
            return current_dir->entry.block_id;
        }
    }
    return current_dir->entry.block_id;
}

// 创建文件夹
void mkdir(
    struct MyFileSystem * fs,
    const char * dir_name
) {
    // 重名检查
    if (select_dir(fs, to_dirctory(fs, fs->current_dir), dir_name) != fs->current_dir) {
        printf("Error: %s exist.\n");
        return;
    }

    // 获取盘块
    int block = block_pop(fs);
    if (block == BLOCK_NULL) {
        printf("Error: No free blocks available for creating a directory!\n");
        return;
    }

    // 转换指针类型 方便后续操作
    struct Directory * current_dir = to_dirctory(fs, fs->current_dir);
    struct Directory * new_dir = to_dirctory(fs, block);
    struct Entry * entry = &new_dir->entry;

    // 新目录无子节点 无子行不行？行！
    new_dir->child_head = BLOCK_NULL;
    new_dir->child_tail = BLOCK_NULL;

    entry->block_id = block;
    strncpy(entry->name, dir_name, MAX_NAME_LENGTH);
    entry->type = ENTRY_DIRECTORY;
    entry->size = 0;
    entry->creation_time = time(NULL);

    // 文件夹没有盘块
    entry->first_block = BLOCK_NULL;
    entry->last_block  = BLOCK_NULL;
    
    // 创建根目录 其无父目录 无需处理
    if (strcmp(dir_name, "/") == 0) {
        new_dir->parent = BLOCK_NULL;
    }
    
    // 创建非根目录 需要处理父目录关系
    else {
        new_dir->parent = current_dir->entry.block_id;
        
        // 没有子节点 将head和tail都指向block
        if (current_dir->child_head == BLOCK_NULL) {
            current_dir->child_head = block;
            current_dir->child_tail = block;
        }
        
        // 有子节点 将tail.next指向block 并将tail指向block
        else {
            to_block(fs, current_dir->child_tail)->next = block;
            current_dir->child_tail = block;
        }

        ++ current_dir->entry.size;
    }
}

// 文件系统初始化
struct MyFileSystem * filesystem_init() {

    // 申请内存
    struct MyFileSystem * fs = malloc(sizeof(struct MyFileSystem));

    // 将所有盘块整理成静态栈
    for (int i = 0; i < BLOCK_NUM; i++) {
        fs->blocks[i].next = i+1;                           // 每个盘块都指向下一盘块
    }
    fs->free_blocks = 0;                                    // 空闲盘块栈顶
    fs->free_blocks_size = BLOCK_NUM;                       // 空闲盘块数量
    fs->current_dir = 0;                                    // 当前目录

    // 创建根目录
    mkdir(fs, "/");
    return fs;
}

// 进入文件夹
void change_dir(
    struct MyFileSystem * fs,
    const char * dir_name
) {

    // 存在检查
    int block = select_dir(fs, to_dirctory(fs, fs->current_dir), dir_name);
    
    printf("block: %d\n", block);
    
    if (block == fs->current_dir) {
        printf("Error %s: no such file or directory\n", dir_name);
        return;
    }

    // 类型检查
    if (to_dirctory(fs, block)->entry.type != ENTRY_DIRECTORY) {
        printf("Error %s: is not a directory.\n", dir_name);
        return;
    }

    fs->current_dir = block;
}

// 输出当前路径 辅助递归函数
void recursion_pwd(
    struct MyFileSystem * fs,
    struct Directory * current_dir
) {
    if (current_dir->parent != BLOCK_NULL) {
        recursion_pwd(fs, to_dirctory(fs, current_dir->parent));
        printf("/");
    }
    if (current_dir->parent == BLOCK_NULL) {
        return;
    }
    
    printf("%s", current_dir->entry.name);
}

// 输出当前路径 
void pwd(struct MyFileSystem * fs) {
    recursion_pwd(fs, to_dirctory(fs, fs->current_dir));
    printf("/");
}

// 删除空目录
void rmdir(
    struct MyFileSystem * fs,
    const char * dir_name
) {
    int block = select_dir(fs, to_dirctory(fs, fs->current_dir), dir_name);
    if (block == BLOCK_NULL) {
        printf("Error %s: no such file or directory\n", dir_name);
        return;
    }

    // 转换指针类型 方便后续操作
    struct Directory * current_dir = to_dirctory(fs, fs->current_dir);
    struct Directory * sub_dir = to_dirctory(fs, block);
    struct Entry * entry = &sub_dir->entry;

    // 非目录无法执行rmdir
    if (entry->type != ENTRY_DIRECTORY) {
        printf("Error %s: not a directory.\n", dir_name);
        return;
    }

    // 目录非空无法执行rmdir
    if (entry->size != 0) {
        printf("Error %s: not empty.\n", dir_name);
        return;
    }

    // 链表中只有一个元素
    if (current_dir->entry.size == 1) {
        current_dir->child_head = BLOCK_NULL;
        current_dir->child_tail = BLOCK_NULL;
    }

    // 其是链表头部
    else if (block == current_dir->child_head) {
        current_dir->child_head = to_block(fs, block)->next;
    }

    // 其在链表中间
    // 能搜到block就一定有
    // 暴力判断无需谨慎
    else {
        int pre = current_dir->child_head;
        while (pre != BLOCK_NULL && to_block(fs, pre)->next != BLOCK_NULL) {
            if (to_block(fs, pre)->next == block) {
                to_block(fs, pre)->next = to_block(fs, to_block(fs, pre)->next)->next;
                break;
            }
        }
    }

    // 归还空间
    -- current_dir->entry.size;
    block_push(fs, block);
}

// 显示当前目录下文件和子目录列表
// 需要重新整理格式
void list_dir(
    struct MyFileSystem * fs
) {
    struct Directory * current_dir = to_dirctory(fs, fs->current_dir);

    int block = current_dir->child_head;
    while (block != BLOCK_NULL) {
        struct Directory * dir_entry = to_dirctory(fs, block);
        struct Entry * entry = &dir_entry->entry;

        if (entry->type == ENTRY_FILE) {
            printf("  - %s [file]\n", entry->name);
        } else if (entry->type == ENTRY_DIRECTORY) {
            printf("  - %s [directory]\n", entry->name);
        } else {
            printf("  - %s [???]\n", entry->name);
        }

        block = to_block(fs, block)->next;
    }
}

// 创建文件
void create_file(
    struct MyFileSystem * fs,
    const char * file_name,
    size_t reserved_size
) {

    // 检查重名
    if (select_dir(fs, to_dirctory(fs, fs->current_dir), file_name) != fs->current_dir) {
        printf("Error: %s exist.\n");
        return;
    }

    // 检查空间是否足够
    if (reserved_size > (fs->free_blocks_size - 1) * DATA_SIZE) {
        printf("not enough free blocks.\n");
        return;
    }

    // 元数据块
    int meta_block = block_pop(fs);
    
    // 转换指针类型方便后续操作
    struct Directory * current_dir = to_dirctory(fs, fs->current_dir);
    struct Directory * sub_dir = to_dirctory(fs, meta_block);
    struct Entry * entry = &sub_dir->entry;

    // 初始化元数据
    entry->block_id = meta_block;
    strncpy(entry->name, file_name, MAX_NAME_LENGTH);
    entry->type = ENTRY_FILE;
    entry->size = reserved_size;
    entry->creation_time = time(NULL);

    entry->first_block = BLOCK_NULL; // 初始化
    entry->last_block  = BLOCK_NULL;

    int blocks_needed = (reserved_size + DATA_SIZE - 1) / DATA_SIZE;

    // 这是一段难以解释的神秘操作
    // 其功能是用于建立动态数量的单链表 并插入到meta block中
    // 其复杂的逻辑是为了保证插入0个 1个 n个 时同样有效
    if (blocks_needed) {
        entry->first_block = block_pop(fs);
        entry->last_block = entry->first_block;
        int current_blk = entry->first_block;
        while (--blocks_needed > 0) {
            int next_blk = block_pop(fs);
            to_block(fs, current_blk)->next = next_blk;
            current_blk = next_blk;
            entry->last_block = current_blk;
        }
    }

    // 将文件节点添加到当前目录的子节点列表
    if (current_dir->child_head == BLOCK_NULL) {
        current_dir->child_head = meta_block;
        current_dir->child_tail = meta_block;
    } else {
        to_block(fs, current_dir->child_tail)->next = meta_block;
        current_dir->child_tail = meta_block;
    }
}


// 删除文件
void rmfile(
    struct MyFileSystem * fs,
    const char * file_name
) {
    printf("remove file %s\n", file_name);
    
    struct Directory * current_dir = to_dirctory(fs, fs->current_dir);

    int block = select_dir(fs, current_dir, file_name);
    if (block == BLOCK_NULL) {
        printf("%s: no such file or directory\n", file_name);
        return;
    }

    // 转换指针类型 方便后续操作
    struct Directory * dir_entry = to_dirctory(fs, block);
    struct Entry * entry = &dir_entry->entry;

    // 检查是否为文件
    if (entry->type != ENTRY_FILE) {
        printf("%s: not a file.\n", file_name);
        return;
    }

    // 释放所有数据块
    int blk_to_free = entry->first_block;
    while (blk_to_free != BLOCK_NULL) {
        int next_blk = to_block(fs, blk_to_free)->next;
        block_push(fs, blk_to_free);
        blk_to_free = next_blk;
    }

    // 从子节点列表中移除并释放元数据块
    int pre = BLOCK_NULL; // 声明并初始化 pre
    if (block == current_dir->child_head) {
        current_dir->child_head = to_block(fs, block)->next;
    } else {
        pre = current_dir->child_head;
        while (to_block(fs, pre)->next != block) {
            pre = to_block(fs, pre)->next;
        }
        to_block(fs, pre)->next = to_block(fs, block)->next;
    }

    if (block == current_dir->child_tail) {
        current_dir->child_tail = pre;
    }

    -- current_dir->entry.size;
    block_push(fs, block);
}

// 文件/目录详情
void fileinfo(
    struct MyFileSystem * fs,
    const char * file_name
) {
    // 文件存在检查
    struct Directory * current_dir = to_dirctory(fs, fs->current_dir);
    int block = select_dir(fs, current_dir, file_name);
    if (block == current_dir->entry.block_id) {
        printf("%s: no such file or directory.\n", file_name);
        return;
    }

    // 转换指针类型 方便后续操作
    struct Directory * dir = to_dirctory(fs, select_dir(fs, current_dir, file_name));
    struct Entry * entry = &dir->entry;

    printf("block id: %d\n", entry->block_id);
    printf("name: %s\n", entry->name);
    const char * type = entry->type == ENTRY_FILE ? "FILE" : "DIR";
    printf("type: %s\n", type);
    printf("size: %d\n", entry->size);

    struct tm * creation_tm = localtime(&entry->creation_time);
    char time_buf[30];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", creation_tm);
    printf("creation time: %s\n", time_buf);
}
