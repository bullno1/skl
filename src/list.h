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

skl_value_t
skl_list_int_get(skl_ctx_t* ctx, skl_list_t* list, skl_index_t index);

void
skl_list_int_set(skl_ctx_t* ctx, skl_list_t* list, skl_index_t index, skl_value_t value);

void
skl_list_int_push(skl_ctx_t* ctx, skl_list_t* list, skl_value_t value);

void
skl_list_int_insert(skl_ctx_t* ctx, skl_list_t* list, skl_index_t index, skl_value_t value);

void
skl_list_int_delete(skl_ctx_t* ctx, skl_list_t* list, skl_index_t index);

void
skl_list_int_quick_delete(skl_ctx_t* ctx, skl_list_t* list, skl_index_t index);

skl_index_t
skl_list_len(skl_list_t* list);

#endif
