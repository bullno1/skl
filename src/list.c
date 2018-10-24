#include "list.h"
#include <bk/array.h>
#include "context.h"
#include "vm.h"


static void
skl_list_mark(skl_ctx_t* ctx, skl_gc_header_t* header);

static void
skl_list_release(skl_ctx_t* ctx, skl_gc_header_t* header);


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


void
skl_list_new(skl_ctx_t* ctx, int capacity)
{
	skl_vm_push_ref(ctx, SKL_VAL_LIST, skl_list_alloc(ctx, capacity));
}

void
skl_list_get(skl_ctx_t* ctx, int index, int n);

void
skl_list_set(skl_ctx_t* ctx, int index, int n);

void
skl_list_resize(skl_ctx_t* ctx, int index, int size);

void
skl_list_insert(skl_ctx_t* ctx, int index, int n);

void
skl_list_delete(skl_ctx_t* ctx, int index, int n);
