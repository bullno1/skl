#include "../src/context.h"
#include "../src/list.h"
#include "test_helper.h"


static MunitResult
stack(const MunitParameter params[], void* fixture)
{
	(void)params;

	skl_ctx_t* ctx = fixture;

	skl_push_null(ctx);
	skl_push_string(ctx, SKL_STRING_REF("wat"));
	skl_push_number(ctx, 4.9);

	munit_assert_int(3, ==, skl_stack_len(ctx));
	skl_assert_enum(skl_value_type_t, SKL_VAL_NULL, ==, skl_type(ctx, 0));
	skl_assert_enum(skl_value_type_t, SKL_VAL_STRING, ==, skl_type(ctx, 1));
	skl_assert_enum(skl_value_type_t, SKL_VAL_NUMBER, ==, skl_type(ctx, 2));

	return MUNIT_OK;
}


static MunitTest tests[] = {
	{
		.name = "/stack",
		.test = stack,
		.setup = setup_ctx,
		.tear_down = teardown_ctx
	},
	{ .test = NULL }
};

MunitSuite vm = {
	.prefix = "/vm",
	.tests = tests
};
