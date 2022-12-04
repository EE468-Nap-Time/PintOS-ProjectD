#include "vm/frame.h"

void frame_table_init(void)
{
    list_init(&frame_table);
    lock_init(&frame_table_lock);
}

/* The frames used for user pages should be obtained from the "user pool," by calling palloc_get_page(PAL_USER).
 * You must use PAL_USER to avoid allocating from the "kernel pool," which could cause some test cases to fail unexpectedly
 */
void *get_frame(enum palloc_flags flags)
{
    void *page = palloc_get_page(flags);

    if (page != NULL)
    {
        // Add entry to frame table
        struct frame_table_entry *entry = malloc(sizeof(struct frame_table_entry));
        entry->page = page;
        entry->owner_td = thread_current();
        lock_acquire(&frame_table_lock);
        list_push_back(&frame_table, &entry->elem);
        lock_release(&frame_table_lock);
    }
    else
    {
        if (!evict_page())
            PANIC("Unable to evict frame");

        page = palloc_get_page(flags);
        // Add entry to frame table
        struct frame_table_entry *entry = malloc(sizeof(struct frame_table_entry));
        entry->page = page;
        entry->owner_td = thread_current();
        lock_acquire(&frame_table_lock);
        list_push_back(&frame_table, &entry->elem);
        lock_release(&frame_table_lock);
    }

    return page;
}

void free_frame(void *page)
{
    lock_acquire(&frame_table_lock);

    for (struct list_elem *e = list_begin(&frame_table); e != list_end(&frame_table); e = list_next(e))
    {
        struct frame_table_entry *entry = list_entry(e, struct frame_table_entry, elem);
        if (entry->page == page)
        {
            list_remove(e);
            free(entry);
            break;
        }
        free(entry);
    }

    palloc_free_page(page);
    lock_release(&frame_table_lock);
}

/* Evict page from frame. If no frame can be evicted without allocating a swap slot, but swap is full, panic the kernel.
 */
bool evict_page(void)
{
    if (list_size(&frame_table) == 0)
    {
        return false;
    }

    struct frame_table_entry *evict_entry = NULL;

    lock_acquire(&frame_table_lock);

    // Loop through frame table to find first page that is not accessed
    for (struct list_elem *e = list_begin(&frame_table); e != list_end(&frame_table); e = list_next(e))
    {
        struct frame_table_entry *entry = list_entry(e, struct frame_table_entry, elem);

        // If page is not accessed
        if (pagedir_is_accessed(entry->owner_td->pagedir, entry->page) == 0)
        {
            evict_entry = entry;
            free(entry);
            break;
        }
        free(entry);
    }

    // All pages were accessed, just evict first one in the list
    if (evict_entry == NULL)
    {
        evict_entry = list_entry(list_begin(&frame_table), struct frame_table_entry, elem);
    }

    evict_entry->owner_td = thread_current()->tid;
    evict_entry->page = NULL;

    lock_release(&frame_table_lock);

    return true;
}
