#ifndef SICKLE_CTX_H
#define SICKLE_CTX_H

#include "common.h"
#include "lexer.h"
#include "gc.h"
#include "strpool.h"


struct skl_ctx_s
{
	skl_config_t cfg;

	skl_gc_t gc;
	skl_strpool_t strpool;
	skl_lexer_t lexer;
};

#endif
