#ifndef _VM_SWAP_H
#define _VM_SWAP_H

#include <list.h>
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "devices/block.h"

#define SECTORS_PER_PAGE (PGSIZE / BLOCK_SECTOR_SIZE)

struct list swap_list;
struct lock swap_list_lock;

struct block *swap_block;

struct swap_entry
{
    struct list_elem elem;
    bool in_memory;
    size_t index;
};

void swap_init(void);
void *swap_in(size_t index, void *vaddr);
size_t swap_out(void *vaddr);

#endif // _VM_SWAP_H