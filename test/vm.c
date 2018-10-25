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
	munit_assert(skl_check_type(ctx, 0, SKL_VAL_NULL));
	munit_assert(skl_check_type(ctx, 1, SKL_VAL_STRING));
	munit_assert(skl_check_type(ctx, 2, SKL_VAL_NUMBER));

	munit_assert(skl_check_type(ctx, -3, SKL_VAL_NULL));
	munit_assert(skl_check_type(ctx, -2, SKL_VAL_STRING));
	munit_assert(skl_check_type(ctx, -1, SKL_VAL_NUMBER));

	skl_dup(ctx, 1);
	munit_assert_int(4, ==, skl_stack_len(ctx));
	munit_assert(skl_check_type(ctx, -1, SKL_VAL_STRING));
	skl_resize_stack(ctx, -1);
	munit_assert_int(3, ==, skl_stack_len(ctx));
	munit_assert(skl_check_type(ctx, -1, SKL_VAL_NUMBER));

	skl_resize_stack(ctx, 3);
	munit_assert_int(3, ==, skl_stack_len(ctx));

	skl_resize_stack(ctx, 5);
	munit_assert_int(5, ==, skl_stack_len(ctx));
	munit_assert(skl_check_type(ctx, -1, SKL_VAL_NULL));
	munit_assert(skl_check_type(ctx, -2, SKL_VAL_NULL));

	skl_string_ref_t ref = skl_as_string(ctx, 1);
	skl_assert_string_ref_equal(SKL_STRING_REF("wat"), ref);

	double number = skl_as_number(ctx, 2);
	munit_assert_double(4.9, ==, number);

	return MUNIT_OK;
}

static MunitResult
trap(const MunitParameter params[], void* fixture)
{
	(void)params;
	skl_ctx_t* ctx = fixture;

	int caught = 0;

	skl_trap_t trap;
	SKL_BEGIN_TRY(ctx, trap)
	{
		skl_throw(ctx, "wat");
	}
	SKL_CATCH(ctx, trap)
	{
		caught = 1;
	}
	SKL_END_TRY(ctx, trap)

	munit_assert_int(1, ==, caught);

	caught = 0;
	SKL_BEGIN_TRY(ctx, trap)
	{
		// No throw
	}
	SKL_CATCH(ctx, trap)
	{
		caught = 1;
	}
	SKL_END_TRY(ctx, trap)

	munit_assert_int(0, ==, caught);

	// nested
	caught = 0;
	SKL_BEGIN_TRY(ctx, trap)
	{
		skl_trap_t trap;
		SKL_BEGIN_TRY(ctx, trap)
		{
			skl_throw(ctx, "once");
		}
		SKL_CATCH(ctx, trap)
		{
			caught = 1;
		}
		SKL_END_TRY(ctx, trap)
		if(caught == 1)
		{
			skl_throw(ctx, "twice");
		}
	}
	SKL_CATCH(ctx, trap)
	{
		caught = 2;
	}
	SKL_END_TRY(ctx, trap)
	munit_assert_int(2, ==, caught);

	// Throw in handler
	caught = 0;
	SKL_BEGIN_TRY(ctx, trap)
	{
		skl_trap_t trap;
		SKL_BEGIN_TRY(ctx, trap)
		{
			skl_throw(ctx, "once");
		}
		SKL_CATCH(ctx, trap)
		{
			skl_throw(ctx, "nested");
		}
		SKL_END_TRY(ctx, trap)
	}
	SKL_CATCH(ctx, trap)
	{
		caught = 2;
	}
	SKL_END_TRY(ctx, trap)
	munit_assert_int(2, ==, caught);

	return MUNIT_OK;
}

static MunitResult
type_error(const MunitParameter params[], void* fixture)
{
	(void)params;
	skl_ctx_t* ctx = fixture;
	bool caught;
	skl_trap_t trap;

	SKL_BEGIN_TRY(ctx, trap)
	{
		skl_push_null(ctx);
		skl_as_number(ctx, 0);
	}
	SKL_CATCH(ctx, trap)
	{
		caught = true;
	}
	SKL_END_TRY(ctx, trap)

	munit_assert(caught);
	munit_assert_int(2, ==, skl_stack_len(ctx));
	munit_assert(skl_check_type(ctx, -1, SKL_VAL_STRING));
	skl_assert_string_ref_equal(
		SKL_STRING_REF("Type error: Value at #0 is not SKL_VAL_NUMBER"),
		skl_as_string(ctx, -1)
	);

	return MUNIT_OK;
}


static MunitTest tests[] = {
	{
		.name = "/stack",
		.test = stack,
		.setup = setup_ctx,
		.tear_down = teardown_ctx
	},
	{
		.name = "/trap",
		.test = trap,
		.setup = setup_ctx,
		.tear_down = teardown_ctx
	},
	{
		.name = "/type_error",
		.test = type_error,
		.setup = setup_ctx,
		.tear_down = teardown_ctx
	},
	{ .test = NULL }
};

MunitSuite vm = {
	.prefix = "/vm",
	.tests = tests
};
