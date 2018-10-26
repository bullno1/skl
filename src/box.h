#ifndef SICKLE_BOX_H
#define SICKLE_BOX_H

#include "common.h"
#include "gc.h"
#include "value.h"


struct skl_box_s
{
	skl_gc_header_t gc_header;
	skl_value_t value;
};


skl_box_t*
skl_box_alloc(skl_ctx_t* ctx);

skl_value_t
skl_box_int_get(skl_ctx_t* ctx, skl_box_t* box);

void
skl_box_int_set(skl_ctx_t* ctx, skl_box_t* box, skl_value_t value);

#endif
