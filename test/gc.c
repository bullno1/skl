#include "../src/context.h"
#include "test_helper.h"


static MunitResult
init(const MunitParameter params[], void* fixture)
{
	(void)params;

	// If root marking is done properly, the ref list should not get collected
	skl_ctx_t* ctx = fixture;

	skl_gc(ctx, SKL_GC_COLLECT);

	return MUNIT_OK;
}


static MunitTest tests[] = {
	{
		.name = "/init",
		.test = init,
		.setup = setup_ctx,
		.tear_down = teardown_ctx
	},
	{ .test = NULL }
};

MunitSuite gc = {
	.prefix = "/gc",
	.tests = tests
};
