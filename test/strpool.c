#include "../src/context.h"
#include "../src/strpool.h"
#include "../src/string.h"
#include "test_helper.h"


static MunitResult
alloc(const MunitParameter params[], void* fixture)
{
	(void)params;

	skl_ctx_t* ctx = fixture;

	skl_string_ref_t foo = SKL_STRING_REF("foo");
	skl_string_ref_t bar = SKL_STRING_REF("bar");

	skl_string_t* str1 = skl_strpool_alloc(ctx, foo);
	skl_assert_string_ref_equal(foo, skl_string_ref(str1->content));

	skl_string_t* str2 = skl_strpool_alloc(ctx, foo);
	munit_assert_ptr_equal(str1, str2);

	skl_string_t* str3 = skl_strpool_alloc(ctx, bar);
	munit_assert_ptr_not_equal(str3, str1);

	skl_assert_string_ref_equal(foo, skl_string_ref(str1->content));
	skl_assert_string_ref_equal(bar, skl_string_ref(str3->content));

	return MUNIT_OK;
}

static MunitResult
fmt(const MunitParameter params[], void* fixture)
{
	(void)params;

	skl_ctx_t* ctx = fixture;

	skl_push_string_fmt(ctx, "Foo %s %d", "bar", 42);
	khint_t size = kh_size(&ctx->strpool);
	skl_push_string(ctx, SKL_STRING_REF("Foo bar 42"));
	khint_t size2 = kh_size(&ctx->strpool);
	munit_assert_int(size, ==, size2);

	return MUNIT_OK;
}


static MunitTest tests[] = {
	{
		.name = "/alloc",
		.test = alloc,
		.setup = setup_ctx,
		.tear_down = teardown_ctx
	},
	{
		.name = "/fmt",
		.test = fmt,
		.setup = setup_ctx,
		.tear_down = teardown_ctx
	},
	{ .test = NULL }
};

MunitSuite strpool = {
	.prefix = "/strpool",
	.tests = tests
};
