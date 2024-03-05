#include <stdio.h>
#include <stdlib.h>
#include "include/lexer.h"

int main(void)
{
    const char *input_string = "f(x) := 2x+3\nx = 4";
    Lexer *lexer = init_lexer(input_string);
    
    Token token;

    do 
    {
        token = get_next_token(lexer);
        if (token.type != NONE)
        {
            printf("Token(type=%d (%s), value='%s', lineno=%d, line='%s', start=(%d, %d) end=(%d, %d) )\n", 
            token.type, 
            tokenNames[token.type] ,
            token.value ? token.value : "NULL",
            token.lineno,
            token.line ? token.line : "NULL",
            token.start_pos.lineno,
            token.start_pos.position,
            token.end_pos.lineno,
            token.end_pos.position
            );
            free(token.value);
        }
    }
    while (token.type != ENDMARKER);
    
    free_lexer(lexer);
    return 0;
}
