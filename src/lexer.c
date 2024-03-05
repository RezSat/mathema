#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "include/lexer.h"
#include "include/helper.h"


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
    lexer->line_start = 0;
    lexer->line_end = 0;
    lexer->current_token.type = NONE;
    lexer->current_token.value = NULL;
    lexer->current_token.lineno = 1;
    lexer->current_token.line = NULL;
    lexer->current_token.start_pos = (Location){lexer->current_token.lineno,lexer->current_pos};
    lexer->current_token.end_pos = (Location){lexer->current_token.lineno,lexer->current_pos};

    //Find the end position of the first line
    while (text[lexer->current_pos] != '\n' && text[lexer->current_pos] != '\0')
    {
        lexer->line_end++;
        lexer->current_pos++;
    }
    lexer->current_pos = 0;
    lexer->current_token.line = malloc(lexer->line_end + 1);
    strncpy(lexer->current_token.line, text, lexer->line_end);
    lexer->current_token.line[lexer->line_end] = '\0';
    //printf("%s\n\n",lexer->current_token.line);
    return lexer;
}

void free_lexer(Lexer *lexer)
{
    free((char *)lexer -> text);
    if (lexer -> current_token.value) 
    {
        free(lexer -> current_token.value);
    }
    if (lexer -> current_token.line)
    {
        free(lexer -> current_token.line);
    }
    free(lexer);
}   

char peek(Lexer *lexer, int offset) 
{
    //printf("%s\n\n", lexer->current_token.line);
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
        return (Token){ERRORTOKEN, NULL, lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}}; // Error Handling should be done
    }
    

    int end_pos = lexer->current_pos;
    int length = end_pos - start_pos;
    if (dot_only == 0)
    {
        char *value = malloc(length +2);
        strcpy(value, "0");
        strncpy(value + 1, lexer->text + start_pos, length);
        value[length + 1] = '\0';
        return (Token) {NUMBER, value, lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,end_pos}};

    } else 
    {
        char *value = malloc(length +1);
        strncpy(value, lexer->text + start_pos, length);
        value[length] = '\0';
        return (Token) {NUMBER, value, lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,end_pos}};
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
    return (Token) {STRING, value, lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,end_pos}};
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
    return (Token) {ID, value, lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,end_pos} };
}

