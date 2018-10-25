#ifndef SICKLE_INTERNAL_H
#define SICKLE_INTERNAL_H

#include <setjmp.h>
#include <skl/skl.h>
#include <skl/utils.h>
#ifndef SKL_NO_NAN_BOXING
#include <nanbox/nanbox.h>
#endif


#define SKL_ASSERT(CTX, COND, ...) \
	do { if(!(COND)) { skl_throw(CTX, __VA_ARGS__); } } while(0);
#define SKL_SET_ERROR_LONGJMP(CTX, TRAP) \
	setjmp(*skl_prepare_longjmp((CTX), (TRAP)))


typedef struct skl_string_s skl_string_t;
typedef struct skl_list_s skl_list_t;
typedef struct skl_map_s skl_map_t;
typedef struct skl_box_s skl_box_t;
typedef struct skl_procedure_s skl_procedure_t;
typedef struct skl_vm_s skl_vm_t;
typedef struct skl_trap_s skl_trap_t;
#ifdef SKL_NO_NAN_BOXING
typedef struct skl_value_s skl_value_t;
#else
typedef nanbox_t skl_value_t;
#endif


struct skl_trap_s
{
	jmp_buf jmp_buf;
	skl_panic_fn_t panic_handler;
};


skl_trap_t
skl_set_trap(skl_ctx_t* ctx, skl_trap_t trap);

skl_trap_t
skl_set_error_panic(skl_ctx_t* ctx, skl_panic_fn_t panic_handler);

jmp_buf*
skl_prepare_longjmp(skl_ctx_t* ctx, skl_trap_t* old);

void
skl_throw(skl_ctx_t* ctx, const char* fmt, ...);

#endif
