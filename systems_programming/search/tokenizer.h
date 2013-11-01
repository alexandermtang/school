#ifndef TOKENIZER_H
#define TOKENIZER_H
/*
 * tokenizer.h
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */

struct TokenizerT_ {
	char* copied_string;
	char* delimiters;
	char* current_position;
};

typedef struct TokenizerT_ TokenizerT;

TokenizerT *TKCreate(char *separators, char *ts);
void TKDestroy(TokenizerT *tk);
char *TKGetNextToken(TokenizerT *tk);

#endif
