#include <bk/macro.h>
#define UGC_IMPLEMENTATION
#define UGC_DECL BK_INLINE
#include <ugc/ugc.h>
#include "gc.h"
#include <bk/assert.h>
#include <bk/allocator.h>
#include <bk/array.h>
#include "list.h"
#include "context.h"
#include "value.h"


static void
skl_gc_visit(ugc_t* gc, ugc_header_t* header);

static void
skl_gc_release(ugc_t* gc, ugc_header_t* header);

static void
skl_gc_mark_vm(skl_ctx_t* ctx, skl_vm_t* vm);


void
skl_gc_init(skl_ctx_t* ctx)
{
	ugc_init(&ctx->gc.ugc, skl_gc_visit, skl_gc_release);
	ctx->gc.ugc.userdata = ctx;

	ctx->gc.refs = skl_list_alloc(ctx, 1);
	BK_ASSERT(ctx->gc.refs, "Could not allocate gc_refs");
	ctx->gc.pause = 0;
	ctx->gc.free_ref_handles =
		bk_array_create(ctx->cfg.allocator, skl_gc_handle_t, 1);
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

	obj->gc_info = gc_info;
	ugc_register(&ctx->gc.ugc, &obj->ugc_header);

	return obj;
}

void
skl_gc_mark_obj(skl_ctx_t* ctx, skl_gc_header_t* obj)
{
	if(obj == NULL) { return; }

	ugc_visit(&ctx->gc.ugc, &obj->ugc_header);
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
skl_gc(skl_ctx_t* ctx, skl_gc_op_t op)
{
	switch(op)
	{
		case SKL_GC_STEP:
			if(ctx->gc.pause == 0) { ugc_step(&ctx->gc.ugc); }
			break;
		case SKL_GC_COLLECT:
			if(ctx->gc.pause == 0) { ugc_collect(&ctx->gc.ugc); }
			break;
		case SKL_GC_PAUSE:
			ctx->gc.pause++;
			break;
		case SKL_GC_UNPAUSE:
			BK_ASSERT(ctx->gc.pause > 0, "Unbalanced GC_PAUSE/GC_UNPAUSE");
			ctx->gc.pause--;
			break;
	}
}


void
skl_gc_visit(ugc_t* gc, ugc_header_t* header)
{
	skl_ctx_t* ctx = gc->userdata;
	if(header == NULL)
	{
		skl_gc_mark_obj(ctx, &ctx->gc.refs->gc_header);
		skl_gc_mark_vm(ctx, &ctx->main_vm);
		skl_gc_mark_vm(ctx, ctx->vm);
	}
	else
	{
		skl_gc_header_t* obj =
			BK_CONTAINER_OF(header, skl_gc_header_t, ugc_header);
		obj->gc_info->mark_fn(ctx, obj);
	}
}

void
skl_gc_release(ugc_t* gc, ugc_header_t* header)
{
	skl_ctx_t* ctx = gc->userdata;
	skl_gc_header_t* obj = BK_CONTAINER_OF(header, skl_gc_header_t, ugc_header);
	obj->gc_info->free_fn(ctx, obj);
	bk_free(ctx->cfg.allocator, obj);
}

void
skl_gc_mark_vm(skl_ctx_t* ctx, skl_vm_t* vm)
{
	for(skl_value_t* itr = vm->sp; itr <= vm->sp_max; ++itr)
	{
		skl_gc_mark_value(ctx, *itr);
	}

	for(skl_stack_frame_t* itr = vm->fp_min; itr <= vm->fp; ++itr)
	{
		skl_gc_mark_obj(ctx, &itr->proc->gc_header);
	}
}
