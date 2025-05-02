#include <stdio.h>

#include "syntax/lex.h"

int main() {
    const char *src = "r###\"###\"##\"###uwu if";
    for (token_t token = lex(src); token.kind != TOKEN_EOF; token = lex(src)) {
        src = token.slice.str + token.slice.len;
        printf("token.kind = %d\n", token.kind);
        printf("token.slice.str = %.*s\n", (int) token.slice.len, token.slice.str);
        printf("token.as.slice.str = %.*s\n", (int) token.as.slice.len, token.as.slice.str);
    }

    return 0;
}
