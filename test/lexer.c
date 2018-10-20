#include "../src/lexer.h"
#include "test_helper.h"


struct fixture_s
{
	skl_ctx_t* ctx;
	skl_lexer_t* lexer;
};


static void*
setup(const MunitParameter params[], void* data)
{
	(void)params;
	(void)data;

	return create_ctx();
}

static void
teardown(void* data)
{
	skl_destroy_ctx(data);
}


static MunitResult
normal(const MunitParameter params[], void* fixture)
{
	(void)params;
	(void)fixture;

	return MUNIT_OK;
}


static MunitTest tests[] = {
	{
		.name = "/normal",
		.test = normal,
		.setup = setup,
		.tear_down = teardown
	},
	{ .test = NULL }
};

MunitSuite lexer = {
	.prefix = "/lexer",
	.tests = tests
};
