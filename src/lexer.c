#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "include/lexer.h"

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
    //this should for now only handle strings inside "" (double quotes)
    int start_pos = lexer->current_pos;
    while (peek(lexer,0) != '"' && peek(lexer, 0) != '\0')
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

Token get_next_token(Lexer * lexer) 
{
    skip_whitespace(lexer);
    if(peek(lexer, 0) == '\0')
    {
        return (Token) {ENDMARKER, NULL};
    }
    if (peek(lexer, 0) == '#' || (peek(lexer, 0) == '#' && peek(lexer, 1) == '#'))
    {
        skip_comment(lexer);
        return get_next_token(lexer);
    }

    switch (peek(lexer, 0))
    {
    case '+':
        advance(lexer,1);
        return (Token) {PLUS, NULL};
    case '-':
        advance(lexer,1);
        return (Token) {MINUS, NULL};
    case '*':
        advance(lexer,1);
        return (Token) {STAR, NULL};
    case '/':
        advance(lexer,1);
        return (Token) {FSLASH, NULL};
    case '(':
        advance(lexer,1);
        return (Token) {LPAREN, NULL};
    case ')':
        advance(lexer,1);
        return (Token) {RPAREN, NULL};
    case ',':
        advance(lexer,1);
        return (Token) {COMMA, NULL};
    case '=':
        advance(lexer,1);
        return (Token) {EQUAL, NULL};

    //DOT is a bit tricky case as we have DOT in .1 and func.maethod so I'll deal with it later
    case '.':
        //advance(lexer,1);
        return get_number(lexer);

    default:
        if (isdigit(peek(lexer, 0)))
        {
            return get_number(lexer);
        } 
        else if (peek(lexer, 0) == '"')
        {
            return get_string(lexer);
        } 
        else if (isalpha(peek(lexer, 0)) || peek(lexer,0) == '_')
        {
            return get_identifer(lexer);
        }
        else
        {
            advance(lexer, 1);
            return (Token){NONE, NULL};
        }
    }
}