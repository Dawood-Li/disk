#pragma once

#include <stdio.h>
#include <stdlib.h>                     
#include <string.h>                     
#include <time.h>                       // for restore creation_time
#include <sys/types.h>                  // for off_t

#define BLOCK_SIZE 512                  // 盘块大小
#define DATA_SIZE  508                  // 数据块大小 = 盘块大小 - 头部数据
#define BLOCK_NUM 4096                  // 盘块数量
#define BLOCK_NULL BLOCK_NUM            // 盘块链表结尾

#define MAX_NAME_LENGTH 256             // 文件名最大长度
