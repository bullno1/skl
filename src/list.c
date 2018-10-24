#include "list.h"
#include <limits.h>
#include <bk/array.h>
#include "context.h"
#include "vm.h"


static void
skl_list_mark(skl_ctx_t* ctx, skl_gc_header_t* header);

static void
skl_list_release(skl_ctx_t* ctx, skl_gc_header_t* header);

static int
skl_list_normalize_index(skl_ctx_t* ctx, skl_list_t* list, int index)
{
	int len = skl_list_len(list);
	if(index < 0) { index += len; }
	SKL_ASSERT(ctx, 0 <= index && index < len, "Invalid list index");

	return index;
}


const skl_gc_info_t skl_list_gc_info = {
	.mark_fn = skl_list_mark,
	.free_fn = skl_list_release,
};


skl_list_t*
skl_list_alloc(skl_ctx_t* ctx, int capacity)
{
	skl_list_t* list = skl_gc_alloc(ctx, sizeof(skl_list_t), &skl_list_gc_info);
	if(list == NULL) { return list; }

	list->elements = bk_array_create(
		ctx->cfg.allocator, skl_value_t, BK_MAX(1, capacity)
	);

	return list;
}

skl_value_t
skl_list_int_get(skl_ctx_t* ctx, skl_list_t* list, int index)
{
	(void)ctx;
	index = skl_list_normalize_index(ctx, list, index);
	return list->elements[index];
}

void
skl_list_int_set(skl_ctx_t* ctx, skl_list_t* list, int index, skl_value_t value)
{
	index = skl_list_normalize_index(ctx, list, index);
	list->elements[index] = value;
	skl_gc_write_barrier(ctx, &list->gc_header, value);
}

void
skl_list_int_push(skl_ctx_t* ctx, skl_list_t* list, skl_value_t value)
{
	bk_array_push(list->elements, value);
	skl_gc_write_barrier(ctx, &list->gc_header, value);
}

void
skl_list_int_insert(skl_ctx_t* ctx, skl_list_t* list, int index, skl_value_t value)
{
	index = skl_list_normalize_index(ctx, list, index);
	bk_array_insert(list->elements, index, value);
	skl_gc_write_barrier(ctx, &list->gc_header, value);
}

void
skl_list_int_delete(skl_ctx_t* ctx, skl_list_t* list, int index)
{
	(void)ctx;
	index = skl_list_normalize_index(ctx, list, index);
	bk_array_remove(list, index);
}

void
skl_list_int_quick_delete(skl_ctx_t* ctx, skl_list_t* list, int index)
{
	(void)ctx;
	index = skl_list_normalize_index(ctx, list, index);
	bk_array_quick_remove(list, index);
}

int
skl_list_len(skl_list_t* list)
{
	return bk_array_len(list->elements);
}


void
skl_list_new(skl_ctx_t* ctx, int capacity)
{
	skl_vm_push_ref(ctx, SKL_VAL_LIST, skl_list_alloc(ctx, capacity));
}

void
skl_list_get(skl_ctx_t* ctx, int index, int n)
{
	skl_list_t* list;
	SKL_GET_OBJ(list, ctx, index, SKL_VAL_LIST);

	skl_vm_push_value(ctx, skl_list_int_get(ctx, list, n));
}

void
skl_list_set(skl_ctx_t* ctx, int index, int n)
{
	skl_list_t* list;
	SKL_GET_OBJ(list, ctx, index, SKL_VAL_LIST);

	skl_list_int_set(ctx, list, n, skl_vm_back(ctx));
	skl_vm_unsafe_pop(ctx);
}

void
skl_list_push(skl_ctx_t* ctx, int index)
{
	skl_list_t* list;
	SKL_GET_OBJ(list, ctx, index, SKL_VAL_LIST);

	skl_list_int_push(ctx, list, skl_vm_back(ctx));
	skl_vm_unsafe_pop(ctx);
}

void
skl_list_insert(skl_ctx_t* ctx, int index, int n)
{
	skl_list_t* list;
	SKL_GET_OBJ(list, ctx, index, SKL_VAL_LIST);

	skl_list_int_insert(ctx, list, n, skl_vm_back(ctx));
	skl_vm_unsafe_pop(ctx);
}

void
skl_list_delete(skl_ctx_t* ctx, int index, int n)
{
	skl_list_t* list;
	SKL_GET_OBJ(list, ctx, index, SKL_VAL_LIST);

	skl_list_int_delete(ctx, list, n);
}


void
skl_list_quick_delete(skl_ctx_t* ctx, int index, int n)
{
	skl_list_t* list;
	SKL_GET_OBJ(list, ctx, index, SKL_VAL_LIST);

	skl_list_int_quick_delete(ctx, list, n);
}


void
skl_list_mark(skl_ctx_t* ctx, skl_gc_header_t* header)
{
	skl_list_t* list = BK_CONTAINER_OF(header, skl_list_t, gc_header);
	bk_array_foreach(skl_value_t, value, list->elements)
	{
		skl_gc_mark_value(ctx, *value);
	}
}

void
skl_list_release(skl_ctx_t* ctx, skl_gc_header_t* header)
{
	(void)ctx;
	skl_list_t* list = BK_CONTAINER_OF(header, skl_list_t, gc_header);
	bk_array_destroy(list->elements);
}
