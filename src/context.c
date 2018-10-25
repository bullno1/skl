#include "context.h"
#include <stdio.h>
#include <bk/allocator.h>
#include <bk/array.h>
#include "vm.h"
#include "gc.h"


static void
skl_default_panic_handler(skl_ctx_t* ctx, const char* fmt, va_list args);


skl_ctx_t*
skl_create_ctx(const skl_config_t* cfg)
{
	skl_ctx_t* ctx = BK_UNSAFE_NEW(cfg->allocator, skl_ctx_t);
	if(ctx == NULL) { return ctx; }

	*ctx = (skl_ctx_t) {
		.cfg = *cfg,
		.fmt_buf = bk_array_create(cfg->allocator, char, 80)
	};

	skl_set_error_panic(ctx, cfg->panic_handler);

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

	bk_array_destroy(ctx->fmt_buf);
	bk_free(ctx->cfg.allocator, ctx);
}

skl_trap_t
skl_set_trap(skl_ctx_t* ctx, skl_trap_t trap)
{
	skl_trap_t old_trap = ctx->trap;
	ctx->trap = trap;
	return old_trap;
}

skl_trap_t
skl_set_error_panic(skl_ctx_t* ctx, skl_panic_fn_t panic_handler)
{
	skl_trap_t trap = {
		.panic_handler = panic_handler ? panic_handler : skl_default_panic_handler
	};
	return skl_set_trap(ctx, trap);
}

jmp_buf*
skl_prepare_longjmp(skl_ctx_t* ctx, skl_trap_t* old)
{
	*old = ctx->trap;
	ctx->trap.panic_handler = NULL;
	return &ctx->trap.jmp_buf;
}

void
skl_throw(skl_ctx_t* ctx, const char* fmt, ...)
{
	va_list args;
	skl_trap_t* trap = &ctx->trap;

	if(trap->panic_handler != NULL)
	{
		va_start(args, fmt);
		trap->panic_handler(ctx, fmt, args);
		va_end(args); // unreachable
	}
	else
	{
		jmp_buf jmpbuf;
		memcpy(&jmpbuf, &trap->jmp_buf, sizeof(jmp_buf));

		skl_set_error_panic(ctx, ctx->cfg.panic_handler);
		va_start(args, fmt);
		skl_push_string_fmtv(ctx, fmt, args);
		va_end(args);

		longjmp(jmpbuf, 1);
	}
}


void
skl_default_panic_handler(skl_ctx_t* ctx, const char* fmt, va_list args)
{
	printf("skl_context_t<%p>: ", (void*)ctx);
	vprintf(fmt, args);
	printf("\n");
	abort();
}
