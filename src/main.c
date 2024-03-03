#include <stdio.h>
#include <stdlib.h>
#include "include/lexer.h"

int main(void)
{
    const char *input_string = "1.";
    Lexer *lexer = init_lexer(input_string);

    Token token;
    do 
    {
        token = get_next_token(lexer);
        if (token.type != NONE)
        {
            printf("Token(type=%d (%s), value='%s')\n", token.type, tokenNames[token.type] ,token.value ? token.value : "NULL");
            free(token.value);  // Move this line here
        }
    }
    while (token.type != ENDMARKER);

    free_lexer(lexer);
    return 0;
}
