#ifndef SICKLE_ASM_H
#define SICKLE_ASM_H

#include "common.h"
#include "vm.h"


struct bk_allocator_s;


typedef struct skl_asm_s skl_asm_t;
typedef struct skl_tagged_instruction_s skl_tagged_instruction_t;


struct skl_tagged_instruction_s
{
	skl_debug_info_t debug_info;
	skl_instruction_t instruction;
};


struct skl_asm_s
{
	BK_ARRAY(skl_tagged_instruction_t) instructions;
	BK_ARRAY(skl_string_t*) string_pool;
	BK_ARRAY(double) number_pool;
	BK_ARRAY(skl_code_chunk_t*) children;
	BK_ARRAY(skl_resource_index_t) imports;
};


void
skl_asm_begin(
	skl_asm_t* sasm,
	skl_ctx_t* ctx,
	skl_arg_index_t env_len,
	skl_arg_index_t num_args,
	bool is_vararg
);

skl_code_chunk_t*
skl_asm_end(skl_asm_t* sasm);

skl_resource_index_t
skl_asm_add_number(skl_asm_t* sasm, double number);

skl_resource_index_t
skl_asm_add_string(skl_asm_t* sasm, skl_string_ref_t string);

skl_resource_index_t
skl_asm_add_code_chunk(skl_asm_t* sasm, skl_code_chunk_t* chunk);

skl_resource_index_t
skl_asm_add_import(skl_asm_t* sasm, skl_string_ref_t string);

skl_resource_index_t
skl_asm_new_label(skl_asm_t* sasm);

void
skl_asm_emit(
	skl_asm_t* sasm,
	skl_opcode_t opcode,
	skl_operand_t operand,
	skl_loc_range_t location,
	skl_string_ref_t file
);

#endif
