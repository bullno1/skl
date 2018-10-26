#include "map.h"
#include <bk/allocator.h>
#include "context.h"
#include "vm.h"


static void
skl_map_mark(skl_ctx_t* ctx, skl_gc_header_t* header);

static void
skl_map_release(skl_ctx_t* ctx, skl_gc_header_t* header);


const skl_gc_info_t skl_map_gc_info = {
	.mark_fn = skl_map_mark,
	.free_fn = skl_map_release,
};


skl_map_t*
skl_map_alloc(skl_ctx_t* ctx)
{
	skl_map_t* map = skl_gc_alloc(ctx, sizeof(skl_map_t), &skl_map_gc_info);
	if(map == NULL) { return map; }

	kh_init(skl_map, ctx->cfg.allocator, &map->entries);

	return map;
}

void
skl_map_int_set(skl_ctx_t* ctx, skl_map_t* map, skl_value_t key, skl_value_t value)
{
	int r;
	khiter_t itr = kh_put(skl_map, &map->entries, key,&r);
	SKL_ASSERT(ctx, r >= 0, "Out of memory");

	kh_val(&map->entries,  itr) = value;
	skl_gc_write_barrier(ctx, &map->gc_header, key);
	skl_gc_write_barrier(ctx, &map->gc_header, value);
}

bool
skl_map_int_get(skl_ctx_t* ctx, skl_map_t* map, skl_value_t key, skl_value_t* value)
{
	(void)ctx;

	khiter_t itr = kh_get(skl_map, &map->entries, key);
	if(itr == kh_end(&map->entries)) { return false; }

	*value = kh_val(&map->entries, itr);

	return true;
}

bool
skl_map_int_delete(skl_ctx_t* ctx, skl_map_t* map, skl_value_t key)
{
	(void)ctx;

	khiter_t itr = kh_get(skl_map, &map->entries, key);
	if(itr == kh_end(&map->entries)) { return false; }

	kh_del(skl_map, &map->entries, itr);
	return true;
}

skl_index_t
skl_map_len(skl_map_t* map)
{
	return (skl_index_t)kh_size(&map->entries);
}


void
skl_map_new(skl_ctx_t* ctx)
{
	skl_vm_push_ref(ctx, SKL_VAL_MAP, skl_map_alloc(ctx));
}

void
skl_map_set(skl_ctx_t* ctx, skl_index_t index)
{
	skl_map_t* map;
	skl_value_t *key, *value;

	SKL_GET_OBJ(map, ctx, index, SKL_VAL_MAP);
	SKL_SAFE_STACK_INDEX(key, ctx, -2, "Stack underflow");
	SKL_SAFE_STACK_INDEX(value, ctx, -1, "Stack underflow");

	skl_map_int_set(ctx, map, *key, *value);
	skl_vm_unsafe_pop(ctx, 2);
}

bool
skl_map_get(skl_ctx_t* ctx, skl_index_t index)
{
	skl_map_t* map;
	skl_value_t *key, value;

	SKL_GET_OBJ(map, ctx, index, SKL_VAL_MAP);
	SKL_SAFE_STACK_INDEX(key, ctx, -1, "Stack underflow");

	bool success = skl_map_int_get(ctx, map, *key, &value);
	if(success)
	{
		skl_vm_push_value(ctx, value);
	}

	return success;
}

bool
skl_map_has(skl_ctx_t* ctx, skl_index_t index)
{
	skl_map_t* map;
	skl_value_t *key, value;

	SKL_GET_OBJ(map, ctx, index, SKL_VAL_MAP);
	SKL_SAFE_STACK_INDEX(key, ctx, -1, "Stack underflow");

	return skl_map_int_get(ctx, map, *key, &value);
}

bool
skl_map_delete(skl_ctx_t* ctx, skl_index_t index)
{
	skl_map_t* map;
	skl_value_t* key;

	SKL_GET_OBJ(map, ctx, index, SKL_VAL_MAP);
	SKL_SAFE_STACK_INDEX(key, ctx, -1, "Stack underflow");

	return skl_map_int_delete(ctx, map, *key);
}


void
skl_map_mark(skl_ctx_t* ctx, skl_gc_header_t* header)
{
	skl_map_t* map = BK_CONTAINER_OF(header, skl_map_t, gc_header);
	kh_foreach(itr, &map->entries)
	{
		skl_gc_mark_value(ctx, kh_key(&map->entries, itr));
		skl_gc_mark_value(ctx, kh_val(&map->entries, itr));
	}
}

void
skl_map_release(skl_ctx_t* ctx, skl_gc_header_t* header)
{
	(void)ctx;

	skl_map_t* map = BK_CONTAINER_OF(header, skl_map_t, gc_header);
	kh_cleanup(skl_map, &map->entries);
}


__KHASH_IMPL(
	skl_map,
	,
	skl_value_t,
	skl_value_t,
	1,
	skl_value_hash,
	skl_value_equal
)
