#ifndef _VM_FRAME_H
#define _VM_FRAME_H

#include <list.h>
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/palloc.h"
#include "userprog/pagedir.h"
#include "vm/page.h"

struct list frame_table;
struct lock frame_table_lock;

struct frame_table_entry
{
    void *page;
    struct list_elem elem;
    struct thread *owner_td;
};

void frame_table_init(void);
void *get_frame(enum palloc_flags);
void free_frame(void *page);
bool evict_page(void);

#endif // _VM_FRAME_H
