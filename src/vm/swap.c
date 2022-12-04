#include "vm/swap.h"
#include "threads/malloc.h"

void swap_init(void)
{
    swap_block = block_get_role(BLOCK_SWAP);
    if (swap_block == NULL)
        PANIC("swap init failed");

    list_init(&swap_list);
    lock_init(&swap_list_lock);

    for (size_t i = 0; i < block_size(swap_block) / SECTORS_PER_PAGE; i++)
    {
        struct swap_entry *entry = malloc(sizeof(struct swap_entry));
        entry->in_memory = true;
        entry->index = i;

        list_push_back(&swap_list, &entry->elem);
    }
}

void *swap_in(size_t index, void *vaddr)
{
    struct swap_entry *to_swap = NULL;

    if (list_empty(&swap_list))
        return NULL;

    lock_acquire(&swap_list_lock);
    for (struct list_elem *e = list_begin(&swap_list); e != list_end(&swap_list); e = list_next(e))
    {
        struct swap_entry *entry = list_entry(e, struct swap_entry, elem);
        if (entry->index == index)
        {
            to_swap = entry;
            free(entry);
            break;
        }
        free(entry);
    }

    if (to_swap == NULL || to_swap->in_memory == true)
    {
        lock_release(&swap_list_lock);
        return;
    }

    to_swap->in_memory = true;

    for (size_t i = 0; i < SECTORS_PER_PAGE; i++)
    {
        block_read(swap_block, index * SECTORS_PER_PAGE + i, vaddr + i * BLOCK_SECTOR_SIZE);
    }

    lock_release(&swap_list_lock);
}

size_t swap_out(void *vaddr)
{
    if (list_empty(&swap_list))
        return -1;

    lock_acquire(&swap_list_lock);

    size_t swap_index = -1;
    for (struct list_elem *e = list_begin(&swap_list); e != list_end(&swap_list); e = list_next(e))
    {
        struct swap_entry *entry = list_entry(e, struct swap_entry, elem);
        if (entry->in_memory)
        {
            entry->in_memory = false;
            swap_index = entry->index;
            break;
        }
    }

    // No entries in memory
    if (swap_index == -1)
    {
        lock_release(&swap_list_lock);
        return swap_index;
    }

    for (size_t i = 0; i < SECTORS_PER_PAGE; i++)
    {
        block_write(swap_block, swap_index * SECTORS_PER_PAGE + i, vaddr + i * BLOCK_SECTOR_SIZE);
    }

    lock_release(&swap_list_lock);
    return swap_index;
}
