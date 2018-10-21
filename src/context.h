#ifndef SICKLE_CTX_H
#define SICKLE_CTX_H

#include "common.h"
#include "lexer.h"
#include "gc.h"
#include "vm.h"
#include "strpool.h"


struct skl_ctx_s
{
	skl_config_t cfg;

	skl_gc_t gc;
	skl_strpool_t strpool;
	skl_lexer_t lexer;

	skl_vm_t main_vm;
	skl_vm_t* vm;
};

#endif
