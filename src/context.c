#include "context.h"
#include <bk/allocator.h>


skl_ctx_t*
skl_create_ctx(skl_config_t* cfg)
{
	skl_ctx_t* ctx = BK_UNSAFE_NEW(cfg->allocator, skl_ctx_t);
	if(ctx == NULL) { return ctx; }

	*ctx = (skl_ctx_t) {
		.cfg = *cfg
	};
	skl_lexer_init(&ctx->lexer, ctx);

	return ctx;
}

void
skl_destroy_ctx(skl_ctx_t* ctx)
{
	skl_lexer_cleanup(&ctx->lexer);
	bk_free(ctx->cfg.allocator, ctx);
}
