#include <stdio.h>
#include <stdlib.h>
#include "include/lexer.h"

int main()
{
    // do not enter something like 1. it will end up loop of '' tokens type 2 I will fix that for now don't enter things like that
    const char *input_string = "1231231233.1232133  .2 0.3";
    Lexer *lexer = init_lexer(input_string);

    Token token;
    do 
    {
        token = get_next_token(lexer);
        if (token.type != NONE)
        {
            printf("Token(type=%d, value='%s')\n", token.type, token.value ? token.value : "NULL");
            free(token.value);
        }
    }
    while (token.type != ENDMARKER);
    free_lexer(lexer);
    return 0;
}
