#include "Cache.h"

Cache::Cache(void)
{
	this->size = 0;
	this->head = NULL;
	this->tail = NULL;
}

static void drop_display_list(fz_context *mu_ctx, void *item)
{
	fz_display_list *dlist = (fz_display_list*)item;
	fz_drop_display_list(mu_ctx, dlist);
}

static void keep_display_list(fz_context *mu_ctx, void *item)
{
	fz_display_list *dlist = (fz_display_list*)item;
	fz_keep_display_list(mu_ctx, dlist);
}

static void drop_page(fz_context *mu_ctx, void *item)
{
	fz_page *page = (fz_page*)item;
	fz_drop_page(mu_ctx, page);
}

static void keep_page(fz_context *mu_ctx, void *item)
{
	fz_page *page = (fz_page*)item;
	fz_keep_page(mu_ctx, page);
}

void Cache::CacheInit(cache_t type)
{
	switch (type)
	{
	case DISPLAY_LIST:
		max_size = MAX_DISPLAY_CACHE_SIZE;
		keep = keep_display_list;
		drop = drop_display_list;
		break;
	case PAGE:
		max_size = MAX_PAGE_CACHE_SIZE;
		keep = keep_page;
		drop = drop_page;
		break;
	}
}

Cache::~Cache(void)
{
}

void Cache::Empty(fz_context *mu_ctx)
{
	if (this != nullptr) {
		cache_entry_t *curr_entry = this->head;

		while (curr_entry != NULL)
		{
			cache_entry_t *old_entry = curr_entry;
			curr_entry = old_entry->next;
			drop(mu_ctx, old_entry->item);
			delete old_entry;
		}
		this->size = 0;
		this->head = NULL;
		this->tail = NULL;
	}
}

void Cache::Add(int value, int width_in, int height_in, void *item, 
				fz_context *mu_ctx)
{
	std::lock_guard<std::mutex> lock(cache_lock);

	/* If full, then delete the tail */
	if (size >= max_size && max_size > 0)
	{
		cache_entry_t *curr_entry = this->tail;
		cache_entry_t *prev_entry = curr_entry->prev;

		if (prev_entry != NULL)
			prev_entry->next = NULL;
		else
			head = NULL;

		tail = prev_entry;

		/* Decrement the caches rc of this list. It is gone from cache but
		   may still be in use by other threads, when threads are done they 
		   should decrement and it should be freed at that time. See 
		   ReleaseDisplayLists in muctx class */
		drop(mu_ctx, curr_entry->item);
		delete curr_entry;
		size--;
	}

	/* Make a new entry and stick at head */
	cache_entry_t *new_entry = new cache_entry_t;

	new_entry->item = item;
	new_entry->index = value;
	new_entry->width = width_in;
	new_entry->height = height_in;
	new_entry->prev = NULL;
	if (head == NULL)
	{
		new_entry->next = NULL;
		head = new_entry;
		tail = new_entry;
	}
	else
	{
		new_entry->next = head;
		head->prev = new_entry;
		head = new_entry;
	}
	size++;
	/* Everytime we add an entry, we are also using it. Increment rc. See above */
	keep(mu_ctx, item);
}

/* Drop an entry as it is no longer valid.  Used for cases where annotation changes
	have occured and we can't use the cached version any longer */
void Cache::Drop(int value, fz_context *mu_ctx)
{
	std::lock_guard<std::mutex> lock(cache_lock);
	cache_entry_t *curr_entry = this->head;
	while (curr_entry != NULL)
	{
		if (curr_entry->index == value)
			break;
		curr_entry = curr_entry->next;
	}
	if (curr_entry == NULL)
		return;

	cache_entry_t *next_entry = curr_entry->next;
	cache_entry_t *prev_entry = curr_entry->prev;

	if (next_entry != NULL)
		next_entry->prev = prev_entry;

	if (prev_entry != NULL)
		prev_entry->next = next_entry;

	/* Take care of head and tail issues */
	if (curr_entry == head)
		head = next_entry;
	if (curr_entry == tail)
		tail = prev_entry;

	size--;
	drop(mu_ctx, curr_entry->item);
}

void* Cache::Use(int value, int *width_out, int *height_out, fz_context *mu_ctx)
{
	std::lock_guard<std::mutex> lock(cache_lock);
	cache_entry_t *curr_entry = this->head;

	while (curr_entry != NULL)
	{
		if (curr_entry->index == value)
			break;
		curr_entry = curr_entry->next;
	}
	if (curr_entry != NULL)
	{
		/* Move this to the front */
		if (curr_entry != head)
		{
			cache_entry_t *prev_entry = curr_entry->prev;
			cache_entry_t *next_entry = curr_entry->next;
			prev_entry->next = next_entry;

			if (next_entry != NULL)
				next_entry->prev = prev_entry;
			else
				tail = prev_entry;

			curr_entry->prev = NULL;
			curr_entry->next = head;
			head->prev = curr_entry;
			head = curr_entry;
		}
		*width_out = curr_entry->width;
		*height_out = curr_entry->height;
		/* We must increment our reference to this one to ensure it is not 
		   freed when removed from the cache. See above comments */
		keep(mu_ctx, curr_entry->item);
		return curr_entry->item;
	}
	else
		return NULL;
}
