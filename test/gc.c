#include "../src/context.h"
#include "../src/list.h"
#include <bk/array.h>
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


static MunitResult
ref(const MunitParameter params[], void* fixture)
{
	(void)params;
	skl_ctx_t* ctx = fixture;

	skl_push_string(ctx, SKL_STRING_REF("wat"));
	skl_index_t ref = skl_gc_ref(ctx, -1);
	skl_resize_stack(ctx, 0);

	skl_gc(ctx, SKL_GC_COLLECT);
	skl_gc(ctx, SKL_GC_COLLECT);

	munit_assert_size(1, ==, kh_size(&ctx->strpool));
	skl_gc_deref(ctx, ref);
	skl_resize_stack(ctx, 1);
	skl_string_ref_t string_ref = skl_as_string(ctx, -1);
	skl_assert_string_ref_equal(string_ref, SKL_STRING_REF("wat"));

	skl_resize_stack(ctx, 0);
	skl_gc_unref(ctx, ref);
	skl_gc(ctx, SKL_GC_COLLECT);
	skl_gc(ctx, SKL_GC_COLLECT);
	munit_assert_size(0, ==, kh_size(&ctx->strpool));

	skl_push_string(ctx, SKL_STRING_REF("watwhy?"));
	skl_index_t ref2 = skl_gc_ref(ctx, -1);

	munit_assert_int(ref, ==, ref2);

	return MUNIT_OK;
}


static MunitTest tests[] = {
	{
		.name = "/init",
		.test = init,
		.setup = setup_ctx,
		.tear_down = teardown_ctx
	},
	{
		.name = "/ref",
		.test = ref,
		.setup = setup_ctx,
		.tear_down = teardown_ctx
	},
	{ .test = NULL }
};

MunitSuite gc = {
	.prefix = "/gc",
	.tests = tests
};
