#include "lexer.h"
#include "context.h"
#include <ctype.h>
#include <bk/fs.h>


static skl_lexer_error_t
skl_lexer_peek_char(skl_lexer_t* lexer, char* ch);

static void
skl_lexer_consume_char(skl_lexer_t* lexer);

static void
skl_lexer_begin_capture(skl_lexer_t* lexer, skl_token_t* token);

static skl_lexer_error_t
skl_lexer_end_capture(
	skl_lexer_t* lexer, skl_token_t* token, skl_token_type_t token_type
);

static skl_lexer_error_t
skl_lexer_skip_to_eol(skl_lexer_t* lexer);

static skl_lexer_error_t
skl_lexer_scan_number(skl_lexer_t* lexer, skl_token_t* token);

static skl_lexer_error_t
skl_lexer_scan_symbol(skl_lexer_t* lexer, skl_token_t* token);

static skl_lexer_error_t
skl_lexer_scan_string(skl_lexer_t* lexer, skl_token_t* token, char quote);

static skl_lexer_error_t
skl_lexer_error(skl_lexer_t* lexer, skl_token_t* token, skl_lexer_error_t error);

static bool
skl_lexer_is_separator(char ch);


void
skl_lexer_init(skl_lexer_t* lexer, skl_ctx_t* ctx)
{
	*lexer = (skl_lexer_t) {
		.capture_buf = bk_array_create(ctx->cfg.allocator, char, 32)
	};
}

void
skl_lexer_cleanup(skl_lexer_t* lexer)
{
	bk_array_destroy(lexer->capture_buf);
}

void
skl_lexer_reset(skl_lexer_t* lexer, struct bk_file_s* file)
{
	lexer->file = file;
	lexer->location.column = 1;
	lexer->location.line = 1;
	lexer->previous_location = lexer->location;
	bk_array_clear(lexer->capture_buf);
	lexer->capturing = false;
	lexer->buffered = false;
	lexer->last_char = 0;
	lexer->eof = false;
}

skl_lexer_error_t
skl_lexer_next_token(skl_lexer_t* lexer, skl_token_t* token)
{
	char ch;
	skl_lexer_error_t err;

	for(;;)
	{
		err = skl_lexer_peek_char(lexer, &ch);
		if(err != SKL_LEX_OK) { return err; }

		skl_lexer_begin_capture(lexer, token);
		skl_lexer_consume_char(lexer);

		switch(ch)
		{
			case ' ':
			case '\t':
				break;
			case '#':
				err = skl_lexer_skip_to_eol(lexer);
				if(err != SKL_LEX_OK) { return err; }
				break;
			case ';':
			case '\n':
				return skl_lexer_end_capture(lexer, token, SKL_TOKEN_TERM);
			case '\r':
				err = skl_lexer_peek_char(lexer, &ch);
				if(err == SKL_LEX_OK && ch != '\n')
				{
					return skl_lexer_end_capture(lexer, token, SKL_TOKEN_TERM);
				}
				break;
			case '"':
			case '\'':
				return skl_lexer_scan_string(lexer, token, ch);
			case '(':
			case '[':
			case '{':
				return skl_lexer_end_capture(lexer, token, SKL_TOKEN_LIST_BEGIN);
			case ')':
			case ']':
			case '}':
				return skl_lexer_end_capture(lexer, token, SKL_TOKEN_LIST_END);
			case '-':
				err = skl_lexer_peek_char(lexer, &ch);
				if(err == SKL_LEX_IO_ERROR) { return err; }
				if(err == SKL_LEX_EOF)
				{
					return skl_lexer_end_capture(lexer, token, SKL_TOKEN_SYMBOL);
				}

				if(isdigit(ch))
				{
					return skl_lexer_scan_number(lexer, token);
				}
				else if(skl_lexer_is_separator(ch))
				{
					return skl_lexer_end_capture(lexer, token, SKL_TOKEN_SYMBOL);
				}
				else
				{
					return skl_lexer_scan_symbol(lexer, token);
				}
				break;
			default:
				if(isdigit(ch))
				{
					return skl_lexer_scan_number(lexer, token);
				}
				else
				{
					return skl_lexer_scan_symbol(lexer, token);
				}
				break;
		}
	}
}


skl_lexer_error_t
skl_lexer_scan_number(skl_lexer_t* lexer, skl_token_t* token)
{
	char ch;
	skl_lexer_error_t err;
	bool found_point = false;

	for(;;)
	{
		err = skl_lexer_peek_char(lexer, &ch);
		if(err == SKL_LEX_IO_ERROR) { return err; }
		if(err == SKL_LEX_EOF)
		{
			return skl_lexer_end_capture(lexer, token, SKL_TOKEN_NUMBER);
		}

		if(ch == '.')
		{
			skl_lexer_consume_char(lexer);

			if(found_point)
			{
				return skl_lexer_error(lexer, token, SKL_LEX_BAD_NUMBER);
			}
			else
			{
				found_point = true;
			}
		}
		else if(isdigit(ch))
		{
			skl_lexer_consume_char(lexer);
		}
		else if(!skl_lexer_is_separator(ch))
		{
			skl_lexer_consume_char(lexer);
			return skl_lexer_error(lexer, token, SKL_LEX_BAD_NUMBER);
		}
		else
		{
			return skl_lexer_end_capture(lexer, token, SKL_TOKEN_NUMBER);
		}
	}
}

