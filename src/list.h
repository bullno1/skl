#ifndef SICKLE_LIST_H
#define SICKLE_LIST_H

#include "common.h"
#include "gc.h"


struct skl_list_s
{
	skl_gc_header_t gc_header;
	BK_ARRAY(skl_value_t) elements;
};


skl_list_t*
skl_list_alloc(skl_ctx_t* ctx, int capacity);

#endif
