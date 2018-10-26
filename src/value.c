#include "value.h"
#define XXH_INLINE_ALL
#define XXH_PRIVATE_API
#define XXH_NAMESPACE skl_
#include <xxHash/xxhash.h>

#ifdef SKL_NO_NAN_BOXING

#include <string.h>

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

bool
skl_value_check_type(skl_value_t value, skl_value_type_t type)
{
	return value.type == type;
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

khint_t
skl_value_equal(skl_value_t lhs, skl_value_t rhs)
{
	return lhs.type == rhs.type && lhs.data.ref == rhs.data.ref;
}

khint_t
skl_value_hash(skl_value_t value)
{
	// Hash type and data separately because there can be padding junk in between
	XXH32_state_t state;
	XXH32_reset(&state, __LINE__); // TODO: better seed (per map seed?)
	XXH32_update(&state, &cell.type, sizeof(cell.type));
	XXH32_update(&state, &cell.data, sizeof(cell.data));
	return XXH32_digest(&state);
}

#else

#include "gc.h"

extern const skl_gc_info_t skl_string_gc_info;
extern const skl_gc_info_t skl_list_gc_info;
extern const skl_gc_info_t skl_vm_gc_info;


static bool
skl_value_check_ref_type(skl_value_t value, const skl_gc_info_t* gc_info);


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

bool
skl_value_check_type(skl_value_t value, skl_value_type_t type)
{
	switch(type)
	{
		case SKL_VAL_NULL:
			return nanbox_is_null(value);
		case SKL_VAL_NUMBER:
			return nanbox_is_double(value);
		case SKL_VAL_LIST:
			return skl_value_check_ref_type(value, &skl_list_gc_info);
		case SKL_VAL_STRING:
			return skl_value_check_ref_type(value, &skl_string_gc_info);
		default:
			return false;
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


bool
skl_value_check_ref_type(skl_value_t value, const skl_gc_info_t* gc_info)
{
	if(!nanbox_is_pointer(value)) { return false; }

	const skl_gc_header_t* header = nanbox_to_pointer(value);
	return header->gc_info == gc_info;
}

khint_t
skl_value_equal(skl_value_t lhs, skl_value_t rhs)
{
	return memcmp(&lhs, &rhs, sizeof(skl_value_t)) == 0;
}

khint_t
skl_value_hash(skl_value_t value)
{
	return XXH32(&value, sizeof(value), __LINE__);
}

#endif
