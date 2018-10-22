#include "vm.h"
#include "context.h"
#include <bk/allocator.h>


static void
skl_vm_mark(skl_ctx_t* ctx, skl_gc_header_t* header);

static void
skl_vm_release(skl_ctx_t* ctx, skl_gc_header_t* header);

static skl_exec_status_t
skl_vm_push_value(skl_ctx_t* ctx, skl_value_t value);

static skl_value_t*
skl_vm_stack_addr(skl_vm_t* vm, int index)
{
	skl_value_t* base_addr = index >= 0 ? vm->fp->bp : vm->sp;
	return base_addr + index;
}


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
	vm->sp = vm->sp_min;

	vm->fp_min = bk_malloc(allocator, sizeof(*vm->fp) * cfg->call_stack_size);
	vm->fp_max = vm->fp_min + cfg->call_stack_size - 1;
	vm->fp = vm->fp_min;

	*(vm->fp) = (skl_stack_frame_t) {
		.bp = vm->sp
	};

	return vm;
}

skl_exec_status_t
skl_vm_push_ref(skl_ctx_t* ctx, skl_value_type_t type, void* ref)
{
	SKL_ASSERT(ctx, ref != NULL, "skl.out-of-memory");
	return skl_vm_push_value(ctx, skl_value_make_ref(type, ref));
}

skl_exec_status_t
skl_push_null(skl_ctx_t* ctx)
{
	return skl_vm_push_value(ctx, skl_value_make_null());
}

skl_exec_status_t
skl_push_number(skl_ctx_t* ctx, double number)
{
	return skl_vm_push_value(ctx, skl_value_make_number(number));
}

skl_exec_status_t
skl_dup(skl_ctx_t* ctx, int index)
{
	int stack_len = skl_stack_len(ctx);
	SKL_ASSERT(
		ctx, -stack_len <= index || index < stack_len, "skl.invalid-index"
	);

	return skl_vm_push_value(ctx, *skl_vm_stack_addr(ctx->vm, index));
}

int
skl_stack_len(skl_ctx_t* ctx)
{
	skl_vm_t* vm = ctx->vm;
	return vm->sp - vm->fp->bp;
}

skl_exec_status_t
skl_pop(skl_ctx_t* ctx, int count)
{
	SKL_ASSERT(
		ctx, 0 <= count && count <= skl_stack_len(ctx), "skl.invalid-index"
	);

	ctx->vm->sp -= count;
	return SKL_EXEC_OK;
}


void
skl_vm_mark(skl_ctx_t* ctx, skl_gc_header_t* header)
{
	skl_vm_t* vm = (skl_vm_t*)header;

	for(skl_value_t* itr = vm->sp_min; itr < vm->sp; ++itr)
	{
		skl_gc_mark_value(ctx, *itr);
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

skl_exec_status_t
skl_vm_push_value(skl_ctx_t* ctx, skl_value_t value)
{
	skl_vm_t* vm = ctx->vm;
	SKL_ASSERT(ctx, vm->sp <= vm->sp_max, "skl.stack-overflow");

	*(vm->sp++) = value;
	return SKL_EXEC_OK;
}
