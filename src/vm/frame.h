#ifndef _VM_FRAME_H
#define _VM_FRAME_H

#include <list.h>
#include "threads/thread.h"
#include "threads/palloc.h"
#include "userprog/pagedir.h"

struct list frame_table;
struct lock frame_table_lock;

struct frame_table_entry
{
    void *page;
    struct list_elem elem;
    struct thread *owner_td;
};

void frame_table_init();
void *get_frame();
void free_frame(void *page);
bool evict_page();

#endif // _VM_FRAME_H
