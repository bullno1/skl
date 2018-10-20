#ifndef SICKLE_LEXER_H
#define SICKLE_LEXER_H

#include "internal.h"


struct bk_file_s;

typedef struct skl_token_s skl_token_t;
typedef struct skl_lexer_s skl_lexer_t;


#define SKL_TOKEN(X) \
	X(SKL_TOKEN_LIST_BEGIN) \
	X(SKL_TOKEN_LIST_END) \
	X(SKL_TOKEN_TERMINATE) \
	X(SKL_TOKEN_STRING) \
	X(SKL_TOKEN_SYMBOL) \
	X(SKL_TOKEN_NUMBER) \

BK_ENUM(skl_token_type_t, SKL_TOKEN)

#define SKL_LEXER_ERROR(X) \
	X(SKL_LEX_OK) \
	X(SKL_LEX_IO_ERROR) \
	X(SKL_LEX_EOF) \
	X(SKL_LEX_BAD_STRING) \
	X(SKL_LEX_BAD_NUMBER) \

BK_ENUM(skl_lexer_error_t, SKL_LEXER_ERROR)


struct skl_token_s
{
	skl_token_type_t type;
	skl_string_ref_t lexeme;
	skl_loc_range_t location;
};

struct skl_lexer_s
{
	struct bk_file_s* file;
	skl_location_t location;
	skl_location_t previous_location;

	BK_ARRAY(char) capture_buf;
	char read_buf;
	char last_char;
	bool capturing;
	bool buffered;
	bool eof;
};


void
skl_lexer_init(skl_lexer_t* lexer, skl_ctx_t* ctx);

void
skl_lexer_cleanup(skl_lexer_t* lexer);

void
skl_lexer_reset(skl_lexer_t* lexer, struct bk_file_s* file);

skl_lexer_error_t
skl_lexer_next_token(skl_lexer_t* lexer, skl_token_t* token);

#endif
