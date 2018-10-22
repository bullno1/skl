#include "context.h"
#include <bk/allocator.h>
#include "gc.h"


skl_ctx_t*
skl_create_ctx(skl_config_t* cfg)
{
	skl_ctx_t* ctx = BK_UNSAFE_NEW(cfg->allocator, skl_ctx_t);
	if(ctx == NULL) { return ctx; }

	*ctx = (skl_ctx_t) {
		.cfg = *cfg
	};

	skl_strpool_init(ctx);
	skl_gc_init(ctx);
	ctx->main_vm = skl_vm_alloc(ctx);
	BK_ASSERT(ctx->main_vm != NULL, "Could not allocate VM");
	ctx->vm = ctx->main_vm;
	skl_lexer_init(&ctx->lexer, ctx);

	return ctx;
}

void
skl_destroy_ctx(skl_ctx_t* ctx)
{
	skl_lexer_cleanup(&ctx->lexer);
	skl_gc_cleanup(ctx);
	skl_strpool_cleanup(ctx);

	bk_free(ctx->cfg.allocator, ctx);
}
