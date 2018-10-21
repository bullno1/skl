#ifndef SICKLE_STRING_H
#define SICKLE_STRING_H

#include "common.h"
#include "gc.h"


struct skl_string_s
{
	skl_gc_header_t gc_header;

	uint32_t hash;
	size_t length;
	BK_FLEXIBLE_ARRAY_MEMBER(char, content);
};

#endif
