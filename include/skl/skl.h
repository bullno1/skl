#ifndef SICKLE_H
#define SICKLE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <bk/macro.h>


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
struct bk_file_s;


typedef struct skl_ctx_s skl_ctx_t;
typedef struct skl_config_s skl_config_t;
typedef struct skl_string_ref_s skl_string_ref_t;
typedef struct skl_location_s skl_location_t;
typedef struct skl_loc_range_s skl_loc_range_t;
typedef uint16_t skl_source_addr_t;
typedef int32_t skl_index_t;


#define SKL_VAL(X) \
	X(SKL_VAL_NULL) \
	X(SKL_VAL_NUMBER) \
	X(SKL_VAL_STRING) \
	X(SKL_VAL_BOX) \
	X(SKL_VAL_LIST) \
	X(SKL_VAL_MAP) \
	X(SKL_VAL_PROC) \
	X(SKL_VAL_OPAQUE) \

BK_ENUM(skl_value_type_t, SKL_VAL)

#define SKL_GC(X) \
	X(SKL_GC_STEP) \
	X(SKL_GC_COLLECT) \
	X(SKL_GC_PAUSE) \
	X(SKL_GC_UNPAUSE) \

BK_ENUM(skl_gc_op_t, SKL_GC)

#define SKL_EXEC_ERROR (-1)

typedef enum skl_proc_flag_e
{
	SKL_PROC_SYNTAX = 1
} skl_proc_flag_t;

typedef enum skl_dump_flag_e
{
	SKL_DUMP_EXTERNAL = 1,
} skl_dump_flag_t;

typedef int(*skl_native_proc_t)(skl_ctx_t*);
typedef void(*skl_panic_fn_t)(skl_ctx_t* ctx, const char* fmt, va_list args);


struct skl_config_s
{
	struct bk_allocator_s* allocator;
	skl_panic_fn_t panic_handler;

	uint16_t operand_stack_size;
	uint16_t call_stack_size;
};

struct skl_string_ref_s
{
	const char* ptr;
	size_t length;
};

struct skl_location_s
{
	skl_source_addr_t line;
	skl_source_addr_t column;
};

struct skl_loc_range_s
{
	skl_location_t start;
	skl_location_t end;
};

// Context

SKL_API skl_ctx_t*
skl_create_ctx(const skl_config_t* cfg);

SKL_API void
skl_destroy_ctx(skl_ctx_t* ctx);

// Stack manipulation

SKL_API bool
skl_check_stack(skl_ctx_t* ctx, skl_index_t count);

SKL_API skl_index_t
skl_stack_len(skl_ctx_t* ctx);

SKL_API void
skl_resize_stack(skl_ctx_t* ctx, skl_index_t size);

SKL_API void
skl_push_null(skl_ctx_t* ctx);

SKL_API void
skl_push_number(skl_ctx_t* ctx, double number);

SKL_API void
skl_push_string(skl_ctx_t* ctx, skl_string_ref_t str);

SKL_API void
skl_push_string_fmtv(skl_ctx_t* ctx, const char* fmt, va_list args);

SKL_API void
skl_push_native_proc(skl_ctx_t* ctx, skl_native_proc_t proc);

SKL_API void
skl_dup(skl_ctx_t* ctx, skl_index_t index);

SKL_API void
skl_replace(skl_ctx_t* ctx, skl_index_t index);

// Value inspection

SKL_API bool
skl_check_type(skl_ctx_t* ctx, skl_index_t index, skl_value_type_t type);

SKL_API skl_index_t
skl_len(skl_ctx_t* ctx, skl_index_t index);

// Simple type

SKL_API skl_string_ref_t
skl_as_string(skl_ctx_t* ctx, skl_index_t index);

SKL_API double
skl_as_number(skl_ctx_t* ctx, skl_index_t index);

// Box

SKL_API void
skl_box_new(skl_ctx_t* ctx);

SKL_API void
skl_box_wrap(skl_ctx_t* ctx, int index);

SKL_API void
skl_box_unwrap(skl_ctx_t* ctx, int index);

SKL_API void
skl_box_put(skl_ctx_t* ctx, int index);

// List

SKL_API void
skl_list_new(skl_ctx_t* ctx, skl_index_t capacity);

SKL_API void
skl_list_get(skl_ctx_t* ctx, skl_index_t index, skl_index_t n);

SKL_API void
skl_list_set(skl_ctx_t* ctx, skl_index_t index, skl_index_t n);

SKL_API void
skl_list_push(skl_ctx_t* ctx, skl_index_t index);

SKL_API void
skl_list_insert(skl_ctx_t* ctx, skl_index_t index, skl_index_t n);

SKL_API void
skl_list_delete(skl_ctx_t* ctx, skl_index_t index, skl_index_t n);

SKL_API void
skl_list_quick_delete(skl_ctx_t* ctx, skl_index_t index, skl_index_t n);

// Map

SKL_API void
skl_map_new(skl_ctx_t* ctx);

SKL_API void
skl_map_set(skl_ctx_t* ctx, skl_index_t index);

SKL_API bool
skl_map_get(skl_ctx_t* ctx, skl_index_t index);

SKL_API bool
skl_map_has(skl_ctx_t* ctx, skl_index_t index);

SKL_API bool
skl_map_delete(skl_ctx_t* ctx, skl_index_t index);

// Garbage collection

SKL_API void
skl_gc(skl_ctx_t* ctx, skl_gc_op_t op);

SKL_API skl_index_t
skl_gc_ref(skl_ctx_t* ctx, skl_index_t index);

SKL_API void
skl_gc_deref(skl_ctx_t* ctx, skl_index_t handle);

SKL_API void
skl_gc_unref(skl_ctx_t* ctx, skl_index_t handle);

#endif
