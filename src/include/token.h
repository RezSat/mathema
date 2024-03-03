#ifndef TOKEN_H
#define TOKEN_H

#define NONE         0
#define NAME         1
#define NUMBER       2
#define STRING       3
#define ID           4
#define PLUS         5
#define PLUSEQUAL    6
#define MINUS        7
#define MINUSEQUAL   8
#define STAR         9
#define STAREQUAL   10
#define GREATER     11
#define LESS        12
#define CARET       13
#define VBAR        14
#define PERCENT     15
#define FSLASH      16
#define FSLASHEQUAL 17
#define BSLASH      18
#define LPAREN      19
#define RPAREN      20
#define LBRACKET    21
#define RBRACKET    22
#define LBRACE      23
#define RBRACE      24
#define EQUAL       25
#define EQUALEQUAL  26
#define COMMA       27
#define COLON       28
#define COLONEQUAL  29
#define DOT         30
#define SEMI        31
#define UNDERSCORE  32
#define HASH        33
#define EXCLAMATION 34
#define AMPERSAND   35
#define ENDMARKER   36
#define ERRORTOKEN  37

//changes to here need to be applied into lexer.c (tokenNames[]) as well.

typedef struct {
    int type;
    char *value;
    //add type name, linemo, line, etc.
} Token;

extern char* tokenNames[];

#endif /* TOKEN_H */