#include "vm.h"
#include "context.h"
#include <bk/allocator.h>


static void
skl_vm_mark(skl_ctx_t* ctx, skl_gc_header_t* header);

static void
skl_vm_release(skl_ctx_t* ctx, skl_gc_header_t* header);


static const skl_gc_info_t skl_vm_gc_info = {
	.mark_fn = skl_vm_mark,
	.free_fn = skl_vm_release,
};


skl_vm_t*
skl_vm_alloc(skl_ctx_t* ctx)
{
	skl_vm_t* vm = skl_gc_alloc(ctx, sizeof(skl_vm_t), &skl_vm_gc_info);

	const skl_config_t* cfg = &ctx->cfg;
	bk_allocator_t* allocator = cfg->allocator;

	vm->sp_min = bk_malloc(allocator, sizeof(*vm->sp) * cfg->operand_stack_size);
	vm->sp_max = vm->sp_min + cfg->operand_stack_size - 1;

	vm->fp_min = bk_malloc(allocator, sizeof(*vm->fp) * cfg->call_stack_size);
	vm->fp_max = vm->fp_min + cfg->call_stack_size - 1;

	skl_vm_reset(vm);

	return vm;
}

void
skl_vm_reset(skl_vm_t* vm)
{
	vm->sp = vm->sp_min;
	vm->fp = vm->fp_min;
	*(vm->fp) = (skl_stack_frame_t) {
		.bp = vm->sp
	};
}


void
skl_vm_mark(skl_ctx_t* ctx, skl_gc_header_t* header)
{
	skl_vm_t* vm = (skl_vm_t*)header;

	for(skl_value_t* itr = vm->sp_min; itr < vm->sp; ++itr)
	{
		skl_gc_mark_value(ctx, *itr);
	}

	for(skl_stack_frame_t* itr = vm->fp_min; itr <= vm->fp; ++itr)
	{
		skl_gc_mark_obj(ctx, itr->proc);
	}
}

void
skl_vm_release(skl_ctx_t* ctx, skl_gc_header_t* header)
{
	bk_allocator_t* allocator = ctx->cfg.allocator;
	skl_vm_t* vm = (skl_vm_t*)header;
	bk_free(allocator, vm->sp_min);
	bk_free(allocator, vm->fp_min);
}
