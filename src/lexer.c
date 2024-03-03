#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "include/lexer.h"

// this should be match to the defines in include/token.h
char *tokenNames[] = {
    "NONE",
    "NAME",
    "NUMBER",
    "STRING",
    "ID",
    "PLUS",
    "PLUSEQUAL",
    "MINUS",
    "MINUSEQUAL",
    "STAR",
    "STAREQUAL",
    "GREATER",
    "LESS",
    "CARET",
    "VBAR",
    "PERCENT",
    "FSLASH",
    "FSLASHEQUAL",
    "BSLASH",
    "LPAREN",
    "RPAREN",
    "LBRACKET",
    "RBRACKET",
    "LBRACE",
    "RBRACE",
    "EQUAL",
    "EQUALEQUAL",
    "COMMA",
    "COLON",
    "COLONEQUAL",
    "DOT",
    "SEMI",
    "UNDERSCORE",
    "HASH",
    "EXCLAMATION",
    "AMPERSAND",
    "ENDMARKER",
    "ERRORTOKEN"
};

Lexer* init_lexer(const char  *text)
{
    Lexer *lexer = malloc(sizeof(Lexer));
    lexer->text = text;
    lexer->current_pos = 0;
    lexer->current_token.type = NONE;
    lexer->current_token.value = NULL;
    return lexer;
}

void free_lexer(Lexer *lexer)
{
    free((char *)lexer -> text);
    if (lexer -> current_token.value) 
    {
        free(lexer -> current_token.value);
    }
    free(lexer);
}   

char peek(Lexer *lexer, int offset) 
{
    return lexer->text[lexer->current_pos + offset];
}

void advance(Lexer *lexer, int steps)
{
    lexer->current_pos += steps;
}

void reverse(Lexer *lexer, int steps)
{
    lexer->current_pos -= steps;
}

void skip_whitespace(Lexer *lexer)
{
    while (isspace(peek(lexer, 0))) 
    {
        advance(lexer,1);
    }
}

void skip_comment(Lexer *lexer)
{
    // single #-single line comment and ##-double means multi-line comment
    if (peek(lexer, 0) == '#' || (peek(lexer,0) == '#' && peek(lexer, 1) == '#') )
    {
        while (peek(lexer, 0) != '\n' && peek(lexer, 0) != '\0')
        {
            advance(lexer,1);
        }
    } 
    else if (peek(lexer, 0) == '#') 
    {
        while (peek(lexer, 0) != '\n' && peek(lexer, 0) != '\0')
        {
            advance(lexer, 1);
        }
    }
}

Token get_number(Lexer *lexer)
{
    //hopefully this should work with 0.1 1.0 and even .1 and 1, 12 of course
    int start_pos = lexer->current_pos;
    int dot_only = -1;
    while (isdigit(peek(lexer,0)))
    {
        advance(lexer, 1);
    }

    if (peek(lexer, 0) == '.' && isdigit(peek(lexer, 1)))
    {   
        if (!isdigit(peek(lexer, (lexer->current_pos == 0) ? 0 : -1)))
        {
            dot_only += 1;
        }
        advance(lexer,1);
        while (isdigit(peek(lexer, 0)))
        {
            advance(lexer, 1);
        }
    }
    else if (peek(lexer, 0) == '.' && !isdigit(peek(lexer, 1)))
    {
        advance(lexer, 1);
        return (Token){ERRORTOKEN, NULL}; // Error Handling should be done
    }
    

    int end_pos = lexer->current_pos;
    int length = end_pos - start_pos;
    if (dot_only == 0)
    {
        char *value = malloc(length +2);
        strcpy(value, "0");
        strncpy(value + 1, lexer->text + start_pos, length);
        value[length + 1] = '\0';
        return (Token) {NUMBER, value};

    } else 
    {
        char *value = malloc(length +1);
        strncpy(value, lexer->text + start_pos, length);
        value[length] = '\0';
        return (Token) {NUMBER, value};
    }
    
}

Token get_string(Lexer *lexer) {
    advance(lexer, 1);
    int start_pos = lexer->current_pos;
    while (peek(lexer,0) != '"' && peek(lexer,0) != '\''  && peek(lexer, 0) != '\0')
    {
        advance(lexer, 1);
    }

    int end_pos = lexer->current_pos;
    int length = end_pos - start_pos;
    char *value = malloc(length + 1);
    strncpy (value, lexer->text + start_pos, length);
    value[length] = '\0';
    advance(lexer,1);
    return (Token) {STRING, value};
}

