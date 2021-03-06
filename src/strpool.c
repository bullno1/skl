#include "strpool.h"
#include <bk/allocator.h>
#include <bk/array.h>
#include <bk/printf.h>
#include <bk/fs/mem.h>
#include "context.h"
#include "string.h"
#include "vm.h"
#define XXH_PRIVATE_API
#define XXH_NAMESPACE skl_
#include <xxHash/xxhash.h>


static void
skl_string_release(skl_ctx_t* ctx, skl_gc_header_t* header);


const skl_gc_info_t skl_string_gc_info = {
	.free_fn = skl_string_release,
};


void
skl_strpool_init(skl_ctx_t* ctx)
{
	kh_init(skl_strpool, ctx->cfg.allocator, &ctx->strpool);
}

void
skl_strpool_cleanup(skl_ctx_t* ctx)
{
	kh_cleanup(skl_strpool, &ctx->strpool);
}

skl_string_t*
skl_strpool_alloc(skl_ctx_t* ctx, skl_string_ref_t string)
{
	skl_strpool_key_t key = {
		.hash = XXH32(string.ptr, string.length, (uintptr_t)ctx),
		.ref = string
	};

	khiter_t itr = kh_get(skl_strpool, &ctx->strpool, key);
	if(itr != kh_end(&ctx->strpool))
	{
		return kh_val(&ctx->strpool, itr);
	}
	else
	{
		skl_string_t* pooled_str = skl_gc_alloc(
			ctx, sizeof(skl_string_t) + string.length + 1, &skl_string_gc_info
		);
		if(pooled_str == NULL) { return pooled_str; }

		pooled_str->hash = key.hash;
		pooled_str->length = string.length;
		memcpy(pooled_str->content, string.ptr, string.length);
		pooled_str->content[string.length] = 0;
		key.ref.ptr = pooled_str->content;

		int r;
		itr = kh_put(skl_strpool, &ctx->strpool, key, &r);
		if(r < 0) { return NULL; }

		kh_val(&ctx->strpool, itr) = pooled_str;

		return pooled_str;
	}
}

void
skl_push_string(skl_ctx_t* ctx, skl_string_ref_t str)
{
	skl_vm_push_ref(ctx, SKL_VAL_STRING, skl_strpool_alloc(ctx, str));
}

skl_string_ref_t
skl_as_string(skl_ctx_t* ctx, skl_index_t index)
{
	const skl_string_t* string;
	SKL_GET_OBJ(string, ctx, index, SKL_VAL_STRING);

	return (skl_string_ref_t) {
		.ptr = string->content,
		.length = string->length,
	};
}

void
skl_push_string_fmtv(skl_ctx_t* ctx, const char* fmt, va_list args)
{
	bk_mem_file_t mem_file;
	bk_file_t* fmt_file = bk_mem_fs_wrap_flexible(&mem_file, &ctx->fmt_buf);
	SKL_ASSERT(ctx, bk_vprintf(fmt_file, fmt, args) == 0, "Invalid format string");
	bk_array_push(ctx->fmt_buf, 0);

	skl_string_t* string = skl_strpool_alloc(ctx, skl_string_ref(ctx->fmt_buf));
	skl_vm_push_ref(ctx, SKL_VAL_STRING, string);
}


void
skl_string_release(skl_ctx_t* ctx, skl_gc_header_t* header)
{
	skl_string_t* string = BK_CONTAINER_OF(header, skl_string_t, gc_header);
	skl_strpool_key_t key = {
		.hash = string->hash,
		.ref = {
			.length = string->length,
			.ptr = string->content
		}
	};

	khiter_t itr = kh_get(skl_strpool, &ctx->strpool, key);
	// if OOM happened in skl_strpool_alloc, it is possible for string to be
	// untracked
	if(itr != kh_end(&ctx->strpool))
	{
		kh_del(skl_strpool, &ctx->strpool, itr);
	}
}

BK_INLINE khint_t
skl_strpool_key_hash(skl_strpool_key_t key)
{
	return key.hash;
}

BK_INLINE khint_t
skl_strpool_key_equal(skl_strpool_key_t lhs, skl_strpool_key_t rhs)
{
	return lhs.ref.length == rhs.ref.length
		&& memcmp(lhs.ref.ptr, rhs.ref.ptr, lhs.ref.length) == 0;
}

__KHASH_IMPL(
	skl_strpool,
	,
	skl_strpool_key_t,
	skl_string_t*,
	1,
	skl_strpool_key_hash,
	skl_strpool_key_equal
)
