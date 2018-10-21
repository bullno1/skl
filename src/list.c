#include "list.h"
#include "context.h"


static void
skl_list_mark(skl_ctx_t* ctx, skl_gc_header_t* header);

static void
skl_list_release(skl_ctx_t* ctx, skl_gc_header_t* header);


static const skl_gc_info_t skl_list_gc_info = {
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
