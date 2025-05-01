#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

enum token_kind_t {
    TOKEN_UNKNOWN,
    TOKEN_EOF,
    
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_SWITCH,
    TOKEN_DO,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_BREAK,
    TOKEN_CONTINUE,
    TOKEN_RETURN,
    TOKEN_GOTO,
    TOKEN_DEFER,

    TOKEN_STRUCT,
    TOKEN_UNION,
    TOKEN_ENUM,
    TOKEN_TYPEDEF,

    TOKEN_U8,
    TOKEN_U16,
    TOKEN_U32,
    TOKEN_U64,
    TOKEN_I8,
    TOKEN_I16,
    TOKEN_I32,
    TOKEN_I64,
    TOKEN_F32,
    TOKEN_F64,

    TOKEN_IDENTIFIER,
    TOKEN_INT,
    TOKEN_RAW_INT,
    TOKEN_STRING,
    TOKEN_RAW_STRING,
    TOKEN_CHAR,

    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_SEMICOLON
};

typedef struct {
    const char *str;
    size_t len;
} slice_t;

union token_as_t {
    slice_t slice;
};

typedef struct {
    slice_t slice;
    enum token_kind_t kind;
    union token_as_t as;
} token_t;

#endif //TOKEN_H
