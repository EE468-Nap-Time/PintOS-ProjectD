#ifndef _VM_PAGE_H
#define _VM_PAGE_H

#include <string.h>
#include "filesys/file.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "vm/frame.h"
#include "vm/swap.h"

#define MAX_STACK_SIZE (8 * (1 << 20)) // 8 MB max stack size

typedef enum
{
    PAGE_SWAP,
    PAGE_FILE,
} PAGE_TYPE;

struct supplemental_pte
{
    void *vaddr;
    PAGE_TYPE type;
    struct list_elem elem;

    // PTE File
    struct file *fp;
    off_t fp_offset;
    uint32_t read_bytes;
    uint32_t zero_bytes;
    bool fp_writable;

    // Swap
    size_t swap_index;
    bool swap_writable;

    bool loaded;
};

struct supplemental_pte *get_supplemental_pte(void *);
void free_supplemental_pte(struct supplemental_pte *pte);
bool load_page(struct supplemental_pte *pte);
bool grow_stack(void *);

#endif // _VM_PAGE_H