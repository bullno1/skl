#ifndef SICKLE_CTX_H
#define SICKLE_CTX_H

#include "internal.h"
#include "lexer.h"
#include "gc.h"
#include "strpool.h"


struct skl_ctx_s
{
	skl_config_t cfg;

	ugc_t gc;
	unsigned int gc_pause;
	skl_lexer_t lexer;
	skl_strpool_t strpool;
};

#endif