skl_lexer_error_t
skl_lexer_scan_symbol(skl_lexer_t* lexer, skl_token_t* token)
{
	char ch;
	skl_lexer_error_t err;

	for(;;)
	{
		err = skl_lexer_peek_char(lexer, &ch);
		if(err == SKL_LEX_IO_ERROR) { return err; }
		if(err == SKL_LEX_EOF)
		{
			return skl_lexer_end_capture(lexer, token, SKL_TOKEN_SYMBOL);
		}

		if(skl_lexer_is_separator(ch))
		{
			return skl_lexer_end_capture(lexer, token, SKL_TOKEN_SYMBOL);
		}
		else
		{
			skl_lexer_consume_char(lexer);
		}
	}
}

skl_lexer_error_t
skl_lexer_scan_string(skl_lexer_t* lexer, skl_token_t* token, char quote)
{
	skl_lexer_begin_capture(lexer, token);
	// include quote
	token->location.start = lexer->previous_location;

	char previous_char = quote;

	char ch;
	skl_lexer_error_t err;
	for(;;)
	{
		err = skl_lexer_peek_char(lexer, &ch);
		if(err == SKL_LEX_IO_ERROR) { return err; }
		if(err == SKL_LEX_EOF)
		{
			return skl_lexer_error(lexer, token, SKL_LEX_BAD_STRING);
		}

		if(ch == quote && previous_char != '\\')
		{
			skl_lexer_end_capture(
				lexer, token, SKL_TOKEN_STRING
			);
			++token->location.end.column; // include quote
			skl_lexer_consume_char(lexer);
			return SKL_LEX_OK;
		}
		else
		{
			skl_lexer_consume_char(lexer);
		}

		if(ch == '\\' && previous_char == '\\')
		{
			previous_char = 0;
		}
		else
		{
			previous_char = ch;
		}
	}
}

skl_lexer_error_t
skl_lexer_skip_to_eol(skl_lexer_t* lexer)
{
	char ch;
	skl_lexer_error_t err;

	for(;;)
	{
		err = skl_lexer_peek_char(lexer, &ch);
		if(err != SKL_LEX_OK) { return err; }

		if(ch == '\r' || ch == '\n')
		{
			return SKL_LEX_OK;
		}
		else
		{
			skl_lexer_consume_char(lexer);
		}
	}
}

skl_lexer_error_t
skl_lexer_peek_char(skl_lexer_t* lexer, char* ch)
{
	if(lexer->eof) { return SKL_LEX_EOF; }

	if(lexer->buffered)
	{
		*ch = lexer->read_buf;
		return SKL_LEX_OK;
	}

	size_t num_bytes = sizeof(*ch);
	if(bk_fread(lexer->file, &lexer->read_buf, &num_bytes) != 0)
	{
		return SKL_LEX_IO_ERROR;
	}

	if(num_bytes == 0)
	{
		lexer->eof = true;
		return SKL_LEX_EOF;
	}
	else
	{
		*ch = lexer->read_buf;
		lexer->buffered = true;
		return SKL_LEX_OK;
	}
}

void
skl_lexer_consume_char(skl_lexer_t* lexer)
{
	if(!lexer->buffered) { return; }

	char last_char = lexer->last_char;
	char current_char = lexer->read_buf;
	lexer->previous_location = lexer->location;

	if(lexer->capturing)
	{
		bk_array_push(lexer->capture_buf, current_char);
	}

	if(false
		|| current_char == '\r'
		|| (current_char == '\n' && last_char != '\r')
	)
	{
		lexer->location.column = 1;
		lexer->location.line++;
	}
	else
	{
		lexer->location.column++;
	}

	lexer->buffered = false;
	lexer->last_char = current_char;
}

void
skl_lexer_begin_capture(skl_lexer_t* lexer, skl_token_t* token)
{
	token->location.start = lexer->location;
	lexer->capturing = true;
	bk_array_clear(lexer->capture_buf);
}

skl_lexer_error_t
skl_lexer_end_capture(
	skl_lexer_t* lexer, skl_token_t* token, skl_token_type_t token_type
)
{
	lexer->capturing = false;
	token->location.end = lexer->previous_location;
	token->type = token_type;

	bk_array_push(lexer->capture_buf, 0);
	token->lexeme = (skl_string_ref_t) {
		.ptr = lexer->capture_buf,
		.length = bk_array_len(lexer->capture_buf) - 1,
	};

	return SKL_LEX_OK;
}

bool
skl_lexer_is_separator(char ch)
{
	return false
		|| ch == ' ' || ch == '\t'
		|| ch == '\r' || ch == '\n' || ch == ';'
		|| ch == '[' || ch == '(' || ch == '{'
		|| ch == ']' || ch == ')' || ch == '}'
		|| ch == '"' || ch == '\''
		|| ch == '#'
		;
}

skl_lexer_error_t
skl_lexer_error(skl_lexer_t* lexer, skl_token_t* token, skl_lexer_error_t error)
{
	skl_lexer_end_capture(lexer, token, SKL_TOKEN_SYMBOL);
	return error;
}
