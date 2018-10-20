#ifndef SKL_H
#define SKL_H

#include <stddef.h>
#include <bk/macro.h>
#include <stdbool.h>

#if SKL_DYNAMIC == 1
#	if SKL_BUILD == 1
#		define SKL_API BK_EXTERN BK_DYNAMIC_EXPORT
#	else
#		define SKL_API BK_EXTERN BK_DYNAMIC_IMPORT
#	endif
#else
#	define SKL_API BK_EXTERN
#endif


struct bk_allocator_s;

typedef struct skl_ctx_s skl_ctx_t;
typedef struct skl_config_s skl_config_t;
typedef struct skl_string_ref_s skl_string_ref_t;


#define SKL_VAL(X) \
	X(SKL_VAL_NULL) \
	X(SKL_VAL_BOOLEAN) \
	X(SKL_VAL_NUMBER) \
	X(SKL_VAL_STRING) \
	X(SKL_VAL_LIST) \
	X(SKL_VAL_PROC) \
	X(SKL_VAL_OPAQUE) \

BK_ENUM(skl_value_type_t, SKL_VAL)


struct skl_config_s
{
	struct bk_allocator_s* allocator;
};

struct skl_string_ref_s
{
	const char* ptr;
	size_t length;
};

SKL_API skl_ctx_t*
skl_create_ctx(skl_config_t* cfg);

SKL_API void
skl_destroy_ctx(skl_ctx_t* ctx);

SKL_API skl_value_type_t
skl_value_type(skl_ctx_t* ctx, int index);

#endif
