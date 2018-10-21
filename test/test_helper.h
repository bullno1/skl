#ifndef SKL_TEST_HELPER_H
#define SKL_TEST_HELPER_H

#include <bk/allocator.h>
#include <bk/default_allocator.h>
#include <skl.h>
#include <munit/munit.h>


#define skl_assert_enum(ENUM_TYPE, EXPECTED, OP, ACTUAL) \
	do { \
		unsigned int expected = (EXPECTED); \
		unsigned int actual = (ACTUAL); \
		if(!(expected OP actual)) { \
			munit_errorf( \
				"assert failed: " #EXPECTED " " #OP " " #ACTUAL " (%s " #OP " %s)", \
				ENUM_TYPE##_to_str(expected), ENUM_TYPE##_to_str(actual) \
			); \
		} \
	} while (0)

#define skl_assert_string_ref_equal(EXPECTED, ACTUAL) \
	do { \
		skl_string_ref_t expected = (EXPECTED); \
		skl_string_ref_t actual = (ACTUAL); \
		if(!skl_string_ref_equal(expected, actual)) { \
			munit_errorf( \
				"assert failed: " #EXPECTED " == " #ACTUAL " (\"%.*s\" == \"%.*s\")", \
				(int)expected.length, expected.ptr, \
				(int)actual.length, actual.ptr \
			); \
		} \
	} while (0)

#define skl_assert_loc_equal(EXPECTED, ACTUAL) \
	do { \
		skl_location_t loc_expected = (EXPECTED); \
		skl_location_t loc_actual = (ACTUAL); \
		munit_assert_uint(loc_expected.column, ==, loc_actual.column); \
		munit_assert_uint(loc_expected.line, ==, loc_actual.line); \
	} while (0)

#define skl_assert_loc_range_equal(EXPECTED, ACTUAL) \
	do { \
		skl_loc_range_t loc_range_expected = (EXPECTED); \
		skl_loc_range_t loc_range_actual = (ACTUAL); \
		skl_assert_loc_equal(loc_range_expected.start, loc_range_actual.start); \
		skl_assert_loc_equal(loc_range_expected.end, loc_range_actual.end); \
	} while(0)


BK_INLINE skl_ctx_t*
create_ctx()
{
	skl_config_t cfg = {
		.allocator = bk_default_allocator,
		.operand_stack_size = 256,
		.call_stack_size = 256,
	};
	return skl_create_ctx(&cfg);
}

BK_INLINE skl_string_ref_t
string_ref(const char* str)
{
	return (skl_string_ref_t) {
		.ptr = str,
		.length = strlen(str)
	};
}

BK_INLINE bool
skl_string_ref_equal(skl_string_ref_t lhs, skl_string_ref_t rhs)
{
	return lhs.length == rhs.length
		&& strncmp(lhs.ptr, rhs.ptr, lhs.length) == 0;
}

BK_INLINE void*
setup_ctx(const MunitParameter params[], void* data)
{
	(void)params;
	(void)data;

	return create_ctx();
}

BK_INLINE void
teardown_ctx(void* data)
{
	skl_destroy_ctx(data);
}

#endif
