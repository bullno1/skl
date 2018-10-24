#ifndef SICKLE_VM_H
#define SICKLE_VM_H

#include "common.h"
#include "opcode.h"
#include "gc.h"
#include "value.h"
#include "box.h"


#define SKL_SAFE_STACK_ADDR(ADDR, CTX, INDEX) \
	do { \
		skl_vm_t* vm = ctx->vm; \
		ADDR = skl_vm_stack_addr(vm, (INDEX)); \
		SKL_ASSERT((CTX), vm->fp->bp <= (ADDR) && (ADDR) < vm->sp, "Invalid stack index") \
	} while(0);

#define SKL_GET_OBJ(VALUE, CTX, INDEX, TYPE) \
	do { \
		skl_value_t* value; \
		SKL_SAFE_STACK_ADDR(value, CTX, INDEX); \
		SKL_ASSERT( \
			ctx, \
			skl_value_check_type(*value, TYPE), \
			"Type error: Value at #%d is not %s", \
			(INDEX), \
			skl_value_type_t_to_str(TYPE) \
		); \
		VALUE = skl_value_as_ref(*value); \
	} while(0);


typedef struct skl_code_chunk_s skl_code_chunk_t;
typedef struct skl_debug_info_s skl_debug_info_t;
typedef struct skl_import_s skl_import_t;
typedef struct skl_stack_frame_s skl_stack_frame_t;
typedef uint16_t skl_resource_index_t;
typedef uint8_t skl_arg_index_t;


struct skl_import_s
{
	skl_resource_index_t name;
	skl_box_t* box;
};

struct skl_code_chunk_s
{
	skl_gc_header_t gc_header;

	skl_resource_index_t num_instructions;
	skl_resource_index_t number_pool_size;
	skl_resource_index_t string_pool_size;
	skl_resource_index_t num_imports;
	skl_resource_index_t num_children;
	skl_resource_index_t num_locals;
	skl_arg_index_t num_args;
	unsigned is_vararg:1;

	BK_ARRAY(skl_instruction_t) instructions;
	BK_ARRAY(skl_debug_info_t) debug_info;
	BK_ARRAY(double) number_pool;
	BK_ARRAY(skl_string_t*) string_pool;
	BK_ARRAY(skl_import_t) imports;
	BK_ARRAY(skl_code_chunk_t*) children;
};

struct skl_debug_info_s
{
	skl_loc_range_t location;
	skl_string_t* path;
};

struct skl_procedure_s
{
	skl_gc_header_t gc_header;

	skl_string_t* debug_name;

	union
	{
		skl_code_chunk_t* skl;
		skl_native_proc_t native;
	} code;

	unsigned env_len:8;
	unsigned is_native:1;

	BK_FLEXIBLE_ARRAY_MEMBER(skl_value_t, env);
};

struct skl_stack_frame_s
{
	skl_instruction_t* pc;
	skl_value_t* bp;
	skl_arg_index_t num_args;
};

struct skl_vm_s
{
	skl_gc_rescan_header_t gc_header;

	skl_value_t *sp_min, *sp_max, *sp;
	skl_stack_frame_t *fp_min, *fp_max, *fp;
};

skl_vm_t*
skl_vm_alloc(skl_ctx_t* ctx);

void
skl_vm_push_ref(skl_ctx_t* ctx, skl_value_type_t type, void* ref);

void
skl_vm_push_value(skl_ctx_t* ctx, skl_value_t value);

skl_value_t*
skl_vm_stack_addr(const skl_vm_t* vm, int index);

skl_value_t
skl_vm_pop(skl_ctx_t* ctx);

#endif
