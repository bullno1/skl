#ifndef SICKLE_UTILS_H
#define SICKLE_UTILS_H

#include <skl/skl.h>
#include <bk/macro.h>
#include <string.h>

#define SKL_STRING_REF(STR) \
	(skl_string_ref_t) { \
		.length = BK_STATIC_ARRAY_LEN(STR) - 1, \
		.ptr = STR \
	}

#define SKL_CHECK(OP) \
	do { \
		skl_exec_status_t status = OP; \
		if(status != SKL_EXEC_OK) { return status; } \
	} while(0)

BK_INLINE skl_string_ref_t
skl_string_ref(const char* string)
{
	return (skl_string_ref_t) {
		.length = strlen(string),
		.ptr = string
	};
}

BK_INLINE skl_exec_status_t
skl_error(skl_ctx_t* ctx, skl_string_ref_t error)
{
	skl_push_string(ctx, error);
	return SKL_EXEC_ERROR;
}

#endif
