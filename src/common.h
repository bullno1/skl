#ifndef SICKLE_INTERNAL_H
#define SICKLE_INTERNAL_H

#include <skl/skl.h>
#include <skl/utils.h>
#include <bk/array.h>


#define SKL_ASSERT(CTX, COND, MSG) \
	do { if(!(COND)) { skl_panic(CTX, SKL_STRING_REF(MSG)); } } while(0);


typedef struct skl_value_s skl_value_t;
typedef struct skl_string_s skl_string_t;
typedef struct skl_list_s skl_list_t;


void
skl_panic(skl_ctx_t* ctx, skl_string_ref_t msg);

#endif
