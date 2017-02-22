#pragma once

#include <mutex>
extern "C" {
	#include "mupdf/fitz.h"
}

#define MAX_DISPLAY_CACHE_SIZE 3
#define MAX_PAGE_CACHE_SIZE 0

typedef enum {
	DISPLAY_LIST,
	PAGE
} cache_t;

/* Generic procs so that we can use the cache for different types (e.g. page
 or display list) */
typedef void(*drop_item) (fz_context *mu_ctx, void *item);
typedef void(*keep_item) (fz_context *mu_ctx, void *item);

typedef struct cache_entry_s cache_entry_t;

struct cache_entry_s
{
	int number;
	int width;
	int height;
	void *item;
	cache_entry_t *next;
	cache_entry_t *prev;
	int index;
};

class Cache
{
private:
	int size;
	cache_entry_t *head;
	cache_entry_t *tail;
	std::mutex cache_lock;
	cache_t type;
	int max_size;
	keep_item keep;
	drop_item drop;

public:
	Cache(void);
	~Cache(void);
	void CacheInit(cache_t type);
	void* Use(int value, int *width, int *height, fz_context *mu_ctx);
	void Add(int value, int width, int height, void *item, fz_context *mu_ctx);
	void Empty(fz_context *mu_ctx);
	void Drop(int value, fz_context *mu_ctx);
};
