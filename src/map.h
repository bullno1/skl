#ifndef SICKLE_MAP_H
#define SICKLE_MAP_H

#include "common.h"
#include "gc.h"
#include "vendor/khash.h"


KHASH_DECLARE(skl_map, skl_value_t, skl_value_t)

struct skl_map_s
{
	skl_gc_header_t gc_header;
	khash_t(skl_map) entries;
};


skl_map_t*
skl_map_alloc(skl_ctx_t* ctx);

void
skl_map_int_set(skl_ctx_t* ctx, skl_map_t* map, skl_value_t key, skl_value_t value);

bool
skl_map_int_get(skl_ctx_t* ctx, skl_map_t* map, skl_value_t key, skl_value_t* value);

bool
skl_map_int_delete(skl_ctx_t* ctx, skl_map_t* map, skl_value_t key);

skl_index_t
skl_map_len(skl_map_t* map);

#endif
