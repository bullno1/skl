#ifndef SICKLE_OPCODE_H
#define SICKLE_OPCODE_H

#include "common.h"


#define SKL_OP(X) \
	X(SKL_OP_NULL) \
	X(SKL_OP_POP) \
	X(SKL_OP_LOAD_INT) \
	X(SKL_OP_LOAD_NUM) \
	X(SKL_OP_LOAD_STR) \
	X(SKL_OP_GET_LOCAL) \
	X(SKL_OP_GET_GLOBAL) \
	X(SKL_OP_GET_ENV) \
	X(SKL_OP_SET_LOCAL) \
	X(SKL_OP_SET_GLOBAL) \
	X(SKL_OP_SETU) \
	X(SKL_OP_CALL) \
	X(SKL_OP_TAIL) \
	X(SKL_OP_GCAL) \
	X(SKL_OP_GTAL) \
	X(SKL_OP_RET) \

BK_ENUM(skl_opcode_t, SKL_OP)

typedef int32_t skl_instruction_t;
typedef int32_t skl_operand_t;

#endif
