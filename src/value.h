#ifndef SICKLE_VALUE_H
#define SICKLE_VALUE_H

#include "common.h"


struct skl_value_s
{
	skl_value_type_t type;
	union
	{
		double number;
		void* ref;
	} data;
};


BK_INLINE bool
skl_value_is_ref(skl_value_t value)
{
	return value.type != SKL_VAL_NULL && value.type != SKL_VAL_NUMBER;
}

BK_INLINE void*
skl_value_as_ref(skl_value_t value)
{
	return value.data.ref;
}

BK_INLINE double
skl_value_as_number(skl_value_t value)
{
	return value.data.number;
}

BK_INLINE skl_value_type_t
skl_value_type(skl_value_t value)
{
	return value.type;
}

#endif
