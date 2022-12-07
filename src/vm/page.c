#include "vm/page.h"

struct supplemental_pte *get_supplemental_pte(void *vaddr)
{
    struct list *pt = &thread_current()->supplemental_pt;
    vaddr = pg_round_down(vaddr);

    for (struct list_elem *e = list_begin(pt); e != list_end(pt); e = list_next(e))
    {
        struct supplemental_pte *pte = list_entry(e, struct supplemental_pte, elem);
        if (pte->vaddr == vaddr)
        {
            return pte;
        }
        free(pte);
    }

    return NULL;
}

void set_supplemental_pte(struct file *file, void *vaddr, bool writeable, int offset, int read, int zero)
{
    struct supplemental_pte * entry;
    struct list *pt = &thread_current()->supplemental_pt;

	/* Create entry in supplemental table */
	entry = malloc(sizeof(struct supplemental_pte));
    entry->vaddr = vaddr;
	entry->fp = file;
    entry->type = PAGE_FILE;
    entry->fp_offset = offset;
    entry->read_bytes = read;
	entry->zero_bytes = zero;
	entry->fp_writable = writeable; 
    entry->loaded = false;

    struct list_elem *elem = &entry->elem;

	// insert element. 
	list_push_back(pt, elem);
}

void free_supplemental_pte(struct supplemental_pte *pte)
{
    struct thread *td = thread_current();
    lock_acquire(&td->supplemental_pt_lock);
    list_remove(&pte->elem);
    lock_release(&td->supplemental_pt_lock);

    uint8_t *page = pagedir_get_page(td->pagedir, pte->vaddr);
    pagedir_clear_page(td->pagedir, pte->vaddr);
    free_frame(page);
    free(pte);
}

bool load_page(struct supplemental_pte *pte)
{
    uint8_t *page = get_frame(PAL_USER);
    if (page == NULL)
        return false;

    switch (pte->type)
    {
    case PAGE_SWAP:
        // Map page to current thread's physical space
        if (!pagedir_set_page(thread_current()->pagedir, pte->vaddr, page, pte->swap_writable))
        {
            free_frame(page);
            return false;
        }

        swap_in(pte->swap_index, pte->vaddr);

        pte->type = PAGE_FILE;
        pte->loaded = true;
        return true;
        break;
    case PAGE_FILE:
        file_seek(pte->fp, pte->fp_offset);

        // Ensure the file offset is correct
        if (file_read(pte->fp, page, pte->read_bytes) != (int)pte->read_bytes)
        {
            free_frame(page);
            return false;
        }

        // Clear zero bytes of PTE
        memset(page + pte->read_bytes, 0, pte->zero_bytes);

        // Map page to current thread's physical space
        if (!pagedir_set_page(thread_current()->pagedir, pte->vaddr, page, pte->fp_writable))
        {
            free_frame(page);
            return false;
        }

        // PTE successfully loaded
        pte->loaded = true;
        return true;
        break;
    default:
        break;
    }

    return false;
}

void delete_supplemental_pte(void * vaddr){

    struct supplemental_pte * pte_to_remove; 

    struct list *pt = &thread_current()->supplemental_pt;
    for (struct list_elem *e = list_begin(pt); e != list_end(pt); e = list_next(e))
    {
        struct supplemental_pte *pte = list_entry(e, struct supplemental_pte, elem);
        if (pte->vaddr == vaddr)
        {
            pte_to_remove = pte;
            break;
        }
        free(pte);
    }

    struct list_elem *elem = &pte_to_remove->elem;
	list_remove(elem);
}

bool grow_stack(void *vaddr)
{
    void *page = get_frame(PAL_USER);
    if (page == NULL)
        return false;

    if (!pagedir_set_page(thread_current()->pagedir, pg_round_down(vaddr), page, true))
    {
        free_frame(page);
    }

    return true;
}