Token get_Onechar(Lexer* lexer)
{   
    int start_pos = lexer->current_pos;
    
    switch (peek(lexer, 0))
    {
    case '(':
        advance(lexer,1);
        return (Token) {LPAREN, duplicate_string("("), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
    case ')':
        advance(lexer,1);
        return (Token) {RPAREN, duplicate_string(")"), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
    case '[':
        advance(lexer,1);
        return (Token) {LBRACKET, duplicate_string("["), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
    case ']':
        advance(lexer,1);
        return (Token) {RBRACKET, duplicate_string("]"), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
    case '{':
        advance(lexer,1);
        return (Token) {LBRACE, duplicate_string("{"), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
    case '}':
        advance(lexer,1);
        return (Token) {RBRACE, duplicate_string("}"), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
    case ',':
        advance(lexer,1);
        return (Token) {COMMA, duplicate_string(","), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
    case ';':
        advance(lexer,1);
        return (Token) {SEMI, duplicate_string(";"), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
    case ':':
        advance(lexer,1);
        return (Token) {COLON, duplicate_string(":"), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
    case '+':
        advance(lexer, 1);
        return (Token) {PLUS, duplicate_string("+"), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
    case '-':
        advance(lexer, 1);
        return (Token) {MINUS, duplicate_string("-"), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
    case '*':
        advance(lexer, 1);
        return (Token) {STAR, duplicate_string("*"), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
    case '/':
        advance(lexer, 1);
        return (Token) {FSLASH, duplicate_string("/"), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
    case '^':
        advance(lexer, 1);
        return (Token) {CARET , duplicate_string("^"), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
    case '=':
        advance(lexer, 1);
        return (Token) {EQUAL, duplicate_string("="), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
    default:
        break;
    }
    advance(lexer, 1);
    return (Token) {NONE, NULL, lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
}

Token get_Twochar(Lexer* lexer)
{
    int start_pos = lexer->current_pos;
    switch (peek(lexer, 0))
    {
    case '=':
        switch (peek(lexer, 1))
        {
            case '=':
            advance(lexer, 2);
            return (Token) {EQUALEQUAL, duplicate_string("=="), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
        }
        break;
    case '+':
        switch (peek(lexer, 1))
        {
            case '=':
            advance(lexer, 2);
            return (Token) {PLUSEQUAL, duplicate_string("+="), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
        }
        break;
    case '-':
        switch (peek(lexer, 1))
        {
            case '=':
            advance(lexer, 2);
            return (Token) {MINUSEQUAL, duplicate_string("-="), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
        }
        break;
    case '*':
        switch (peek(lexer, 1))
        {
            case '=':
            advance(lexer, 2);
            return (Token) {STAREQUAL, duplicate_string("*="), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
        }
        break;
    case '/':
        switch (peek(lexer, 1))
        {
            case '=':
            advance(lexer, 2);
            return (Token) {FSLASHEQUAL, duplicate_string("/="), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
        }
        break;
    case '^':
        switch (peek(lexer, 1))
        {
            case '=':
            advance(lexer, 2);
            return (Token) {CARET, duplicate_string("^="), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
        }
        break;
    case ':':
        switch (peek(lexer, 1))
        {
            case '=':
            advance(lexer, 2);
            return (Token) {COLONEQUAL, duplicate_string(":="), lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
        }
        break;
    default:
        break;
    }
    return (Token) {NONE, NULL, lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, start_pos}, {lexer->current_token.lineno,lexer->current_pos}};
}

Token get_next_token(Lexer * lexer) 
{   

    if (peek(lexer, 0) == '\n')
    {
        lexer->current_token.lineno++;
        lexer->line_start = lexer->current_pos;
        free(lexer->current_token.line);
        while (lexer->text[lexer->line_start] != '\n' && lexer->text[lexer->line_start] != '\0')
        {
            lexer->line_end++;
            lexer->current_pos++;
        }
        lexer->current_pos = lexer->line_start;
        lexer->current_token.line = malloc(lexer->line_end - lexer->line_start + 1);
        strncpy(lexer->current_token.line, lexer->text, ((lexer->line_end - lexer->line_start)));
        lexer->current_token.line[lexer->line_end - lexer->line_start] = '\0';
    
    }

    // Skip whitespaces
    skip_whitespace(lexer);

    /* Check for End OF File */
    if( peek(lexer, 0) == '\0')
    {
        lexer->current_token.lineno++;
        return (Token) {ENDMARKER, NULL, lexer->current_token.lineno, lexer->current_token.line, {lexer->current_token.lineno, 0}, {lexer->current_token.lineno,0}};
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
        Token number = get_number(lexer);
        number.line = lexer->current_token.line;  // Set line information for the number token
        return number;
    }

     /* Handle String*/
    if (peek(lexer, 0) == '"' || peek(lexer, 0) == '\'')
    {
        
        Token string = get_string(lexer);
        string.line = lexer->current_token.line;  // Set line information for the string token
        return string;
    }
    
    if (isalpha( peek(lexer, 0)) ||  peek(lexer, 0) == '_')
    {
        Token identifier = get_identifer(lexer);
        identifier.line = lexer->current_token.line;  // Set line information for the identifier token
        return identifier;
    }
    
     /* Check for Two Character Tokens */
    Token twoChar = get_Twochar(lexer);
    

    if (twoChar.type == NONE)
    {
        /* Check for One Character Tokens */
        Token oneChar = get_Onechar(lexer);
        oneChar.line = lexer->current_token.line;  // Set line information for the one character token
        return oneChar;
    }
    else
    {
        twoChar.line = lexer->current_token.line;
        return twoChar;
    }


    
}