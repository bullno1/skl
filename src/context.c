#include "context.h"
#include <stdio.h>
#include <bk/allocator.h>
#include "vm.h"
#include "gc.h"


static void
skl_default_panic_handler(skl_ctx_t* ctx, skl_string_ref_t msg);


skl_ctx_t*
skl_create_ctx(const skl_config_t* cfg)
{
	skl_ctx_t* ctx = BK_UNSAFE_NEW(cfg->allocator, skl_ctx_t);
	if(ctx == NULL) { return ctx; }

	*ctx = (skl_ctx_t) {
		.cfg = *cfg,
	};

	skl_strpool_init(ctx);
	skl_gc_init(ctx);

	ctx->vm = NULL;
	ctx->main_vm = skl_vm_alloc(ctx);
	SKL_ASSERT(ctx, ctx->main_vm != NULL, "Could not allocate main VM");
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

void
skl_panic(skl_ctx_t* ctx, skl_string_ref_t msg)
{
	skl_panic_fn_t panic_handler = ctx->cfg.panic_handler;
	if(panic_handler == NULL) { panic_handler = skl_default_panic_handler; }

	skl_default_panic_handler(ctx, msg);
}


void
skl_default_panic_handler(skl_ctx_t* ctx, skl_string_ref_t msg)
{
	(void)ctx;
	printf("skl_context<0x%p>: %.*s\n", (void*)ctx, (int)msg.length, msg.ptr);
	abort();
}
