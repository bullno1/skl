#include "../src/lexer.h"
#include "../src/context.h"
#include "test_helper.h"
#include <bk/fs/mem.h>


static MunitResult
normal(const MunitParameter params[], void* fixture)
{
	(void)params;

	skl_ctx_t* ctx = fixture;

	char* text =
		"(\n"
		"  ) \n"
		"  [ ] { }\n"
		"50.6 \"hi hi \" \r\n"
		"# comment \r"
		" 12; \"hi\\\"\" \r"
		"test-23 -3 -ve -\r\n"
		" \"f \\\"\"\n"
		"  ' \\\\'"
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
			.type = SKL_TOKEN_TERMINATE,
			.lexeme = string_ref("\n"),
			.location = {
				.start = {.line = 1, .column = 2},
				.end = {.line = 1, .column = 2}
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
			.type = SKL_TOKEN_TERMINATE,
			.lexeme = string_ref("\n"),
			.location = {
				.start = {.line = 2, .column = 5},
				.end = {.line = 2, .column = 5}
			}
		},
		{
			.type = SKL_TOKEN_LIST_BEGIN,
			.lexeme = string_ref("["),
			.location = {
				.start = {.line = 3, .column = 3},
				.end = {.line = 3, .column = 3}
			}
		},
		{
			.type = SKL_TOKEN_LIST_END,
			.lexeme = string_ref("]"),
			.location = {
				.start = {.line = 3, .column = 5},
				.end = {.line = 3, .column = 5}
			}
		},
		{
			.type = SKL_TOKEN_LIST_BEGIN,
			.lexeme = string_ref("{"),
			.location = {
				.start = {.line = 3, .column = 7},
				.end = {.line = 3, .column = 7}
			}
		},
		{
			.type = SKL_TOKEN_LIST_END,
			.lexeme = string_ref("}"),
			.location = {
				.start = {.line = 3, .column = 9},
				.end = {.line = 3, .column = 9}
			}
		},
		{
			.type = SKL_TOKEN_TERMINATE,
			.lexeme = string_ref("\n"),
			.location = {
				.start = {.line = 3, .column = 10},
				.end = {.line = 3, .column = 10}
			}
		},
		{
			.type = SKL_TOKEN_NUMBER,
			.lexeme = string_ref("50.6"),
			.location = {
				.start = {.line = 4, .column = 1},
				.end = {.line = 4, .column = 4}
			}
		},
		{
			.type = SKL_TOKEN_STRING,
			.lexeme = string_ref("hi hi "),
			.location = {
				.start = {.line = 4, .column = 6},
				.end = {.line = 4, .column = 13}
			}
		},
		{
			.type = SKL_TOKEN_TERMINATE,
			.lexeme = string_ref("\n"),
			.location = {
				.start = {.line = 4, .column = 16},
				.end = {.line = 4, .column = 16}
			}
		},
		{
			.type = SKL_TOKEN_TERMINATE,
			.lexeme = string_ref("\r"),
			.location = {
				.start = {.line = 5, .column = 11},
				.end = {.line = 5, .column = 11}
			}
		},
		{
			.type = SKL_TOKEN_NUMBER,
			.lexeme = string_ref("12"),
			.location = {
				.start = {.line = 6, .column = 2},
				.end = {.line = 6, .column = 3}
			}
		},
		{
			.type = SKL_TOKEN_TERMINATE,
			.lexeme = string_ref(";"),
			.location = {
				.start = {.line = 6, .column = 4},
				.end = {.line = 6, .column = 4}
			}
		},
		{
			.type = SKL_TOKEN_STRING,
			.lexeme = string_ref("hi\\\""),
			.location = {
				.start = {.line = 6, .column = 6},
				.end = {.line = 6, .column = 11}
			}
		},
		{
			.type = SKL_TOKEN_TERMINATE,
			.lexeme = string_ref("\r"),
			.location = {
				.start = {.line = 6, .column = 13},
				.end = {.line = 6, .column = 13}
			}
		},
		{
			.type = SKL_TOKEN_SYMBOL,
			.lexeme = string_ref("test-23"),
			.location = {
				.start = {.line = 7, .column = 1},
				.end = {.line = 7, .column = 7}
			}
		},
		{
			.type = SKL_TOKEN_NUMBER,
			.lexeme = string_ref("-3"),
			.location = {
				.start = {.line = 7, .column = 9},
				.end = {.line = 7, .column = 10}
			}
		},
		{
			.type = SKL_TOKEN_SYMBOL,
			.lexeme = string_ref("-ve"),
			.location = {
				.start = {.line = 7, .column = 12},
				.end = {.line = 7, .column = 14}
			}
		},
		{
			.type = SKL_TOKEN_SYMBOL,
			.lexeme = string_ref("-"),
			.location = {
				.start = {.line = 7, .column = 16},
				.end = {.line = 7, .column = 16}
			}
		},
		{
			.type = SKL_TOKEN_TERMINATE,
			.lexeme = string_ref("\n"),
			.location = {
				.start = {.line = 7, .column = 18},
				.end = {.line = 7, .column = 18}
			}
		},
		{
			.type = SKL_TOKEN_STRING,
			.lexeme = string_ref("f \\\""),
			.location = {
				.start = {.line = 8, .column = 2},
				.end = {.line = 8, .column = 7}
			}
		},
		{
			.type = SKL_TOKEN_TERMINATE,
			.lexeme = string_ref("\n"),
			.location = {
				.start = {.line = 8, .column = 8},
				.end = {.line = 8, .column = 8}
			}
		},
		{
			.type = SKL_TOKEN_STRING,
			.lexeme = string_ref(" \\\\"),
			.location = {
				.start = {.line = 9, .column = 3},
				.end = {.line = 9, .column = 7}
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
		skl_assert_loc_range_equal(expected_token.location, token.location);
	}

	err = skl_lexer_next_token(&ctx->lexer, &token);
	skl_assert_enum(skl_lexer_error_t, SKL_LEX_EOF, ==, err);

	return MUNIT_OK;
}


static MunitTest tests[] = {
	{
		.name = "/normal",
		.test = normal,
		.setup = setup_ctx,
		.tear_down = teardown_ctx
	},
	{ .test = NULL }
};

MunitSuite lexer = {
	.prefix = "/lexer",
	.tests = tests
};
