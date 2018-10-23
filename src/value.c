#include "value.h"

#ifdef SKL_NO_NAN_BOXING

bool
skl_value_is_ref(skl_value_t value)
{
	return value.type != SKL_VAL_NULL && value.type != SKL_VAL_NUMBER;
}

void*
skl_value_as_ref(skl_value_t value)
{
	return value.data.ref;
}

double
skl_value_as_number(skl_value_t value)
{
	return value.data.number;
}

skl_value_type_t
skl_value_type(skl_value_t value)
{
	return value.type;
}

skl_value_t
skl_value_make_null()
{
	return (skl_value_t) {
		.type = SKL_VAL_NULL
	};
}

skl_value_t
skl_value_make_number(double number)
{
	return (skl_value_t) {
		.type = SKL_VAL_NUMBER,
		.data = { .number = number }
	};
}

skl_value_t
skl_value_make_ref(skl_value_type_t type, void* ref)
{
	return (skl_value_t) {
		.type = type,
		.data = { .ref = ref }
	};
}

#else

#include "gc.h"

extern const skl_gc_info_t skl_string_gc_info;
extern const skl_gc_info_t skl_list_gc_info;
extern const skl_gc_info_t skl_vm_gc_info;

bool
skl_value_is_ref(skl_value_t value)
{
	return nanbox_is_pointer(value);
}

void*
skl_value_as_ref(skl_value_t value)
{
	return nanbox_to_pointer(value);
}

double
skl_value_as_number(skl_value_t value)
{
	return nanbox_to_double(value);
}

skl_value_type_t
skl_value_type(skl_value_t value)
{
	if(nanbox_is_null(value))
	{
		return SKL_VAL_NULL;
	}
	else if(nanbox_is_double(value))
	{
		return SKL_VAL_NUMBER;
	}
	else
	{
		const skl_gc_header_t* ptr = nanbox_to_pointer(value);
		const skl_gc_info_t* gc_info = ptr->gc_info;
		if(gc_info == &skl_string_gc_info)
		{
			return SKL_VAL_STRING;
		}
		else if(gc_info == &skl_list_gc_info)
		{
			return SKL_VAL_LIST;
		}
		else
		{
			return SKL_VAL_NULL;
		}
	}
}

skl_value_t
skl_value_make_null()
{
	return nanbox_null();
}

skl_value_t
skl_value_make_number(double number)
{
	return nanbox_from_double(number);
}

skl_value_t
skl_value_make_ref(skl_value_type_t type, void* ref)
{
	(void)type;
	return nanbox_from_pointer(ref);
}

#endif