Token get_identifer(Lexer *lexer)
{
    int start_pos = lexer->current_pos;
    while (isalnum(peek(lexer, 0)) || peek(lexer, 0) == '_')
    {
        advance(lexer, 1);
    }
    int end_pos = lexer->current_pos;
    int length = end_pos - start_pos;
    char *value = malloc (length +1);
    strncpy(value, lexer->text + start_pos, length);
    value[length] = '\0';
    return (Token) {ID, value};
}

Token get_Onechar(Lexer* lexer)
{
    
    switch (peek(lexer, 0))
    {
    case '(':
        advance(lexer,1);
        return (Token) {LPAREN, strdup("(")};
    case ')':
        advance(lexer,1);
        return (Token) {RPAREN, strdup(")")};
    case '[':
        advance(lexer,1);
        return (Token) {LBRACKET, strdup("[")};
    case ']':
        advance(lexer,1);
        return (Token) {RBRACKET, strdup("]")};
    case '{':
        advance(lexer,1);
        return (Token) {LBRACE, strdup("{")};
    case '}':
        advance(lexer,1);
        return (Token) {RBRACE, strdup("}")};
    case ',':
        advance(lexer,1);
        return (Token) {COMMA, strdup(",")};
    case ';':
        advance(lexer,1);
        return (Token) {SEMI, strdup(";")};
    case ':':
        advance(lexer,1);
        return (Token) {COLON, strdup(":")};
    case '+':
        advance(lexer, 1);
        return (Token) {PLUS, strdup("+")};
    case '-':
        advance(lexer, 1);
        return (Token) {MINUS, strdup("-")};
    case '*':
        advance(lexer, 1);
        return (Token) {STAR, strdup("*")};
    case '/':
        advance(lexer, 1);
        return (Token) {FSLASH, strdup("/")};
    case '^':
        advance(lexer, 1);
        return (Token) {CARET , strdup("^")};
    case '=':
        advance(lexer, 1);
        return (Token) {EQUAL, strdup("=")};
    default:
        break;
    }
    advance(lexer, 1);
    return (Token) {NONE, NULL};
}

Token get_Twochar(Lexer* lexer)
{
    switch (peek(lexer, 0))
    {
    case '=':
        switch (peek(lexer, 1))
        {
            case '=':
            advance(lexer, 2);
            return (Token) {EQUALEQUAL, strdup("==")};
        }
        break;
    case '+':
        switch (peek(lexer, 1))
        {
            case '=':
            advance(lexer, 2);
            return (Token) {PLUSEQUAL, strdup("+=")};
        }
        break;
    case '-':
        switch (peek(lexer, 1))
        {
            case '=':
            advance(lexer, 2);
            return (Token) {MINUSEQUAL, strdup("-=")};
        }
        break;
    case '*':
        switch (peek(lexer, 1))
        {
            case '=':
            advance(lexer, 2);
            return (Token) {STAREQUAL, strdup("*=")};
        }
        break;
    case '/':
        switch (peek(lexer, 1))
        {
            case '=':
            advance(lexer, 2);
            return (Token) {FSLASHEQUAL, strdup("/=")};
        }
        break;
    case '^':
        switch (peek(lexer, 1))
        {
            case '=':
            advance(lexer, 2);
            return (Token) {CARET, strdup("^=")};
        }
        break;
    case ':':
        switch (peek(lexer, 1))
        {
            case '=':
            advance(lexer, 2);
            return (Token) {COLONEQUAL, strdup(":=")};
        }
        break;
    default:
        break;
    }
    return (Token) {NONE, NULL};
}

Token get_next_token(Lexer * lexer) 
{   
    // Skip whitespaces
    skip_whitespace(lexer);

    /* Check for End OF File */
    if( peek(lexer, 0) == '\0')
    {
        return (Token) {ENDMARKER, NULL};
    }

    /* Skip Comment */
    if ( peek(lexer, 0) == '#' || ( peek(lexer, 0) == '#' &&  peek(lexer, 1) == '#'))
    {
        skip_comment(lexer);
        return get_next_token(lexer);
    }
     /* Handle Number*/
    if (isdigit( peek(lexer, 0)) ||  peek(lexer, 0) == '.')
    {
       return get_number(lexer);
    }

     /* Handle String*/
    if (peek(lexer, 0) == '"' || peek(lexer, 0) == '\'')
    {
        
        return get_string(lexer);
    }
    
    if (isalpha( peek(lexer, 0)) ||  peek(lexer, 0) == '_')
    {
        return get_identifer(lexer);
    }
    
     /* Check for Two Character Tokens */
    Token c = get_Twochar(lexer);

    if (c.type == NONE)
    {
        /* Check for One Character Tokens */
        return get_Onechar(lexer);
    }
    else
    {
        return c;
    }


    
}