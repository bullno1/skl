#include <bk/macro.h>
#define UGC_IMPLEMENTATION
#define UGC_DECL BK_INLINE
#include <ugc/ugc.h>
#include "gc.h"
#include <bk/assert.h>
#include <bk/allocator.h>
#include "string.h"
#include "context.h"


static void
skl_gc_visit(ugc_t* gc, ugc_header_t* header);

static void
skl_gc_release(ugc_t* gc, ugc_header_t* header);


void
skl_gc_init(skl_ctx_t* ctx)
{
	ugc_init(&ctx->gc, skl_gc_visit, skl_gc_release);
	ctx->gc.userdata = ctx;
}

void
skl_gc_cleanup(skl_ctx_t* ctx)
{
	ugc_release_all(&ctx->gc);
}

void*
skl_gc_alloc(skl_ctx_t* ctx, size_t size, const skl_gc_info_t* gc_info)
{
	// TODO: implement emergency GC
	skl_gc_header_t* obj = bk_malloc(ctx->cfg.allocator, size);
	if(obj == NULL) { return obj; }

	obj->gc_info = gc_info;
	ugc_register(&ctx->gc, &obj->ugc_header);

	return obj;
}

void
skl_gc_mark_obj(skl_ctx_t* ctx, skl_gc_header_t* obj)
{
	if(obj == NULL) { return; }

	ugc_visit(&ctx->gc, &obj->ugc_header);
}

void
skl_gc(skl_ctx_t* ctx, skl_gc_op_t op)
{
	switch(op)
	{
		case SKL_GC_STEP:
			if(ctx->gc_pause == 0) { ugc_step(&ctx->gc); }
			break;
		case SKL_GC_COLLECT:
			if(ctx->gc_pause == 0) { ugc_collect(&ctx->gc); }
			break;
		case SKL_GC_PAUSE:
			ctx->gc_pause++;
			break;
		case SKL_GC_UNPAUSE:
			BK_ASSERT(ctx->gc_pause > 0, "Unbalanced GC_PAUSE/GC_UNPAUSE");
			ctx->gc_pause--;
			break;
	}
}


void
skl_gc_visit(ugc_t* gc, ugc_header_t* header)
{
	skl_ctx_t* ctx = gc->userdata;
	if(header == NULL)
	{
		// TODO: mark root set
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
}
