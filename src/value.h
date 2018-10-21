#ifndef SICKLE_VALUE_H
#define SICKLE_VALUE_H

#include "internal.h"


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
