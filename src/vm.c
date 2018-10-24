#include "vm.h"
#include "context.h"
#include <bk/allocator.h>


static void
skl_vm_mark(skl_ctx_t* ctx, skl_gc_header_t* header);

static void
skl_vm_release(skl_ctx_t* ctx, skl_gc_header_t* header);


const skl_gc_info_t skl_vm_gc_info = {
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
	vm->sp_max = vm->sp_min + cfg->operand_stack_size; // 1 past allocated area
	vm->sp = vm->sp_min;

	vm->fp_min = bk_malloc(allocator, sizeof(*vm->fp) * cfg->call_stack_size);
	vm->fp_max = vm->fp_min + cfg->call_stack_size - 1;
	vm->fp = vm->fp_min;

	*(vm->fp) = (skl_stack_frame_t) {
		.bp = vm->sp
	};

	return vm;
}

void
skl_vm_push_ref(skl_ctx_t* ctx, skl_value_type_t type, void* ref)
{
	SKL_ASSERT(ctx, ref != NULL, "Out of memory");
	skl_vm_push_value(ctx, skl_value_make_ref(type, ref));
}

void
skl_vm_push_value(skl_ctx_t* ctx, skl_value_t value)
{
	skl_vm_t* vm = ctx->vm;
	SKL_ASSERT(ctx, vm->sp < vm->sp_max, "Stack overflow");

	*(vm->sp++) = value;
}

skl_value_t
skl_vm_pop(skl_ctx_t* ctx)
{
	skl_vm_t* vm = ctx->vm;
	SKL_ASSERT(ctx, vm->sp > vm->fp->bp, "Stack underflow");

	return *(--vm->sp);
}


void
skl_push_null(skl_ctx_t* ctx)
{
	skl_vm_push_value(ctx, skl_value_make_null());
}

void
skl_push_number(skl_ctx_t* ctx, double number)
{
	skl_vm_push_value(ctx, skl_value_make_number(number));
}

void
skl_dup(skl_ctx_t* ctx, int index)
{
	skl_value_t* value;
	SKL_SAFE_STACK_ADDR(value, ctx, index);

	skl_vm_push_value(ctx, *value);
}

void
skl_replace(skl_ctx_t* ctx, int index)
{
	skl_value_t* value;
	SKL_SAFE_STACK_ADDR(value, ctx, index);

	*value = *(--ctx->vm->sp);
}

bool
skl_check_stack(skl_ctx_t* ctx, int count)
{
	skl_vm_t* vm = ctx->vm;
	skl_value_t* next_sp = vm->sp + count;
	return vm->fp->bp <= next_sp && next_sp <= vm->sp_max;
}

int
skl_stack_len(skl_ctx_t* ctx)
{
	skl_vm_t* vm = ctx->vm;
	return vm->sp - vm->fp->bp;
}

void
skl_resize_stack(skl_ctx_t* ctx, int count)
{
	skl_vm_t* vm = ctx->vm;
	skl_value_t* target = skl_vm_stack_addr(vm, count);
	SKL_ASSERT(ctx, vm->fp->bp <= target && target <= vm->sp_max, "Invalid index");

	if(target > vm->sp)
	{
		skl_value_t null = skl_value_make_null();
		for(skl_value_t* itr = vm->sp; itr <= target; ++itr)
		{
			*itr = null;
		}
	}

	vm->sp = target;
}

bool
skl_check_type(skl_ctx_t* ctx, int index, skl_value_type_t type)
{
	skl_value_t* value;
	SKL_SAFE_STACK_ADDR(value, ctx, index);

	return skl_value_check_type(*value, type);
}

double
skl_as_number(skl_ctx_t* ctx, int index)
{
	skl_value_t* value;
	SKL_SAFE_STACK_ADDR(value, ctx, index);
	SKL_ASSERT(ctx, skl_value_check_type(*value, SKL_VAL_NUMBER), "Type error");

	return skl_value_as_number(*value);
}


void
skl_vm_mark(skl_ctx_t* ctx, skl_gc_header_t* header)
{
	skl_vm_t* vm = BK_CONTAINER_OF(header, skl_vm_t, gc_header);

	for(skl_value_t* itr = vm->sp_min; itr < vm->sp; ++itr)
	{
		skl_gc_mark_value(ctx, *itr);
	}
}

skl_value_t*
skl_vm_stack_addr(const skl_vm_t* vm, int index)
{
	skl_value_t* base_addr = index >= 0 ? vm->fp->bp : vm->sp;
	return base_addr + index;
}

void
skl_vm_release(skl_ctx_t* ctx, skl_gc_header_t* header)
{
	skl_vm_t* vm = BK_CONTAINER_OF(header, skl_vm_t, gc_header);
	bk_allocator_t* allocator = ctx->cfg.allocator;
	bk_free(allocator, vm->sp_min);
	bk_free(allocator, vm->fp_min);
}
