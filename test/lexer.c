#include "../src/lexer.h"
#include "../src/context.h"
#include "test_helper.h"
#include <bk/fs/mem.h>


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

	skl_ctx_t* ctx = fixture;

	char* text =
		"(\n"
		"  ) \n"
		/*"  [ ] { }\n"*/
		/*"50.6 \"hi hi \" \r\n"*/
		/*"# comment 12 \"hi\\"" \r"*/
		/*"; 12 \"hi\\"" \r"*/
		/*"\n"*/
		/*"test-23 -3 -ve -\r\n"*/
		/*"\r"*/
		/*" \"f \\\"\"\n"*/
		/*"  \" \\\\\""*/
	;

	 skl_token_t expected_tokens[] = {
		{
			.type = SKL_TOKEN_LIST_BEGIN,
			.lexeme = string_ref("("),
			.location = {
				.start = {.line = 1, .column = 1},
				.end = {.line = 1, .column = 1}
			}
		},
		{
			.type = SKL_TOKEN_TERM,
			.lexeme = string_ref("\n"),
			.location = {
				.start = {.line = 1, .column = 1},
				.end = {.line = 1, .column = 1}
			}
		},
		{
			.type = SKL_TOKEN_LIST_END,
			.lexeme = string_ref(")"),
			.location = {
				.start = {.line = 2, .column = 3},
				.end = {.line = 2, .column = 3}
			}
		},
		{
			.type = SKL_TOKEN_TERM,
			.lexeme = string_ref("\n"),
			.location = {
				.start = {.line = 2, .column = 5},
				.end = {.line = 2, .column = 5}
			}
		},
	 };

	bk_mem_file_t mem_file;
	bk_file_t* file = bk_mem_fs_wrap_fixed(&mem_file, text, strlen(text));

	skl_lexer_reset(&ctx->lexer, file);
	skl_lexer_error_t err;
	skl_token_t token;

	uint32_t size = BK_STATIC_ARRAY_LEN(expected_tokens);

	for(uint32_t i = 0; i < size; ++i)
	{
		err = skl_lexer_next_token(&ctx->lexer, &token);

		munit_logf(
			MUNIT_LOG_INFO, "%s %s '%.*s' (%u:%u - %u:%u)",
			skl_lexer_error_t_to_str(err) + sizeof("SKL"),
			skl_token_type_t_to_str(token.type) + sizeof("SKL"),
			(int)token.lexeme.length,
			token.lexeme.ptr,
			token.location.start.line,
			token.location.start.column,
			token.location.end.line,
			token.location.end.column
		);

		skl_assert_enum(skl_lexer_error_t, SKL_LEX_OK, ==, err);

		skl_token_t expected_token = expected_tokens[i];
		skl_assert_string_ref_equal(expected_token.lexeme, token.lexeme);

		skl_assert_enum(skl_token_type_t, expected_token.type, ==, token.type);
	}

	err = skl_lexer_next_token(&ctx->lexer, &token);
	skl_assert_enum(skl_lexer_error_t, SKL_LEX_EOF, ==, err);

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
