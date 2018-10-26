#include "box.h"
#include "vm.h"
#include "context.h"


static void
skl_box_mark(skl_ctx_t* ctx, skl_gc_header_t* header);


const skl_gc_info_t skl_box_gc_info = {
	.mark_fn = skl_box_mark
};


skl_box_t*
skl_box_alloc(skl_ctx_t* ctx)
{
	return skl_gc_alloc(ctx, sizeof(skl_box_t), &skl_box_gc_info);
}

skl_value_t
skl_box_int_get(skl_ctx_t* ctx, skl_box_t* box)
{
	(void)ctx;
	return box->value;
}

void
skl_box_int_set(skl_ctx_t* ctx, skl_box_t* box, skl_value_t value)
{
	(void)ctx;
	box->value = value;
	skl_gc_write_barrier(ctx, &box->gc_header, value);
}


void
skl_box_new(skl_ctx_t* ctx)
{
	skl_vm_push_ref(ctx, SKL_VAL_BOX, skl_box_alloc(ctx));
}

void
skl_box_wrap(skl_ctx_t* ctx, int index)
{
	skl_value_t* value;
	SKL_SAFE_STACK_ADDR(value, ctx, index);

	skl_box_t* box = skl_box_alloc(ctx);
	skl_box_int_set(ctx, box, *value);

	skl_vm_push_ref(ctx, SKL_VAL_BOX, box);
}

void
skl_box_unwrap(skl_ctx_t* ctx, int index)
{
	skl_box_t* box;
	SKL_GET_OBJ(box, ctx, index, SKL_VAL_BOX);
	skl_vm_push_value(ctx, box->value);
}

void
skl_box_put(skl_ctx_t* ctx, int index)
{
	skl_box_t* box;
	SKL_GET_OBJ(box, ctx, index, SKL_VAL_BOX);

	skl_value_t* value;
	SKL_SAFE_STACK_INDEX(value, ctx, -1, "Stack underflow");

	skl_box_int_set(ctx, box, *value);
	skl_vm_unsafe_pop(ctx, 1);
}


void
skl_box_mark(skl_ctx_t* ctx, skl_gc_header_t* header)
{
	skl_box_t* box = BK_CONTAINER_OF(header, skl_box_t, gc_header);
	skl_gc_mark_value(ctx, box->value);
}
