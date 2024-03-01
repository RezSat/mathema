#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    NONE,
    NAME,
    NUMBER,
    STRING,
    ID,
    PLUS,
    MINUS,
    STAR,
    FSLASH,
    LPAREN,
    RPAREN,
    EQUAL,
    COMMA,
    ENDMARKER
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;

#endif /* TOKEN_H */