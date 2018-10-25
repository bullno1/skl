#ifndef SICKLE_GC_H
#define SICKLE_GC_H

#include "common.h"
#include <ugc/ugc.h>
#include "value.h"


typedef struct skl_gc_s skl_gc_t;
typedef struct skl_gc_header_s skl_gc_header_t;
typedef struct skl_gc_rescan_header_s skl_gc_rescan_header_t;
typedef struct skl_gc_info_s skl_gc_info_t;
typedef void(*skl_gc_visit_fn_t)(skl_ctx_t* ctx, skl_gc_header_t* obj);


struct skl_gc_info_s
{
	skl_gc_visit_fn_t mark_fn;
	skl_gc_visit_fn_t free_fn;
};

struct skl_gc_header_s
{
	ugc_header_t ugc_header;
	const skl_gc_info_t* gc_info;
};

struct skl_gc_rescan_header_s
{
	skl_gc_header_t gc_header;
	skl_gc_rescan_header_t* next;
};

struct skl_gc_s
{
	ugc_t ugc;
	unsigned int pause;
	skl_list_t* refs;
	skl_gc_rescan_header_t rescan_list;
	BK_ARRAY(skl_index_t) free_ref_handles;
};


void
skl_gc_init(skl_ctx_t* ctx);

void
skl_gc_cleanup(skl_ctx_t* ctx);

void*
skl_gc_alloc(skl_ctx_t* ctx, size_t size, const skl_gc_info_t* gc_info);

void
skl_gc_mark_obj(skl_ctx_t* ctx, skl_gc_header_t* obj);

void
skl_gc_mark_value(skl_ctx_t* ctx, skl_value_t value);

void
skl_gc_write_barrier(skl_ctx_t* ctx, skl_gc_header_t* obj, skl_value_t value);

void
skl_gc_schedule_rescan(skl_ctx_t* ctx, skl_gc_rescan_header_t* obj);

#endif
