#ifndef SICKLE_UTILS_H
#define SICKLE_UTILS_H

#include <skl/skl.h>
#include <bk/macro.h>
#include <string.h>
#include <bk/printf.h>


#define SKL_STRING_REF(STR) \
	(skl_string_ref_t) { \
		.length = BK_STATIC_ARRAY_LEN(STR) - 1, \
		.ptr = STR \
	}

#define SKL_ERROR_FMT(CTX, FMT, ...) \
	do { \
		skl_push_string_fmt(CTX, FMT, __VA_ARGS__); \
		return SKL_EXEC_ERROR; \
	} while(0)

#define SKL_ERROR(CTX, ERROR) \
	do { \
		skl_push_string(CTX, SKL_STRING_REF(ERROR)); \
		return SKL_EXEC_ERROR; \
	} while(0)


BK_INLINE skl_string_ref_t
skl_string_ref(const char* string)
{
	return (skl_string_ref_t) {
		.length = strlen(string),
		.ptr = string
	};
}

BK_PRINTF_LIKE(2, 3)
BK_INLINE void
skl_push_string_fmt(skl_ctx_t* ctx, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	skl_push_string_fmtv(ctx, fmt, args);
	va_end(args);
}

#endif
