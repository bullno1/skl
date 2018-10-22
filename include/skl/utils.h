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

#define SKL_ERROR(CTX, TYPE) \
	skl_set_error((CTX), (skl_error_t) { \
		.type = SKL_STRING_REF(TYPE), \
		.file = SKL_STRING_REF(__FILE__), \
		.location = { \
			.start =  { .line = __LINE__ }, \
			.end =  { .line = __LINE__ }, \
		} \
	})

#define SKL_ASSERT(CTX, COND, TYPE) \
	do { \
		if(!(COND)) { return SKL_ERROR(CTX, TYPE); } \
	} while(0)

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

#endif
