#ifndef SICKLE_BOX_H
#define SICKLE_BOX_H

#include "common.h"
#include "gc.h"
#include "value.h"


struct skl_box_s
{
	skl_gc_header_t gc_header;
	skl_value_t value;
};

#endif
