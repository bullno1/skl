#ifndef SICKLE_STRPOOL_H
#define SICKLE_STRPOOL_H

#include "common.h"
#include "vendor/khash.h"


typedef struct skl_strpool_key_s skl_strpool_key_t;


struct skl_strpool_key_s
{
	uint32_t hash;
	skl_string_ref_t ref;
};

KHASH_DECLARE(skl_strpool, skl_strpool_key_t, skl_string_t*)
typedef khash_t(skl_strpool) skl_strpool_t;


void
skl_strpool_init(skl_ctx_t* ctx);

void
skl_strpool_cleanup(skl_ctx_t* ctx);

skl_string_t*
skl_strpool_alloc(skl_ctx_t* ctx, skl_string_ref_t string);

#endif
