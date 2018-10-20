#ifndef SKL_TEST_HELPER_H
#define SKL_TEST_HELPER_H

#include <bk/allocator.h>
#include <bk/default_allocator.h>
#include <skl.h>
#include <munit/munit.h>


static inline skl_ctx_t*
create_ctx()
{
	skl_config_t cfg = {
		.allocator = bk_default_allocator
	};
	return skl_create_ctx(&cfg);
}


#endif
