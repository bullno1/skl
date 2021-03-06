#include <bk/macro.h>
#define UGC_IMPLEMENTATION
#define UGC_DECL BK_INLINE
#include <ugc/ugc.h>
#include "gc.h"
#include <bk/allocator.h>
#include <bk/array.h>
#include "list.h"
#include "context.h"
#include "value.h"
#include "vm.h"


static void
skl_gc_visit(ugc_t* gc, ugc_header_t* header);

static void
skl_gc_release(ugc_t* gc, ugc_header_t* header);

static void
skl_gc_visit_root(ugc_t* gc);

static void
skl_gc_visit_obj(ugc_t* gc, ugc_header_t* header);


void
skl_gc_init(skl_ctx_t* ctx)
{
	skl_gc_t* gc = &ctx->gc;
	ugc_init(&gc->ugc, skl_gc_visit, skl_gc_release);
	gc->ugc.userdata = ctx;

	gc->refs = skl_list_alloc(ctx, 1);
	SKL_ASSERT(ctx, gc->refs, "Could not allocate gc_refs");
	gc->pause = 0;
	gc->rescan_list.next = NULL;
	gc->free_ref_handles = bk_array_create(ctx->cfg.allocator, skl_index_t, 1);
}

void
skl_gc_cleanup(skl_ctx_t* ctx)
{
	bk_array_destroy(ctx->gc.free_ref_handles);
	ugc_release_all(&ctx->gc.ugc);
}

void*
skl_gc_alloc(skl_ctx_t* ctx, size_t size, const skl_gc_info_t* gc_info)
{
	// TODO: implement emergency GC
	skl_gc_header_t* obj = bk_malloc(ctx->cfg.allocator, size);
	if(obj == NULL) { return obj; }

	memset(obj, 0, BK_MIN(sizeof(skl_gc_rescan_header_t), size));

	SKL_ASSERT(ctx, gc_info != NULL, "Invalid gc_info");
	obj->gc_info = gc_info;

	ugc_register(&ctx->gc.ugc, &obj->ugc_header);

	// An allocation means there could be new objects stored to the stack
	if(ctx->vm != NULL)
	{
		skl_gc_schedule_rescan(ctx, &ctx->vm->gc_header);
	}

	return obj;
}

void
skl_gc_mark_obj(skl_ctx_t* ctx, skl_gc_header_t* header)
{
	if(header == NULL) { return; }

	ugc_visit(&ctx->gc.ugc, &header->ugc_header);
}

void
skl_gc_mark_value(skl_ctx_t* ctx, skl_value_t value)
{
	if(skl_value_is_ref(value))
	{
		skl_gc_mark_obj(ctx, skl_value_as_ref(value));
	}
}

void
skl_gc_write_barrier(skl_ctx_t* ctx, skl_gc_header_t* obj, skl_value_t value)
{
	if(skl_value_is_ref(value))
	{
		ugc_add_ref(
			&ctx->gc.ugc,
			&obj->ugc_header,
			skl_value_as_ref(value)
		);
	}
}


void
skl_gc(skl_ctx_t* ctx, skl_gc_op_t op)
{
	skl_gc_t* gc = &ctx->gc;
	switch(op)
	{
		case SKL_GC_STEP:
			if(gc->pause == 0) { ugc_step(&gc->ugc); }
			break;
		case SKL_GC_COLLECT:
			if(gc->pause == 0) { ugc_collect(&gc->ugc); }
			break;
		case SKL_GC_PAUSE:
			gc->pause++;
			break;
		case SKL_GC_UNPAUSE:
			SKL_ASSERT(ctx, gc->pause > 0, "Unbalanced GC_PAUSE/GC_UNPAUSE");
			gc->pause--;
			break;
	}
}

skl_index_t
skl_gc_ref(skl_ctx_t* ctx, skl_index_t index)
{
	skl_value_t* value;
	SKL_SAFE_STACK_ADDR(value, ctx, index);

	skl_gc_t* gc = &ctx->gc;
	size_t num_free_handles = bk_array_len(gc->free_ref_handles);
	skl_index_t handle;

	if(num_free_handles > 0)
	{
		handle = bk_array_pop(gc->free_ref_handles);
		skl_list_int_set(ctx, gc->refs, handle, *value);
	}
	else
	{
		handle = skl_list_len(gc->refs);
		skl_list_int_push(ctx, gc->refs, *value);
	}

	return handle;
}

void
skl_gc_deref(skl_ctx_t* ctx, skl_index_t handle)
{
	skl_value_t value = skl_list_int_get(ctx, ctx->gc.refs, handle);
	skl_vm_push_value(ctx, value);
}

void
skl_gc_unref(skl_ctx_t* ctx, skl_index_t handle)
{
	skl_gc_t* gc = &ctx->gc;
	skl_list_int_set(ctx, gc->refs, handle, skl_value_make_null());
	bk_array_push(gc->free_ref_handles, handle);
}


void
skl_gc_visit(ugc_t* gc, ugc_header_t* header)
{
	if(header == NULL)
	{
		skl_gc_visit_root(gc);
	}
	else
	{
		skl_gc_visit_obj(gc, header);
	}
}

void
skl_gc_visit_root(ugc_t* ugc)
{
	skl_ctx_t* ctx = ugc->userdata;
	skl_gc_t* gc = &ctx->gc;

	skl_gc_mark_obj(ctx, &gc->refs->gc_header);
	skl_gc_mark_obj(ctx, &ctx->main_vm->gc_header.gc_header);

	for(skl_gc_rescan_header_t* itr = gc->rescan_list.next; itr != NULL;)
	{
		skl_gc_rescan_header_t* next = itr->next;

		skl_gc_visit_fn_t mark_fn = itr->gc_header.gc_info->mark_fn;
		mark_fn(ctx, (skl_gc_header_t*)itr);
		itr->next = NULL;

		itr = next;
	}
	gc->rescan_list.next = NULL;
}

void
skl_gc_visit_obj(ugc_t* gc, ugc_header_t* header)
{
	skl_gc_header_t* obj = (skl_gc_header_t*)header;

	skl_gc_visit_fn_t mark_fn = obj->gc_info->mark_fn;
	if(mark_fn != NULL) { mark_fn(gc->userdata, obj); }
}

void
skl_gc_release(ugc_t* gc, ugc_header_t* header)
{
	skl_ctx_t* ctx = gc->userdata;
	skl_gc_header_t* obj = (skl_gc_header_t*)header;

	skl_gc_visit_fn_t free_fn = obj->gc_info->free_fn;
	if(free_fn != NULL) { free_fn(ctx, obj); }

	bk_free(ctx->cfg.allocator, obj);
}

void
skl_gc_schedule_rescan(skl_ctx_t* ctx, skl_gc_rescan_header_t* header)
{
	if(header->next == NULL)
	{
		skl_gc_rescan_header_t* rescan_list = &ctx->gc.rescan_list;
		header->next = rescan_list->next;
		rescan_list->next = header;
	}
}
