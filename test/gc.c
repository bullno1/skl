#include "../src/context.h"
#include "../src/list.h"
#include "test_helper.h"


static MunitResult
init(const MunitParameter params[], void* fixture)
{
	(void)params;

	skl_ctx_t* ctx = fixture;

	skl_gc(ctx, SKL_GC_COLLECT);
	skl_gc(ctx, SKL_GC_COLLECT);
	// If root marking is done properly, the ref list should not get collected
	// and this will not crash
	bk_array_len(ctx->gc.refs->elements);

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
