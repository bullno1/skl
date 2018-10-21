#ifndef SICKLE_LIST_H
#define SICKLE_LIST_H

#include "internal.h"
#include "gc.h"


typedef struct skl_list_s skl_list_t;


struct skl_list_s
{
	skl_gc_header_t gc_header;
	BK_ARRAY(skl_value_t) elements;
};

#endif
