#ifndef LEXER_H
#define LEXER_H

#include "token.h"

typedef struct {
    const char *text;
    int current_pos;
    Token  current_token;
} Lexer;

Lexer* init_lexer(const char  *text);
void free_lexer(Lexer *lexer);
char peek(Lexer *lexer, int offset);
void advance(Lexer *lexer, int steps);
void reverse(Lexer *lexer, int steps);

void skip_whitespace(Lexer *lexer);
void skip_comment(Lexer *lexer);

Token get_number(Lexer *lexer);
Token get_string(Lexer *lexer);
Token get_identifer(Lexer *lexer);
Token get_next_token(Lexer * lexer);

#endif /* LEXER_H */