#ifndef SICKLE_INTERNAL_H
#define SICKLE_INTERNAL_H

#include <skl/skl.h>
#include <skl/utils.h>
#ifndef SKL_NO_NAN_BOXING
#include <nanbox/nanbox.h>
#endif


#define SKL_ASSERT(CTX, COND, MSG) \
	do { if(!(COND)) { skl_panic(CTX, SKL_STRING_REF(MSG)); } } while(0);


typedef struct skl_string_s skl_string_t;
typedef struct skl_list_s skl_list_t;
typedef struct skl_map_s skl_map_t;
typedef struct skl_box_s skl_box_t;
typedef struct skl_procedure_s skl_procedure_t;
#ifdef SKL_NO_NAN_BOXING
typedef struct skl_value_s skl_value_t;
#else
typedef nanbox_t skl_value_t;
#endif


void
skl_panic(skl_ctx_t* ctx, skl_string_ref_t msg);

#endif
