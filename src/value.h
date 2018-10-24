#ifndef SICKLE_VALUE_H
#define SICKLE_VALUE_H

#include "common.h"

#ifdef SKL_NO_NAN_BOXING

struct skl_value_s
{
	skl_value_type_t type;
	union
	{
		double number;
		void* ref;
	} data;
};

#endif


bool
skl_value_is_ref(skl_value_t value);

void*
skl_value_as_ref(skl_value_t value);

double
skl_value_as_number(skl_value_t value);

bool
skl_value_check_type(skl_value_t value, skl_value_type_t type);

skl_value_t
skl_value_make_null();

skl_value_t
skl_value_make_number(double number);

skl_value_t
skl_value_make_ref(skl_value_type_t type, void* ref);

#endif
