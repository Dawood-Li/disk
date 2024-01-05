#pragma once

#include "config.h"
#include "block.h"
#include "entry.h"

// 创建文件夹
void mkdir(
    struct MyFileSystem * fs,
    const char * dir_name
);

struct MyFileSystem * filesystem_init();

// 进入文件夹
void change_dir(
    struct MyFileSystem * fs,
    const char * dir_name
);

// 输出当前路径 
void pwd(
    struct MyFileSystem * fs
);

// 删除空目录
void rmdir(
    struct MyFileSystem * fs,
    const char * dir_name
);

// 显示当前目录下文件和子目录列表
// 需要重新整理格式
void list_dir(
    struct MyFileSystem * fs
);

// 创建文件
// 需要重写
void create_file(
    struct MyFileSystem * fs,
    const char * file_name,
    size_t reserved_size
);

// 删除文件
void rmfile(
    struct MyFileSystem * fs,
    const char * file_name
);

// 文件/目录详情
void fileinfo(
    struct MyFileSystem * fs,
    const char * file_name
);
