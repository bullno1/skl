#ifndef SICKLE_INTERNAL_H
#define SICKLE_INTERNAL_H

#include <skl.h>
#include <stdint.h>
#include <bk/array.h>


typedef struct skl_location_s skl_location_t;
typedef struct skl_loc_range_s skl_loc_range_t;


struct skl_string_ref_t
{
	size_t length;
	const char* ptr;
};

struct skl_location_s
{
	uint16_t line;
	uint16_t column;
};

struct skl_loc_range_s
{
	skl_location_t start;
	skl_location_t end;
};

#endif
