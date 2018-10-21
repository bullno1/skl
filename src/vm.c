#include "vm.h"
#include "context.h"
#include <bk/allocator.h>


void
skl_vm_init(skl_vm_t* vm, skl_ctx_t* ctx)
{
	const skl_config_t* cfg = &ctx->cfg;

	vm->ctx = ctx;

	vm->sp_min =
		bk_malloc(cfg->allocator, sizeof(*vm->sp) * cfg->operand_stack_size);
	vm->sp_max = vm->sp_min + cfg->operand_stack_size - 1;

	vm->fp_min =
		bk_malloc(cfg->allocator, sizeof(*vm->fp) * cfg->call_stack_size);
	vm->fp_max = vm->fp_min + cfg->call_stack_size - 1;

	skl_vm_reset(vm);
}

void
skl_vm_cleanup(skl_vm_t* vm)
{
	bk_allocator_t* allocator = vm->ctx->cfg.allocator;
	bk_free(allocator, vm->fp_min);
	bk_free(allocator, vm->sp_min);
}

void
skl_vm_reset(skl_vm_t* vm)
{
	vm->sp = vm->sp_max;
	vm->fp = vm->fp_min;
}
