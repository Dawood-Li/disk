#include "entry.h"

struct Block * to_block(struct MyFileSystem * fs, int block_id) {
    return (struct Block*)&fs->blocks[block_id];
}

struct Directory * to_dirctory(struct MyFileSystem * fs, int block_id) {
    return (struct Directory *)fs->blocks[block_id].data;
}
