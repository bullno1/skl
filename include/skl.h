#ifndef SICKLE_H
#define SICKLE_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
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

#define SKL_STRING_REF(STR) \
	(skl_string_ref_t) { \
		.length = BK_STATIC_ARRAY_LEN(STR) - 1, \
		.ptr = STR \
	}


struct bk_allocator_s;

typedef struct skl_ctx_s skl_ctx_t;
typedef struct skl_config_s skl_config_t;
typedef struct skl_string_ref_s skl_string_ref_t;
typedef struct skl_error_s skl_error_t;
typedef struct skl_location_s skl_location_t;
typedef struct skl_loc_range_s skl_loc_range_t;
typedef unsigned int skl_gc_handle_t;
typedef uint16_t skl_source_addr_t;


#define SKL_VAL(X) \
	X(SKL_VAL_NULL) \
	X(SKL_VAL_NUMBER) \
	X(SKL_VAL_STRING) \
	X(SKL_VAL_BOX) \
	X(SKL_VAL_LIST) \
	X(SKL_VAL_PROC) \
	X(SKL_VAL_OPAQUE) \

BK_ENUM(skl_value_type_t, SKL_VAL)

#define SKL_EXEC(X) \
	X(SKL_EXEC_OK) \
	X(SKL_EXEC_ERROR) \

BK_ENUM(skl_exec_status_t, SKL_EXEC)

#define SKL_GC(X) \
	X(SKL_GC_STEP) \
	X(SKL_GC_COLLECT) \
	X(SKL_GC_PAUSE) \
	X(SKL_GC_UNPAUSE) \

BK_ENUM(skl_gc_op_t, SKL_GC)

typedef enum skl_reset_e
{
	SKL_RESET_EXEC = 1,
	SKL_RESET_GLOBALS = 1 << 1,
	SKL_RESET_GC = 1 << 2,
	SKL_RESET_ALL = 0
		| SKL_RESET_EXEC
		| SKL_RESET_GLOBALS
		| SKL_RESET_GC,
} skl_reset_t;

typedef enum skl_proc_flag_e
{
	SKL_PROC_SYNTAX = 1
} skl_proc_flag_t;

typedef skl_exec_status_t(*skl_native_proc_t)(skl_ctx_t*);


struct skl_config_s
{
	struct bk_allocator_s* allocator;

	uint32_t operand_stack_size;
	uint32_t call_stack_size;
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

struct skl_error_s
{
	skl_string_ref_t type;
	skl_string_ref_t file;
	skl_loc_range_t location;
};

// Context

SKL_API skl_ctx_t*
skl_create_ctx(skl_config_t* cfg);

SKL_API void
skl_destroy_ctx(skl_ctx_t* ctx);

SKL_API void
skl_reset_ctx(skl_ctx_t* ctx, skl_reset_t reset);

SKL_API skl_error_t
skl_last_error(skl_ctx_t* ctx);

// Stack manipulation

SKL_API skl_exec_status_t
skl_push_null(skl_ctx_t* ctx);

SKL_API skl_exec_status_t
skl_push_number(skl_ctx_t* ctx, double number);

SKL_API skl_exec_status_t
skl_push_string(skl_ctx_t* ctx, skl_string_ref_t str);

SKL_API skl_exec_status_t
skl_push_native_proc(skl_ctx_t* ctx, skl_native_proc_t proc);

SKL_API skl_exec_status_t
skl_dup(skl_ctx_t* ctx, int index);

SKL_API skl_exec_status_t
skl_pop(skl_ctx_t* ctx, int count);

SKL_API int
skl_get_top(skl_ctx_t* ctx);

SKL_API skl_exec_status_t
skl_set_top(skl_ctx_t* ctx, int len);

// Global

SKL_API skl_exec_status_t
skl_get_global(skl_ctx_t* ctx, int index);

SKL_API skl_exec_status_t
skl_set_global(skl_ctx_t* ctx, int index);

// Value inspection

SKL_API skl_value_type_t
skl_type(skl_ctx_t* ctx, int index);

SKL_API skl_exec_status_t
skl_len(skl_ctx_t* ctx, int index);

// Simple type

SKL_API skl_exec_status_t
skl_to_string(skl_ctx_t* ctx, int index, skl_string_ref_t* ref);

SKL_API skl_exec_status_t
skl_to_number(skl_ctx_t* ctx, int index, double* number);

// List

SKL_API skl_exec_status_t
skl_list_new(skl_ctx_t* ctx, int capacity);

SKL_API skl_exec_status_t
skl_list_get(skl_ctx_t* ctx, int index, int n);

SKL_API skl_exec_status_t
skl_list_set(skl_ctx_t* ctx, int index, int n);

SKL_API skl_exec_status_t
skl_list_resize(skl_ctx_t* ctx, int index);

SKL_API skl_exec_status_t
skl_list_push(skl_ctx_t* ctx, int index);

SKL_API skl_exec_status_t
skl_list_pop(skl_ctx_t* ctx, int index);

// Box

SKL_API skl_exec_status_t
skl_box_new(skl_ctx_t* ctx);

SKL_API skl_exec_status_t
skl_box_wrap(skl_ctx_t* ctx, int index);

SKL_API skl_exec_status_t
skl_box_unwrap(skl_ctx_t* ctx, int index);

SKL_API skl_exec_status_t
skl_box_put(skl_ctx_t* ctx, int index);

// Procedure

SKL_API skl_exec_status_t
skl_call(skl_ctx_t* ctx, int num_args);

SKL_API skl_exec_status_t
skl_get_proc_flags(skl_ctx_t* ctx, int index, skl_proc_flag_t* flags);

SKL_API skl_exec_status_t
skl_set_proc_flags(skl_ctx_t* ctx, int index, skl_proc_flag_t flags);

// Garbage collection

SKL_API void
skl_gc(skl_ctx_t* ctx, skl_gc_op_t op);

SKL_API skl_gc_handle_t
skl_ref(skl_ctx_t* ctx, int index);

SKL_API void
skl_deref(skl_ctx_t* ctx, skl_gc_handle_t handle);

SKL_API void
skl_unref(skl_ctx_t* ctx, skl_gc_handle_t handle);

// Helper

BK_INLINE skl_string_ref_t
skl_string_ref(const char* string)
{
	return (skl_string_ref_t) {
		.length = strlen(string),
		.ptr = string
	};
}

#endif
